//ReactorClass.h

#ifndef _REACTORCLASS_h
#define _REACTORCLASS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
class ReactorClass {

public:


	
	ReactorClass(int ChillPinSupply, int HeatPinSupply, int ChillPinReturn, int HeatPinReturn,String ID,int ArgonPin);

	void init(int initValue, String ChillPic, String HeatPic, String IdlePic);
	
	void update(String Component, int valueInt, float valueFloat, String ReactorNum,float TempSpan);
	void ControlDirection(String ReactorName);
	void manualControl();
	void AutoControl();

	void Chill();
	void Heat();
	void Idle();
	void SendPicToNextion(String Pic);
	void SendValueToNextion(String ComponentName, String value);
	void SendTempToNextion();
	void buttonLow(String Component);
	void buttonHigh(String Component);
	void updateButton(String Component, int valueInt);

	
	/*static int WashCycle(pt* pt);*/

	float _Temp;
	int i;
	int _WashCycleWarm;
	int _WashCycleCool;
	int _ChillPinSupply;
	int _HeatPinSupply;
	int _ChillPinReturn;
	int _HeatPinReturn;
	int _manChillState;
	int _manWarmState;
	int _enableState;
	int _ArgonState;
	float _SetPointHigh;
	float _SetPointLow;
	int _ArgonPin;
	int _washCycleState;
	float _washCycleHigh;
	float _washCycleLow;

	String _ReactorID;
	String _ChillPic;
	String _HeatPic;
	String _IdlePic;
	String endChar = String(char(0xff)) + String(char(0xff)) + String(char(0xff));
private:
	int _TimesRanHeat;
	int _TimesRanChill;
	int _TimesRanIdle;
};

#endif

