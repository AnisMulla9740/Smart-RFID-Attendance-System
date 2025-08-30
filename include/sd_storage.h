#pragma once
#include <Arduino.h>

struct Student {
  char roll_no[32];
  char name[64];
  char rfid[32];
};

bool sd_init(int csPin);
bool sd_validate_college(int collegeID);
bool sd_find_student_by_rfid(int collegeID, const char* uid, Student &out);
bool sd_attendance_already_marked(int collegeID, const char* dateStr, const char* uid);
bool sd_mark_attendance(int collegeID, const char* dateStr, const Student &s);
bool sd_append_student_delta(const Student &s);
