// Timezone and DST Settings
// Timezone and DST Settings
// Timezone and DST Settings
// Timezone and DST 
// Select your timezone by setting the corresponding number (1-10):
// 1 = UTC
// 2 = EST (Eastern Standard Time, UTC-5)
// 3 = CST (Central Standard Time, UTC-6)
// 4 = MST (Mountain Standard Time, UTC-7)
// 5 = PST (Pacific Standard Time, UTC-8)
// 6 = CET (Central European Time, UTC+1)
// 7 = MSK (Moscow Standard Time, UTC+3)
// 8 = CST (China Standard Time, UTC+8)
// 9 = JST (Japan Standard Time, UTC+9)
// 10 = AEST (Australian Eastern Standard Time, UTC+10)

#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include "secrets.h"  // Include the secrets file

// Timezone and DST Settings
int timezoneSelection = 3;  // Set this number to your timezone (1-10)
bool useDST = true;  // Set to true if DST should be used, false otherwise

// List of Common Timezones (in seconds offset from UTC)
int timezoneOffsets[] = {
  0,      // UTC
  -18000, // EST (UTC-5)
  -21600, // CST (UTC-6)
  -25200, // MST (UTC-7)
  -28800, // PST (UTC-8)
  3600,   // CET (UTC+1)
  10800,  // MSK (UTC+3)
  28800,  // CST (UTC+8)
  32400,  // JST (UTC+9)
  36000   // AEST (UTC+10)
};

const char* timezoneNames[] = {
  "UTC",
  "EST (UTC-5)",
  "CST (UTC-6)",
  "MST (UTC-7)",
  "PST (UTC-8)",
  "CET (UTC+1)",
  "MSK (UTC+3)",
  "CST (China, UTC+8)",
  "JST (UTC+9)",
  "AEST (UTC+10)"
};

// NTP Client Settings
WiFiUDP ntpUDP;
int timeOffset = timezoneOffsets[timezoneSelection - 1];  // Get initial timezone offset
NTPClient timeClient(ntpUDP, "pool.ntp.org", timeOffset, 60000); // Sync time every 60 seconds

RTC_DS3231 rtc;
Adafruit_7segment matrix = Adafruit_7segment();

const int ledPin = LED_BUILTIN;  // Onboard LED pin

// Time tracking
DateTime lastSyncTime;
const unsigned long syncInterval = 3600; // 1 hour in seconds
DateTime lastRetryTime;
const unsigned long retryInterval = 900; // 15 minutes in seconds
bool firstLoop = true;  // Flag to track the first loop iteration
bool errorOccurred = false;  // Flag to track if an error occurred
bool wasConnected = false;  // Track the previous Wi-Fi state

// Wi-Fi connection retry settings
const int maxRetries = 15;
unsigned long lastWiFiCheck = 0; // To track the last Wi-Fi check time
const unsigned long wifiCheckInterval = 60000; // Check Wi-Fi every minute when disconnected

void setup() {
  Serial.begin(115200);  // Start the serial communication

  pinMode(ledPin, OUTPUT);  // Initialize the onboard LED pin
  digitalWrite(ledPin, HIGH);  // Ensure the LED is off initially

  // Display selected timezone and DST setting
  Serial.println("Selected Timezone: " + String(timezoneNames[timezoneSelection - 1]));
  Serial.println("Daylight Savings Time: " + String(useDST ? "Enabled" : "Disabled"));

  // Adjust for DST if needed
  if (useDST) {
    timeOffset += 3600;  // Add 1 hour for DST
    timeClient.setTimeOffset(timeOffset);  // Update the NTP client with the new offset
  }

  // Initialize the LED display
  Serial.println("Initializing LED display...");
  matrix.begin(0x70);

  // Attempt to connect to Wi-Fi
  connectToWiFi();

  // Initialize NTP Client
  timeClient.begin();
  
  // Initialize RTC
  Serial.println("Checking RTC connection...");
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Set time if RTC is not running or on the first boot
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time from NTP.");
    syncTimeWithNTP();
  } else if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Syncing RTC with NTP on first boot...");
    syncTimeWithNTP();
    lastSyncTime = rtc.now();  // Set the initial sync time
  } else {
    Serial.println("RTC is running with saved time.");
    lastSyncTime = rtc.now();  // Set the initial sync time
    lastRetryTime = rtc.now();  // Set the initial retry time
  }
}

