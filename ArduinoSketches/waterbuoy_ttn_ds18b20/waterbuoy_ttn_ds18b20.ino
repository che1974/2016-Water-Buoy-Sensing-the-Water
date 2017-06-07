//Include libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TheThingsNetwork.h>
#include <TheThingsMessage.h>

// Data wire is plugged into pin 0 on the Arduino
#define ONE_WIRE_BUS 0
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Set your AppEUI and AppKey
const char *appEui = "70B3D57EF00049BE"; // from dashboard ttn
const char *appKey = "F32B27E3D58E451E656F620D3AB315E5";

#define loraSerial Serial1
#define debugSerial SerialUSB

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

devicedata_t data = api_DeviceData_init_default;

void setup() {
  loraSerial.begin(57600);
  debugSerial.begin(9600);
  sensors.begin();

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);

  // Select what fields to include in the encoded message
  data.has_motion = false;
  data.has_water = false;
  data.has_temperature_celcius = true;
  data.has_temperature_fahrenheit = false;
  data.has_humidity = false; 
}

void loop() {
  debugSerial.println("-- LOOP");

  //Serial.print("Hello world!");
  
  // Read the sensors
  sensors.requestTemperatures(); 
  debugSerial.print("Current Water Temperature is: ");
  debugSerial.println(sensors.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  data.temperature_celcius = (int)sensors.getTempCByIndex(0);
  
  // Encode the selected fields of the struct as bytes
  byte *buffer;
  size_t size;
  TheThingsMessage::encodeDeviceData(&data, &buffer, &size);

  // Send it off
  ttn.sendBytes(buffer, size);

  delay(10000);
}

