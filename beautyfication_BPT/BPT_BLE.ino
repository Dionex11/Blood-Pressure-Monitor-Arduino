#include <Wire.h>
#include "max32664.h"
#include <ArduinoBLE.h>

BLEService customService("791fcfcb-495d-4095-924a-55a22724eb44");
BLEStringCharacteristic sysval("6dd5152b-de87-4823-aaa4-c43c58d5c945", BLERead | BLEWrite,13);
BLEStringCharacteristic diaval("eb988f82-a7c8-11ed-afa1-0242ac120002", BLERead | BLEWrite,13);
BLEStringCharacteristic calsysval("56268214-4c39-4d5e-a74d-186b335c96b5", BLERead | BLEWrite | BLENotify,13); 
BLEStringCharacteristic caldiaval("2e8bd505-da2e-4285-8979-5c85fc2c2520", BLERead | BLEWrite | BLENotify ,13);
BLEDevice central;

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



void BLE_setup(){
  
   while (!Serial);
  if (!BLE.begin()) 
  {
    Serial.println("starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("Blood Pressure Monitor");
  
  BLE.setAdvertisedService(customService);
  customService.addCharacteristic(sysval);
  customService.addCharacteristic(diaval);
  customService.addCharacteristic(calsysval);
  customService.addCharacteristic(caldiaval);
  BLE.addService(customService);
  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
     
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

    delay(1000);
    Serial.println("failed calib, please retsart");
    ret = MAX32664.startBPTcalibration();
  }

  delay(1000);

  //Serial.println("start in estimation mode");
  ret = MAX32664.configAlgoInEstimationMode();
  while(!ret){

    //Serial.println("failed est mode");
    ret = MAX32664.configAlgoInEstimationMode();
    delay(1000);
  }

  //MAX32664.enableInterruptPin();
  Serial.println("Getting the device ready..");
  BLE_setup();
  delay(1000);
}

void loop(){
  byte sysvalue;
  byte diavalue;
  BLEDevice central=BLE.central();
  Serial.println("DEVICE DISCONNECTED, PLEASE RECONNECT !!!");
  while(central.connected()){
  String data="";
  sysval.readValue(sysvalue);
  Serial.println("Entered systollic value;");
  Serial.print(sysvalue);
  Serial.println("Entered diastollic value:");
  diaval.readValue(diavalue);
  Serial.print(diavalue);
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
     data=String(MAX32664.max32664Output.sys)+","+String(MAX32664.max32664Output.dia)+","+String(MAX32664.max32664Output.hr)+","+String(MAX32664.max32664Output.spo2);
     calsysval.writeValue(String(data));
             } 
    else {
      }
      
      
  }}
delay(100);
}
