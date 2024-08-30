# ESP8266 NTP Clock with DS3231 RTC and 7-Segment LED Display

This project implements a highly accurate and reliable digital clock using an ESP8266 microcontroller, synchronized with an NTP (Network Time Protocol) server and backed up by a DS3231 RTC (Real-Time Clock) module. The time is displayed on a 4-digit 7-segment LED display. The clock supports multiple time zones and daylight saving time (DST) adjustments.

Features
Accurate Timekeeping: Synchronizes with NTP servers to maintain precise time.
RTC Backup: Utilizes DS3231 RTC module to keep time during network outages or power failures.
Time Zone Support: Supports multiple predefined time zones with easy configuration.
Daylight Saving Time (DST): Optional DST adjustment with simple toggle.
Automatic Syncing: Syncs with NTP server on the first boot and periodically every hour.
Wi-Fi Reconnection: Attempts to reconnect to Wi-Fi every 15 minutes if disconnected.
Visual Time Display: Displays current time in HH
format on a 4-digit 7-segment LED display.
Blinking Colon: Colon blinks every second to indicate running time.
Detailed Serial Output: Provides comprehensive logs for debugging and monitoring via serial console.
Hardware Requirements
ESP8266 Development Board (e.g., NodeMCU or Wemos D1 Mini)
DS3231 RTC Module
Adafruit 4-Digit 7-Segment LED Display Backpack
Breadboard and Jumper Wires
Micro USB Cable (for programming and power supply)
Power Supply (optional if not using USB power)
Optional:

Enclosure or Case (for a finished look)
Push Buttons or Switches (for future enhancements like manual time adjustment)
Software Requirements
Arduino IDE (Version 1.8 or higher)
ESP8266 Board Package installed in Arduino IDE
Required Arduino Libraries:
ESP8266WiFi.h
NTPClient.h
WiFiUdp.h
Wire.h
RTClib.h
Adafruit_GFX.h
Adafruit_LEDBackpack.h
Circuit Diagram

Note: Replace the above URL with the actual path to your circuit diagram image.

Pin Connections
Component	ESP8266 Pin	DS3231 RTC Pin	7-Segment Display Pin
SDA	D2 (GPIO4)	SDA	SDA
SCL	D1 (GPIO5)	SCL	SCL
VCC	3.3V	VCC	VCC
GND	GND	GND	GND
Both the DS3231 RTC and the 7-Segment LED display use the I2C protocol, allowing them to share the same SDA and SCL lines.

Setup Instructions
Follow these steps to set up and run the project successfully.

1. Clone the Repository
bash
Copy code
git clone https://github.com/yourusername/esp8266-ntp-clock.git
Alternatively, download the ZIP file and extract it to your preferred directory.

2. Install Arduino IDE
Download and install the Arduino IDE from the official website.

3. Install Required Libraries
Open Arduino IDE and install the following libraries via Library Manager:

ESP8266 Board Package

Go to File > Preferences.
In Additional Boards Manager URLs, add:
bash
Copy code
http://arduino.esp8266.com/stable/package_esp8266com_index.json
Click OK.
Go to Tools > Board > Boards Manager.
Search for ESP8266 and install the latest version.
Libraries

Go to Sketch > Include Library > Manage Libraries.
Install the following libraries:
ESP8266WiFi
NTPClient
RTClib by Adafruit
Adafruit GFX Library
Adafruit LED Backpack Library
Wire
Ensure all libraries are up to date to prevent compatibility issues.

4. Configure secrets.h File
Create a new file named secrets.h in the project directory with the following content:

cpp
Copy code
#ifndef SECRETS_H
#define SECRETS_H

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

#endif
Replace YOUR_WIFI_SSID and YOUR_WIFI_PASSWORD with your actual Wi-Fi credentials.

This file is used to keep sensitive information separate from your main codebase.

5. Configure Timezone and DST Settings
In the main .ino file, locate the following section and configure accordingly:

cpp
Copy code
// Timezone and DST Settings
int timezoneSelection = 1;  // Set this number to your timezone (1-10)
bool useDST = true;         // Set to true if DST should be used, false otherwise
Available Timezones:

Number	Timezone Name	Offset (from UTC)
1	UTC	+00:00
2	EST (Eastern Standard)	-05:00
3	CST (Central Standard)	-06:00
4	MST (Mountain Standard)	-07:00
5	PST (Pacific Standard)	-08:00
6	CET (Central European)	+01:00
7	MSK (Moscow Standard)	+03:00
8	CST (China Standard)	+08:00
9	JST (Japan Standard)	+09:00
10	AEST (Australian Eastern)	+10:00
Example:

For Eastern Standard Time with DST enabled:
cpp
Copy code
int timezoneSelection = 2;
bool useDST = true;
Adjust these settings according to your geographical location and DST observance.

