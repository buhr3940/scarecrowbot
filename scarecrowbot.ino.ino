// This #include statement was automatically added by the Particle IDE.
#include "Tinker/Tinker.h"



STARTUP(System.enableFeature(FEATURE_RETAINED_MEMORY));

int inputPin = D0;              // choose the input pin (for PIR sensor)
int ledPin = D1;                // LED Pin
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
int buzzerPin = D2;
int calibrateTime = 10000;      // wait for the thingy to calibrate
int frequency = 1;
int repeatTime = 900000;
String aMotion;
int detectCount = 0;
int firstCount = 0;
int repeatCount = 0;
int diff = 0;
int tStart = 0;
int tStop = 0;
int t = 0;
bool sleepOn = false;
String sleepOnS = "off";
int setFreq (String limit);
int setSleep (String command);     // Set the deep sleep mode


void setup()

{
    
  Serial.begin(9600);    
  pinMode( ledPin, OUTPUT );
  pinMode(inputPin, INPUT);     // declare sensor as input
  Particle.variable ("pirState", pirState);
  Particle.variable ("frequency", frequency);
  Particle.variable ("repeatCount", repeatCount);
  Particle.variable ("detectCount", detectCount);
  Particle.variable ("sleepOnS", sleepOnS);
  
  Particle.function("set_Freq", setFreq);
  Particle.function("set_Sleep", setSleep);
 
}

void gotoSleep() {
    System.sleep(D0,RISING,57600);
}

void setSleepT() {
  
  tStart = millis();
  
     
  if ((Time.hour() > 16) || (Time.hour() < 8 )){
    //  delay(2);  
    //  gotoSleep();
      sleepOn = true;
    }
  if (detectCount == 0) {
     t = tStart - millis(); 
  
    if (t==1800000) {
      gotoSleep();
      sleepOn = true;
      }
    }
  if (sleepOnS == "on"){
      delay(2);
      System.sleep(SLEEP_MODE_DEEP, 30);
      sleepOn = true;
    }
    
  if (sleepOnS == "off"){
      sleepOn = false;
  }
}

void loop()
{
     // if the sensor is calibrated
  if ( calibrated() )
  {
  // get the data from the sensor
    readTheSensor();

    // report it out, if the state has changed
    
    reportTheData();
  }
  
  setSleepT();
  
}

void readTheSensor() {
  val = digitalRead(inputPin);
}

bool calibrated() {
  return millis() - calibrateTime > 0;
}



void reportTheData() {

  // if the sensor reads high
  // or there is now motion
  if (val == HIGH) {
   
    // the current state is no motion
    // i.e. it's just changed
    // announce this change by publishing an eent
    if (pirState == LOW) {
      // we have just turned on
      detectCount++;
      firstCount = millis();
      Particle.publish("motion", aMotion);
      
      // Update the current state
      pirState = HIGH;
      setLED( pirState );
      diff = millis() - firstCount;
      if (detectCount > 1 && diff <= repeatTime) {
        Particle.publish ("repeat");
        repeatCount++;
        delay(600);
        frequency = frequency + 100;
        }
    
    }
  } else {
    if (pirState == HIGH) {
        tone(buzzerPin, frequency, 500);
      // we have just turned of
      // Update the current state
      pirState = LOW;
      setLED( pirState );
    }
  }
}

// Sets temperature scale. Returns 1 if input is "F" or "C",

// otherwise returns -1.

int setSleep (String command){

  command.toLowerCase();


  if( command == "on" ) {
        
         sleepOnS = command;
         
    return 1;

  } else {

    return -1;

  }

}


// Set the frequency

int setFreq(String limit){

  frequency = limit.toInt();

  return 1;

}

void setLED( int state )
{
  digitalWrite( ledPin, state );
}
