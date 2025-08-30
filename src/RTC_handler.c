#include "rtc_handler.h"
#include <Wire.h>
#include <RTClib.h>

static RTC_DS3231 rtc;
static int last_upload_day = -1;

void rtc_init() {
  Wire.begin();
  rtc.begin();
  // If RTC lost power you may set it here from compile time:
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

String rtc_get_date_string(){
  DateTime now = rtc.now();
  char buf[11];
  sprintf(buf, "%04d%02d%02d", now.year(), now.month(), now.day());
  return String(buf);
}

String rtc_get_timestamp(){
  DateTime now = rtc.now();
  char buf[25];
  sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  return String(buf);
}

bool rtc_is_time(int hour, int minute) {
  DateTime now = rtc.now();
  // Ensure upload happens once per day; guard with static
  static int lastDay = -1;
  if (now.hour() == hour && now.minute() == minute) {
    if (lastDay != now.day()) {
      lastDay = now.day();
      return true;
    }
  }
  return false;
}
