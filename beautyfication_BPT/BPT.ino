#include <Wire.h>
#include "max32664.h"

#define RESET_PIN 04
#define MFIO_PIN 05
#define RAWDATA_BUFFLEN 250

max32664 MAX32664(RESET_PIN, MFIO_PIN, RAWDATA_BUFFLEN); 

// function declaration to load sys,dia,sp02 medically correct parameters values for calibration 
void loadAlgomodeParameters(){

  algomodeInitialiser algoParameters;
  

  algoParameters.calibValSys[0] = 120;
  algoParameters.calibValSys[1] = 122;
  algoParameters.calibValSys[2] = 125;

  algoParameters.calibValDia[0] = 80;
  algoParameters.calibValDia[1] = 81;
  algoParameters.calibValDia[2] = 82;

  algoParameters.spo2CalibCoefA = 1.5958422;
  algoParameters.spo2CalibCoefB = -34.659664;
  algoParameters.spo2CalibCoefC = 112.68987;

  MAX32664.loadAlgorithmParameters(&algoParameters);
}


// main program
void setup(){

  Serial.begin(57600);

  Wire.begin();

  loadAlgomodeParameters(); // function call to load parameter values

  int result = MAX32664.hubBegin();// initiating sensor hub to begin
  if (result == CMD_SUCCESS){
    Serial.println("Sensorhub begin!");
  }else{
    //stay here.
    while(1){
      Serial.println("Could not communicate with the sensor! please make proper connections");
      delay(5000);
    }
  }
  Serial.println("connection established successfully");
  bool ret = MAX32664.startBPTcalibration(); // function call for BPT calibration
  while(!ret){

    delay(10000);
    Serial.println("failed calib, please retsart");
    ret = MAX32664.startBPTcalibration();
  }

  delay(1000);

  //Serial.println("start in estimation mode");
  ret = MAX32664.configAlgoInEstimationMode();
  while(!ret){

    //Serial.println("failed est mode");
    ret = MAX32664.configAlgoInEstimationMode();
    delay(10000);
  }

  //MAX32664.enableInterruptPin();
  Serial.println("Getting the device ready..");
  delay(1000);
}

void loop(){

  uint8_t num_samples = MAX32664.readSamples();

  if(num_samples){
    bool x = true;
    bool y = (MAX32664.max32664Output.sys==0 && MAX32664.max32664Output.dia==0 && MAX32664.max32664Output.hr==0 && MAX32664.max32664Output.spo2==0);
    if(x != y){
    Serial.print("sys = ");
    Serial.print(MAX32664.max32664Output.sys);
    Serial.print(", dia = ");
    Serial.print(MAX32664.max32664Output.dia);
    Serial.print(", hr = ");
    Serial.print(MAX32664.max32664Output.hr);
    Serial.print(" spo2 = ");
    Serial.println(MAX32664.max32664Output.spo2);
             } 
    else {
      }
      
      
  }
delay(100);
}
