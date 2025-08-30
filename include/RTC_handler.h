#pragma once
#include <Arduino.h>

void rtc_init();
String rtc_get_date_string(); // YYYYMMDD
bool rtc_is_time(int hour, int minute); // true at that minute
String rtc_get_timestamp(); // ISO
