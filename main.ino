
//TODO: Add nextion responses
//TODO: add Reaction Timer
//TODO: add Wash Cycle Controller
//TODO: Create 6 protohreads in main function




#include <SoftwareSerial.h>
#include <SPI.h>
#include <pt100rtd.h>
#include <Adafruit_MAX31865.h>
#include <Controllino.h>
#include <pt.h>
#include "ReactorClass.h"
#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor



#define RREFA 430.0//These must be accurate
#define RREFB 430.0//These must be accurate
#define RREFC 430.0//These must be accurate

#define C2F(c) ((9 * c / 5) + 32)
#define Base 0

#define ArgonOn = CONTROLLINO_R10;
#define R2ChillPinSupply = CONTROLLINO_R12;
#define R3ChillPinSupply = CONTROLLINO_R14;
#define R1HeatPinSupply = CONTROLLINO_R11;
#define R2HeatPinSupply = CONTROLLINO_R13;
#define R3HeatPinSupply = CONTROLLINO_R15;
#define R1ChillPinReturn = CONTROLLINO_R0;
#define R2ChillPinReturn = CONTROLLINO_R2;
#define R3ChillPinReturn = CONTROLLINO_R4;
#define R1HeatPinReturn = CONTROLLINO_R1;
#define R2HeatPinReturn = CONTROLLINO_R3;
#define R3HeatPinReturn = CONTROLLINO_R5;
#define R1ArgonOnPin = CONTROLLINO_R6;
#define R2ArgonOnPin = CONTROLLINO_R7;
#define R3ArgonOnPin = CONTROLLINO_R8;


// initialize the Stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

int stepCount = 0;  // number of steps the motor has taken


String endChar = (String(char(0xff)) + String(char(0xff)) + String(char(0xff)));
int stringCount = 0;
String  dfd = ""; // data from display
int valueInt;
float valueFloat;

String R1ChillPic = "2";
String R2ChillPic = "0";
String R3ChillPic = "0";
String R1HeatPic = "3";
String R2HeatPic = "1";
String R3HeatPic = "1";
String R1IdlePic = "6";
String R2IdlePic = "5";
String R3IdlePic = "5";





pt100rtd PT100 = pt100rtd();
String Component;
String value;
String dfdIndex[30];

unsigned long time = 0;

//Temp Object Constructor
Adafruit_MAX31865 max31865A = Adafruit_MAX31865(CONTROLLINO_D15);


//Reactor Object Constructor
ReactorClass R1(CONTROLLINO_R10, CONTROLLINO_R11, CONTROLLINO_R0, CONTROLLINO_R1, "R1",CONTROLLINO_R6);




// NOTE : Initial Async Delay
unsigned	long initDelayLong = 1000;
unsigned	long currentMillis = 0;// NOTE : 4,294,967,295
int	initDelayLength = 2000;
unsigned long RxnDuration=100;
unsigned long RxnDurationMillis;
unsigned long WashDuration=100;
unsigned long WashDurationMillis;
float TempSpan = 5.00;




void setup() {

    Serial.begin(115200);
    delay(50);
    Serial2.begin(57600);
    delay(50);

    //Reactor Object Initialization
    R1.init(0, R1ChillPic, R1HeatPic, R1IdlePic);
   

    //Temperature Initialization
    max31865A.begin(MAX31865_3WIRE);
    delay(10);





    delay(50);

    Serial2.print("rest" + endChar);


    Serial.println("Setup Complete");



}

void loop() {
     int sensorReading = analogRead(A0);
  // map it to a range from 0 to 100:
  int motorSpeed = map(sensorReading, 0, 1023, 0, 100);
  // set the motor speed:
  if (motorSpeed > 0) {
    myStepper.setSpeed(motorSpeed);
    // step 1/100 of a revolution:
    myStepper.step(stepsPerRevolution / 100);
  }

    if (Serial2.available()) {
        serialInput();
    }

    // NOTE : ASYNC DELAY
    if (millis() > initDelayLong) {
        initDelayLong += initDelayLength;
        gettempR1();
     
        
        R1.ControlDirection("R1");


        sendTemps();

        currentMillis = millis();

     
      
    }

   
    }






void serialInput() {
    dfd += char(Serial2.read());
 
    if (dfd.length() > 2 && dfd.substring(0, 3) != "COM")//This filters out the broken messages
        dfd = "";
    else {
        // NOTE : If string ends in a ? then command completed
        if (dfd.substring((dfd.length() - 1), dfd.length()) == "?")
        {
            String ReactorName = "";
            String Component = "";
            String value = "";  // NOTE : Get the value string
            int valueInt = 0; // convert string to int
            float valueFloat = 0;//
            int stringCount = 0;
            while (dfd.length() > 0)
            {


                int index = dfd.indexOf(' ');
                if (index == -1) // No space found
                {
                    dfdIndex[stringCount++] = dfd;
                    break;
                }
                else
                {
                    dfdIndex[stringCount++] = dfd.substring(0, index);
                    dfd = dfd.substring(index + 1);
                }
            }

            /*___COMStart___Reactor___Component____Value*/

            ReactorName = dfdIndex[1];
            Component = dfdIndex[2];// NOTE : Get the Component
            value = dfdIndex[3];  // NOTE : Get the value string
            valueInt = value.toInt(); // convert string to int
            valueFloat = value.toFloat();// convert string to float

            Serial.println(ReactorName + Component + ":    " + valueInt + "," + valueFloat);

            Serial2.print("dataPic.pic=12" + endChar);


            ComponentFilter(ReactorName, Component, valueInt);
            dfd = "";
        }

    }
}


void ComponentFilter(String ReactorName, String Component, int valueInt) {
   
        Serial.println("ComponentFilter Ran = " + Component);
        R1.update(Component, valueInt, valueFloat, ReactorName,TempSpan);
        updateGlobal(Component, valueInt, valueFloat, ReactorName);
 


}




void updateGlobal(String Component, int valueInt, float valueFloat, String Reactor)
{

    if (Component == "TempSpan") {
        TempSpan = valueFloat / 100;
    }else if (Component == "RxnDuration") {
        RxnDuration = valueInt;
        RxnDurationMillis = RxnDuration*60000; 
    } else if (Component == "WashDuration") {
        WashDuration = valueInt;
        WashDurationMillis = WashDuration*60000;//Wash Duraation is now in milliseconds

    }
    else {

    }
    Serial.println("UpdateGlobalRan");
}
void sendTemps() {

    R1.SendTempToNextion();

}

void getTemp() {
    uint16_t rtd = 0;
    uint16_t ohmsx100 = 0;
    uint32_t dummy = 0;
    float Tlut = 0.00;
    float RunningTotal1 = 0;

    for (int i = 0; i <= 9; i++) {


        rtd = max31865A.readRTD();
        dummy = ((uint32_t)(rtd << 1)) * 100 * ((uint32_t)floor(RREFA));
        dummy >>= 16;
        ohmsx100 = (uint16_t)(dummy & 0xFFFF);
        Tlut = PT100.celsius(ohmsx100);
        RunningTotal1 += Tlut;

    }

    R1._Temp = RunningTotal1 / 10;




}