6. Upload the Code
Connect your ESP8266 board to your computer via USB.
In Arduino IDE, go to Tools and set:
Board: Select your specific ESP8266 board model.
Port: Select the correct COM port for your board.
Click the Upload button (right-pointing arrow) to compile and upload the code to your ESP8266.
Open the Serial Monitor (Tools > Serial Monitor) and set the baud rate to 115200 to view debug logs.
If the upload is successful, you should see initialization logs and time displayed on your 7-segment display.

Usage
Once the setup is complete and the code is uploaded:

Power On: Connect power to your ESP8266 board. It will automatically attempt to connect to the configured Wi-Fi network.
Initial NTP Sync: On first boot, the system will sync time with the NTP server if Wi-Fi is connected.
Time Display: The current time will be displayed on the 7-segment LED display in HH
format.
The colon (:) blinks every second, indicating that the clock is running.
Periodic Syncing:
NTP Sync: The clock syncs with the NTP server every hour to maintain accuracy.
Wi-Fi Reconnection: If Wi-Fi disconnects, the system attempts to reconnect every 15 minutes.
RTC Backup: In case of Wi-Fi unavailability, the DS3231 RTC module maintains accurate timekeeping.
Serial Monitor Logs: Detailed logs are available through the serial monitor for monitoring and debugging purposes, including:

Wi-Fi connection status
NTP synchronization status
RTC operation status
Current time updates
Troubleshooting
Problem: Cannot connect to Wi-Fi.

Solution:
Check if the Wi-Fi SSID and password in secrets.h are correct.
Ensure that the Wi-Fi network is operational and within range.
Restart the ESP8266 and monitor serial logs for connection attempts.
Problem: Time is incorrect or not updating.

Solution:
Verify that the correct timezoneSelection and useDST settings are configured.
Ensure that the ESP8266 has a stable internet connection for NTP syncing.
Check if the RTC module is properly connected and functioning.
Problem: 7-Segment display is not showing anything.

Solution:
Confirm that all connections between the ESP8266 and the display are secure and correct.
Ensure that the display is receiving adequate power.
Verify that the correct I2C address (0x70) is used in the code.
Problem: Compilation errors in Arduino IDE.

Solution:
Ensure all required libraries are installed and up to date.
Verify that the ESP8266 board package is correctly installed.
Restart Arduino IDE and try compiling again.
Problem: Serial Monitor shows garbled text.

Solution:
Set the correct baud rate (115200) in the serial monitor settings.
Check for loose or faulty USB connections.
Customization Options
1. Adding More Timezones:

You can easily add more timezones by updating the timezoneOffsets and timezoneNames arrays.
Example:
cpp
Copy code
// Add IST (Indian Standard Time)
timezoneOffsets[] = { ... , 19800 }; // UTC+5:30
timezoneNames[] = { ... , "IST (UTC+5:30)" };
2. Adjusting Sync Intervals:

Modify syncInterval and retryInterval to change how often NTP syncing and Wi-Fi reconnection attempts occur.
cpp
Copy code
const unsigned long syncInterval = 3600; // Sync every hour
const unsigned long retryInterval = 900; // Retry Wi-Fi every 15 minutes
3. Changing NTP Server:

Update the NTP server address in the NTPClient initialization.
cpp
Copy code
NTPClient timeClient(ntpUDP, "time.nist.gov", timeOffset, 60000);
4. Display Formats:

Modify the displayTime function to change how time is displayed (e.g., 24-hour vs. 12-hour format).
cpp
Copy code
int displayHour = hour % 12 == 0 ? 12 : hour % 12; // 12-hour format
5. Adding Date Display:

Expand the functionality to display the current date by integrating additional display components or cycling information on the existing display.
6. Incorporating Buttons:

Add buttons for manual time adjustment, switching between timezones, or toggling DST on the fly.
Future Enhancements
Web Interface: Implement a web server on the ESP8266 to allow remote configuration of settings like Wi-Fi credentials, timezone, and DST.
Alarm Functionality: Add features to set alarms or timers with audible or visual indicators.
Temperature Display: Utilize the built-in temperature sensor of the DS3231 RTC to display ambient temperature.
Automatic DST Adjustment: Programmatically adjust for DST based on the selected timezone and current date.
Battery Backup Monitoring: Include status checks and alerts for the RTC module's backup battery health.
Enclosure Design: Create a 3D-printable case design for a polished and portable clock unit.
License
This project is licensed under the MIT License - see the LICENSE file for details.

Acknowledgments
Adafruit Industries for their excellent libraries and hardware components.
Arduino Community for continuous support and contributions to open-source projects.
ESP8266 Community for providing extensive resources and documentation.
NTP.org for maintaining reliable time servers accessible worldwide.
RTClib developers for simplifying RTC integration in Arduino projects.
Feel free to contribute to this project by submitting issues, pull requests, or suggestions to improve functionality and usability.

Happy Coding!
