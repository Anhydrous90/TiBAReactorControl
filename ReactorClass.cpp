#include "Arduino.h"
#include "ReactorClass.h"
#include <pt.h>
ReactorClass::ReactorClass(int ChillPinSupply, int HeatPinSupply, int ChillPinReturn,
    int HeatPinReturn,String ReactorID,int ArgonPin)
{
    Serial.println("ConstructorRan");
    _ReactorID = ReactorID;
    _ChillPinSupply = ChillPinSupply;
    _ChillPinReturn = ChillPinReturn;
    _HeatPinReturn = HeatPinReturn;
    _HeatPinSupply = HeatPinSupply;
    _ArgonPin = ArgonPin;
    
}

void ReactorClass::init(int initValue, String ChillPic, String HeatPic, String IdlePic) {
    _manChillState = initValue;
    _manWarmState = initValue;
    _enableState = initValue;
    _ArgonState = initValue;
    _washCycleState = initValue;

    _ChillPic = ChillPic;
    _HeatPic = HeatPic;
    _IdlePic = IdlePic;
    static struct pt pt;
    PT_INIT(&pt)

    _Temp = 00.00;
    _washCycleHigh=30.00;
    _washCycleLow=15.00;
    _SetPointHigh=30;
    _SetPointLow=25;

    pinMode(_ChillPinSupply, OUTPUT);
    pinMode(_HeatPinSupply, OUTPUT);
    pinMode(_ChillPinReturn, OUTPUT);
    pinMode(_HeatPinReturn, OUTPUT);
    digitalWrite(_ChillPinSupply, LOW);
    digitalWrite(_ChillPinReturn, HIGH);
    digitalWrite(_HeatPinSupply, LOW);
    digitalWrite(_HeatPinReturn, HIGH);

    Serial.println("Reactor "+_ReactorID+ " Initialized");
   
}






void ReactorClass::update(String Component, int valueInt, float valueFloat, String ReactorNum,float TempSpan)
{
    
    if (Component == "HighLimit") {
        _SetPointHigh = valueFloat / 100;
        _SetPointLow = _SetPointHigh - TempSpan;

        SendValueToNextion( "LowLimit", String(_SetPointLow * 100));
    }
    else if (Component == "ManualWarm") {
        _manWarmState = valueInt;
        SendPicToNextion(_HeatPic); 
      

    }
    else if (Component == "ManualChill") {

        _manChillState = valueInt;
        SendPicToNextion(_ChillPic);
       
    }
    else if (Component == "Enable") {
        _enableState = valueInt;
        updateButton("ManChill", valueInt);
    }
    else if (Component == "Argon") {
        _ArgonState = valueInt;
        digitalWrite(_ArgonPin,_ArgonState);
    }
    else if (Component == "WashCycle") {
        _washCycleState = valueInt;
    }
    else {
        Serial.println("UpdateRan error" + ReactorNum);
    }
    Component = "";
    valueInt = 0;
    valueFloat = 0;

}



void ReactorClass::ControlDirection(String ReactorName) {
    if (_enableState == 1) {

        AutoControl();

    }
    if (_manChillState != 0 || _manWarmState != 0)
    {

        manualControl();
       
    }else if(_washCycleState==1) {

       
       
    }
    else {
        Idle();
    }

}


void ReactorClass::manualControl() {
    Serial.println("ManualControlRunning" + _ReactorID);
    if (_manChillState == 1 && _manWarmState == 1) {    //Both cant be on at same time,turn off both

        Idle();

        _manChillState = 0;
        _manWarmState = 0;
        
        updateButton("ManWarm", 0);
        updateButton("ManChill", 0);

        Serial.println("Both R2 Manual Valves enabled. Both closed");

    }
    else if (_manChillState == 1 && _manWarmState == 0) {
        
        Chill();
        Serial.println("Case:Chilling Manually");
        SendPicToNextion(_ChillPic);
    }
    else if (_manChillState == 0 && _manWarmState == 1) {
        Heat();
        Serial.println("Case:Warming Manually");
    }
    else if (_manChillState == 0 && _manWarmState == 0) {

        Serial.println("Both Manual Valves closed");
        Idle();
        
    }
    else
    {
        Serial.println("error:manualControl");
    }

}

void ReactorClass::AutoControl() {

    if (_manChillState == 0) {

        if (_Temp > _SetPointHigh)
        {

            Chill();
        }
        else if (_Temp < _SetPointLow)
        {
            Heat();
        }
        else {
            Idle();
        }

        if (_manChillState == 1) {

            if (_Temp > _SetPointHigh)
            {
                Chill();
            }
            else if (_Temp < _SetPointLow)
            {
                Heat();
            }
            else {
                
            }
        }
    }
}

//void ReactorClass::WashCycle(pt* pt) {
//
//
//}

void ReactorClass::Chill() {
    if (_TimesRanChill == 0) {
        SendPicToNextion(_HeatPic);
        
    }
    digitalWrite(_ChillPinSupply, HIGH);
    digitalWrite(_ChillPinReturn, LOW);
    digitalWrite(_HeatPinSupply, LOW);
    digitalWrite(_HeatPinReturn, HIGH);
    SendPicToNextion(_ChillPic);
    _TimesRanHeat = 0;
    _TimesRanIdle = 0;
    _TimesRanChill = 1;
    

}

void ReactorClass::Heat()
{
    if (_TimesRanHeat == 0) {
        SendPicToNextion(_HeatPic);

    }
    digitalWrite(_ChillPinSupply, LOW);
    digitalWrite(_ChillPinReturn, HIGH);
    digitalWrite(_HeatPinSupply, HIGH);
    digitalWrite(_HeatPinReturn, LOW);
    SendPicToNextion(_HeatPic);
    _TimesRanIdle = 0;
    _TimesRanChill = 0;
    _TimesRanHeat = 1;
}



void ReactorClass::Idle() {
    if (_TimesRanIdle == 0) {
        SendPicToNextion(_HeatPic);

    }
    digitalWrite(_ChillPinSupply, LOW);
    digitalWrite(_ChillPinReturn, HIGH);
    digitalWrite(_HeatPinSupply, LOW);
    digitalWrite(_HeatPinReturn, HIGH);
    SendPicToNextion(_IdlePic);
    _TimesRanHeat = 0;
    _TimesRanChill = 0;
    _TimesRanIdle = 1;

}


void ReactorClass::SendPicToNextion(String Pic) {
    
            Serial2.print(_ReactorID + "Pic.pic=" + Pic + endChar);
    
        }

void ReactorClass::SendValueToNextion(String ComponentName, String value) {
    
    
            Serial2.print(_ReactorID + ComponentName + ".val=" + String(value) + endChar);
    
        }
void ReactorClass::SendTempToNextion() {


    Serial2.print(String(_ReactorID) + "Temp.val=" + String(_Temp*100) + endChar);

}
void ReactorClass::buttonLow(String Component) {

    Serial2.print(_ReactorID +Component+ ".val=0" + endChar);

}
void ReactorClass::buttonHigh(String Component) {

    Serial2.print(_ReactorID + Component + ".val=1" + endChar);

}

void ReactorClass::updateButton(String Component, int valueInt) {
    if (valueInt == 0)
    {
        buttonLow(Component);
    }
    else {
        buttonHigh(Component);
    }


     

}