void loop() {
  if (firstLoop) {
    Serial.println("Main loop running...");
    firstLoop = false;  // Set the flag to false after the first loop iteration
  }

  DateTime now = rtc.now();

  // Update RTC from NTP every hour
  if ((now - lastSyncTime).totalseconds() >= syncInterval) {
    syncTimeWithNTP();
    lastSyncTime = rtc.now();  // Reset the last sync time
  }

  // Check Wi-Fi connection status
  if (WiFi.status() != WL_CONNECTED) {
    if (!errorOccurred) {
      // Print Wi-Fi loss information only once when disconnected
      Serial.println("Wi-Fi disconnected at " + now.timestamp());
      errorOccurred = true;
    }
    if (millis() - lastWiFiCheck >= wifiCheckInterval) {
      lastWiFiCheck = millis();  // Update the last check time
      connectToWiFi();  // Attempt to reconnect to Wi-Fi
    }
  } else {
    if (errorOccurred || !wasConnected) {
      // Print reconnection information when Wi-Fi reconnects
      Serial.println("Wi-Fi connected at " + now.timestamp());
      Serial.println("Connected to SSID: " + String(WiFi.SSID()));
      Serial.println("Signal strength (RSSI): " + String(WiFi.RSSI()) + " dBm");
      Serial.println("Local IP address: " + WiFi.localIP().toString());
      errorOccurred = false;
    }
  }
  
  wasConnected = (WiFi.status() == WL_CONNECTED);  // Update the Wi-Fi connection status

  // Display time on LED matrix
  displayTime(now.hour(), now.minute(), now.second());

  // Blink the LED if an error occurred (i.e., Wi-Fi is disconnected)
  if (errorOccurred) {
    digitalWrite(ledPin, !digitalRead(ledPin));  // Toggle the LED state
  } else {
    digitalWrite(ledPin, HIGH);  // Turn off the LED if Wi-Fi is connected
  }

  delay(1000);  // Update every second
}

// Function to connect to Wi-Fi with retries
void connectToWiFi() {
  int retries = 0;
  Serial.println("Attempting to connect to WiFi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED && retries < maxRetries) {
    delay(1000);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    errorOccurred = false;  // Clear the error flag
    digitalWrite(ledPin, HIGH);  // Turn off the LED immediately after connecting
  } else {
    Serial.println("\nFailed to connect to WiFi. Running from RTC.");
    errorOccurred = true;  // Set the error flag
  }
}

// Function to sync RTC with NTP time
void syncTimeWithNTP() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Syncing time with NTP...");
    timeClient.update();
    
    unsigned long epochTime = timeClient.getEpochTime();
    if (epochTime > 0) {
      // Convert epoch to DateTime
      DateTime ntpTime = DateTime(epochTime);
      
      // Set RTC with NTP time
      rtc.adjust(ntpTime);
      Serial.println("RTC time successfully updated with NTP time.");
      errorOccurred = false;  // Clear the error flag
    } else {
      Serial.println("Failed to retrieve time from NTP server.");
      errorOccurred = true;  // Set the error flag
    }
  } else {
    Serial.println("Cannot sync with NTP. No Wi-Fi connection. Using RTC time.");
    errorOccurred = true;  // Set the error flag
  }
}

void displayTime(int hour, int minute, int second) {
  // Display hour
  matrix.writeDigitNum(0, hour / 10);  // Tens digit of the hour
  matrix.writeDigitNum(1, hour % 10);  // Ones digit of the hour

  // Blink colon based on seconds
  matrix.drawColon(second % 2 == 0);  // Colon on for even seconds, off for odd seconds

  // Display minute
  matrix.writeDigitNum(3, minute / 10);  // Tens digit of the minute
  matrix.writeDigitNum(4, minute % 10);  // Ones digit of the minute

  matrix.writeDisplay();  // Update the display
}
