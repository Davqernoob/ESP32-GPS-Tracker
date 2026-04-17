This is a small project, even more of a joke, but all the more so, there will be a detailed guide about it here.
To get started, you will need at least 3 things:
1. ESP32
2. GY-NEO6MV2 NEO-6M (GPS module)
3. Ware and electrical to power

      Connection
To connect this module to the ESP32 controller, use this pinout:
1. VCC - 5V
2. TX - GPIO 16
3. RX - GPIO 17
4. GND - GND

   After connecting and loading the code, take the assembled device outside with access to the sky. Indoors, the module will receive poor data or no data at all. When it starts displaying information, you'll notice its LED light up and start blinking blue. After that, connect to the Wi-Fi network (ESP32_GPS_Tracker), and then enter this link in the browser on this device: http://192.168.4.1

You will then see your coordinates, or more precisely:
1. Your longitude
2. Your latitude
3. Your altitude
4. Number of satellites
5. UTC time
