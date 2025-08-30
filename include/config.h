#pragma once

// ====== WiFi & Server ======
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
const char* SERVER_BASE_URL = "http://your.server.api"; // e.g. http://192.168.1.100:5000

// ====== SD ======
const int SD_CS_PIN = 5; // adjust for your module

// ====== RFID (MFRC522) ======
const int RST_PIN = 22;
const int SS_PIN  = 21;

// ====== RTC ======
 // Using I2C (DS3231) on default SDA/SCL

// ====== LCD ======
const int LCD_ADDR = 0x27; // common
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// ====== Keypad ======
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 3;

// ====== Attendance behavior ======
const char* ATTENDANCE_LOG_FOLDER = "/attendance_logs";
const char* ATTENDANCE_DATA_FOLDER = "/attendance_data";
const int STUDENT_SHARD_SIZE = 500; // informational

// ====== Misc ======
const char* PREFS_NAMESPACE = "smartrfid";
const char* PREF_SELECTED_COLLEGE = "sel_col";
