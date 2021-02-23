# Home health

Log the environment in your house.

* ESP32
* BME680
* OLED
* Arduino


BME680 is used as the sensor, and temperature, humidity, atmospheric pressure, and BSEC are used to measure eCO2 and bVOC.
The MCU uses ESP32 for WIFI communication, data collection, and calculation.

The device has an OLED display so you can see the same information.
At the same time, the data is posted to ThingSpaek and can be checked on the cloud.

![IMG_5028](https://user-images.githubusercontent.com/75776044/108789013-a8ba1180-75bc-11eb-87a6-869029f46388.jpg)
