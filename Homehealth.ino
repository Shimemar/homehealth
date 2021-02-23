#define TS_ENABLE_SSL // For HTTPS SSL connection

#include <WiFiClientSecure.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "bsec.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
#define LED_BUILTIN 2

// Create an object of the class Bsec
Bsec iaqSensor;
SSD1306AsciiWire oled;
WiFiClientSecure  client;

String output;
//your wifi settings
char ssid[] = "xxxxxxxxxxxx";   // your network SSID (name) 
char pass[] = "xxxxxxxxxxxx";    // your network password

//your thingspeak settings
unsigned long myChannelNumber = 0000000;
const char * myWriteAPIKey = "XXXXXXXXXXXXXXXX";

int keyIndex = 0;            // your network key Index number (needed only for WEP)
// ThingSpeak Certificate Fingerprint, Expiration Date: August 3, 2022 at 8:00:00 AM EST 
#define SECRET_SHA1_FINGERPRINT "27 18 92 DD A4 26 C3 07 09 B9 7A E6 C5 21 B9 5B 48 F7 16 E1"
const char * fingerprint = SECRET_SHA1_FINGERPRINT;


// Entry point for the example
void setup(void)
{
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000L);

  iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);
  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(output);

  checkIaqSensorStatus();

  bsec_virtual_sensor_t sensorList[10] = {
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };

  iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();

  // Print the header
  output = "Timestamp [ms], raw temperature [°C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [°C], relative humidity [%], Static IAQ, CO2 equivalent, breath VOC equivalent";
  Serial.println(output);

  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(System5x7);
  oled.clear();

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak

}

int rCnt = 3;
int output_cnt = 1;

// Function that is looped forever
void loop(void)
{
  unsigned long time_trigger = millis();
  if (iaqSensor.run()) { // If new data is available
    output = String(time_trigger);
    output += " rTmp:" + String(iaqSensor.rawTemperature);
    output += " Pres:" + String((iaqSensor.pressure/100));
    output += " rHum:" + String(iaqSensor.rawHumidity);
    output += " GasR:" + String(iaqSensor.gasResistance);
    output += " iaq:" + String(iaqSensor.iaq);
    output += " iAcc:" + String(iaqSensor.iaqAccuracy);
    output += " temp:" + String(iaqSensor.temperature);
    output += " humi:" + String(iaqSensor.humidity);
    output += " siag:" + String(iaqSensor.staticIaq);
    output += " Eco2: " + String(iaqSensor.co2Equivalent);
    output += " Voc:" + String(iaqSensor.breathVocEquivalent);
    Serial.println(output);

    oled.clear();
    output = "Temp: " + String(iaqSensor.temperature);
    oled.println(output);
    output = "Humi: " + String(iaqSensor.humidity);
    oled.println(output);
    output = "Pres: " + String(iaqSensor.pressure /100 );
    oled.println(output);
    output = "sIaq: " + String(iaqSensor.staticIaq);
    oled.println(output);
    output = "eCo2: " + String(iaqSensor.co2Equivalent);
    oled.println(output);
    output = "bVoc: " + String(iaqSensor.breathVocEquivalent);
    oled.println(output);
    output = "iAcc: " + String(iaqSensor.iaqAccuracy);
    oled.println(output);

    output_cnt--;
    Serial.println("Set Count " + String(output_cnt));

  } else {
    checkIaqSensorStatus();
  }


  if( output_cnt == 0 ) {  
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect to SSID: ");
      Serial.println("parlmAir_N_EXT");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
        Serial.print(".");
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }
    // set the fields with the values
    ThingSpeak.setField(1, iaqSensor.temperature);
    ThingSpeak.setField(2, iaqSensor.humidity);
    ThingSpeak.setField(3, iaqSensor.pressure /100 );
    ThingSpeak.setField(4, iaqSensor.staticIaq);
    ThingSpeak.setField(5, iaqSensor.co2Equivalent);
    ThingSpeak.setField(6, iaqSensor.breathVocEquivalent);
  
    // write to the ThingSpeak channel
    while(rCnt) {
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      if(x == 200){
        Serial.println("Channel update successful.");
        break;
      }
      else{
        Serial.println("Problem updating channel. HTTP error code " + String(x));
        delay(3000); // Wait 20 seconds to update the channel again
        rCnt--;
      }
    }
    rCnt = 3;
    output_cnt = 30;
  }
}


// Helper function definitions
void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK) {
    if (iaqSensor.status < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }

  if (iaqSensor.bme680Status != BME680_OK) {
    if (iaqSensor.bme680Status < BME680_OK) {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}

void errLeds(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}
