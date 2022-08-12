#include <HTSensor.h>

#include "ghaemShopSmSim.h"
#define   SERIAL_BAUD_RATE   19200
#define SENSOR_PIN 5

ghaemShopSmSim SmSim(10, 11, SERIAL_BAUD_RATE);  //RX, TX, SerialBaudRate
HTSensor s;

///// Sample Test Relays /////
const int TestRelayPinsCount = 3;
struct RelayPinsStruct TestRelays[TestRelayPinsCount] = {
  {"light", 1, false, HIGH},
  {NULL,    2, false, NULL},
  {"valve", 3, true,  LOW}
};
  
const int VariablesCount = 6;
struct Element TestSensors[VariablesCount] = {
  {"Gas", ET_BYTE, NULL},
  {"Temperture", ET_FLOAT, NULL},
  {"Humidity", ET_INT, NULL},
  {"Pressure", ET_DOUBLE, NULL},
  {"Sensor1", ET_CHAR, NULL},
  {"Sensor2", ET_STRING, NULL}
};
  
void setup() {

  Serial.begin(SERIAL_BAUD_RATE); // Open serial communications and wait for port to open
  while (!Serial);  // wait for serial port to connect. Needed for native USB port only

  Serial.println("Salam ghaemShop.ir!");
  Serial.println("Prg: [ghaemShopSmSim Example]");
  Serial.println("Start...");
  
  ////////////////////////////////// Start Paogram ////////////////////////////////////////
  s.SetDataPin(SENSOR_PIN);   // Temperture/Humidity Sensor object
  
  SmSim.Begin();
  SmSim.SetNewPassword("pass");
  Serial.println("Pass:[" + String(SmSim.GetPassword()) + "]");
  SmSim.DeleteAllSmsInSim();
  SmSim.SetSmsFormatAsText();
  SmSim.ReplyRelayNameAsNumber = false;
  SmSim.ConfigRelay(TestRelays, TestRelayPinsCount);
  strcpy(TestSensors[5].s, "ghaemShop");    //sample of fill String variable
  SmSim.ConfigVariables(TestSensors, VariablesCount);

  for (int i = 0; i < VariablesCount; i++) {
    Serial.println("TestSensors[" + String(i) + "].Name = [" + TestSensors[i].Name + "]");
  }
}

//unsigned long t = millis();
unsigned long sd = millis();  // sensor delay
void loop() {
  
  SmSim.GSMRecieveEvent();    //main of ghaemShopSmSim Lib

  // Read Temperture/Hiumidity Sernsor
  if (millis() > sd + 10000) {      //10 seconds delay
    sd = millis();  // sensor delay
    
    int r = s.ReadData();
    if (r == SENSOR_RESULT_OK) {
      
      float T = s.getTemperture();
      int H = s.getHumidity();

      Serial.println("T = " + String(T));
      Serial.println("H = " + String(H));
      
      TestSensors[1].f = T;   //set Temperture Variable; read from sensor {"Temperture", ET_FLOAT, NULL}
      TestSensors[2].i = H;   //set Humidity Variable; read from sensor {"Humidity", ET_INT, NULL}
    }
  }

  /*
  // Test variables value
  if (millis() > t + 30000) {
    t = millis();
    for (int i = 0; i < VariablesCount; i++) {
      String Value = "";
      switch (TestSensors[i].Type) {
        case ET_BYTE:   Value = String(TestSensors[i].b); break;
        case ET_INT:    Value = String(TestSensors[i].i); break;
        case ET_FLOAT:  Value = String(TestSensors[i].f, FLOAT_DECIMAL_POINT); break;
        case ET_DOUBLE: Value = String(TestSensors[i].d, DOUBLE_DECIMAL_POINT); break;
        case ET_CHAR:   Value = String(TestSensors[i].c); break;
        case ET_STRING: Value = String(TestSensors[i].s); break;
      }
      Serial.println("Loop.TestSensors[" + String(i) + "].Name = [" + TestSensors[i].Name + "] Value = [" + Value + "]");
    }
  }
  */
}

///// END ghaemShop.ir ghaemShopSmSim Lib Example /////
///////////////////////////////////////////////////////

