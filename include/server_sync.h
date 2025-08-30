#pragma once
#include <Arduino.h>
#include "sd_storage.h"

bool wifi_connect();
bool server_fetch_and_save_students(int collegeID);
bool server_add_student(const Student &s);
bool server_upload_attendance_day();
