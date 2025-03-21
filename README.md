# Home health

Log the environment in my house.

I used these technologies.
* ESP32
* BME680
* OLED (SSD1306)
* Arduino
* BSEC library

BME680 is used as the sensor, and temperature, humidity, atmospheric pressure, and BSEC are used to measure eCO2 and bVOC.
The MCU uses ESP32 for WIFI communication, data collection, and calculation.

The device has an OLED display so you can see the same information.
At the same time, the data is posted to ThingSpaek and can be checked on the cloud.

# Preparation
Prepare the environment for the arduino IDE.

* ESP32
* BSEC
* ThingSpeak

At my place, I am building BSEC with version 18.1.0 of Arduino IDE because the latest Arduino IDE has an error.


# Prototyping
![IMG_5028](https://user-images.githubusercontent.com/75776044/108789013-a8ba1180-75bc-11eb-87a6-869029f46388.jpg)
<br>
<br>

# ThingSpeak
![things](https://user-images.githubusercontent.com/75776044/108789294-4ca3bd00-75bd-11eb-8039-bd23f8c9968e.jpg)
<br>
<br>
<br>
<br>
# You change Your environment

Change the following items in the source code to suit your environment.

```C
//your wifi settings
char ssid[] = "xxxxxxxxxxxx";   // your network SSID (name)
char pass[] = "xxxxxxxxxxxx";    // your network password

//your thingspeak settings
unsigned long myChannelNumber = 0000000;
const char * myWriteAPIKey = "XXXXXXXXXXXXXXXX";
```
