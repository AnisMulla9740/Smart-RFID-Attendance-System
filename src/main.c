#include <Arduino.h>
#include "config.h"
#include "lcd_display.h"
#include "sd_storage.h"
#include "server_sync.h"
#include "rfid_handler.h"
#include "rtc_handler.h"
#include "keypad_input.h"
#include <Preferences.h>

Preferences prefs;

void setup() {
  Serial.begin(115200);
  delay(200);

  // init LCD
  lcd_init();

  // SD
  if(!sd_init(SD_CS_PIN)) {
    lcd_print_line(0, "SD Init Failed");
    Serial.println("SD init failed");
    // continue but many features require SD
  }

  // RTC
  rtc_init();

  // WiFi try (server sync will connect as needed)
  lcd_print_line(0, "Power ON");
  delay(500);

  // college selection (persisted per day)
  prefs.begin(PREFS_NAMESPACE, false);
  int selected = prefs.getInt(PREF_SELECTED_COLLEGE, -1);

  // if not set or different date, ask
  if (selected < 0) {
    lcd_print_line(0, "Select College ID");
    int id = keypad_get_college_id(); // blocks until input
    // validate against colleges.json on SD
    if (!sd_validate_college(id)) {
      lcd_print_line(0, "Invalid College ID");
      delay(1500);
      // for simplicity: try again once
      id = keypad_get_college_id();
      if (!sd_validate_college(id)) {
        lcd_print_line(0, "Invalid - Reboot");
        delay(2000);
        ESP.restart();
      }
    }
    prefs.putInt(PREF_SELECTED_COLLEGE, id);
    selected = id;
    lcd_print_line(0, "College Selected:");
    lcd_print_line(1, String(selected).c_str());
    delay(1000);
  } else {
    lcd_print_line(0, "College ID:");
    lcd_print_line(1, String(selected).c_str());
    delay(1000);
  }

  // daily sync of student data (once per day)
  lcd_print_line(0, "Syncing students");
  if (wifi_connect()) {
    bool ok = server_fetch_and_save_students(selected);
    if (ok) {
      lcd_print_line(0, "Sync OK");
    } else {
      lcd_print_line(0, "Sync Failed");
    }
  } else {
    lcd_print_line(0, "WiFi Fail");
  }
  delay(500);

  // ready screen
  lcd_print_line(0, "READY: Tap Card");
  rfid_init(SS_PIN, RST_PIN); // start reader
}

void loop() {
  // check time for end of day upload
  if (rtc_is_time(17, 0)) { // when hh:mm matches 17:00 it will trigger repeatedly; handler ensures single upload per day
    server_upload_attendance_day();
  }

  // check keypad for manual add
  if (keypad_add_pressed()) {
    lcd_print_line(0, "Add New Student");
    Student s = keypad_get_new_student(); // collect roll,name,rfid via keypad
    // send to server & locally append
    if (wifi_connect()) {
      if (server_add_student(s)) {
        sd_append_student_delta(s);
        lcd_print_line(0, "Added OK");
      } else lcd_print_line(0, "Add Failed");
    } else {
      sd_append_student_delta(s);
      lcd_print_line(0, "Saved Locally");
    }
    delay(1000);
    lcd_print_line(0, "READY: Tap Card");
  }

  // RFID scanning
  if ( rfid_card_present() ) {
    String uid = rfid_read_uid();
    lcd_print_line(0, "Card UID:");
    lcd_print_line(1, uid.c_str());
    // search local student shards
    Student s;
    if (sd_find_student_by_rfid(prefs.getInt(PREF_SELECTED_COLLEGE), uid.c_str(), s)) {
      // found
      if (sd_attendance_already_marked(prefs.getInt(PREF_SELECTED_COLLEGE), rtc_get_date_string().c_str(), s.rfid)) {
        lcd_print_line(0, "Already Marked");
      } else {
        sd_mark_attendance(prefs.getInt(PREF_SELECTED_COLLEGE), rtc_get_date_string().c_str(), s);
        lcd_print_line(0, "Marked:");
        lcd_print_line(1, s.roll_no);
      }
    } else {
      lcd_print_line(0, "Not Registered");
      lcd_print_line(1, "Press Add?");
      // optionally handle quick add via keypad (left for keypad_input)
    }
    delay(1200);
    lcd_print_line(0, "READY: Tap Card");
  }

  delay(150);
}
