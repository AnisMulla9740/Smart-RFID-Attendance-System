#include "sd_storage.h"
#include "config.h"
#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>

bool sd_init(int csPin) {
  return SD.begin(csPin);
}

bool sd_validate_college(int collegeID) {
  File f = SD.open("/colleges.json");
  if(!f) return false;
  DynamicJsonDocument doc(4096);
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err) return false;
  for (JsonVariant v : doc.as<JsonArray>()) {
    if (v["id"].as<int>() == collegeID) return true;
  }
  return false;
}

// helper: get folder path for students for today (date string)
static String student_folder(int collegeID) {
  String date = ""; // get date via rtc
  // We cannot include rtc dependency here to keep modular; assume RTC file exists
  extern String rtc_get_date_string();
  date = rtc_get_date_string();
  char buf[128];
  sprintf(buf, "%s/%d/%s", ATTENDANCE_DATA_FOLDER, collegeID, date.c_str());
  return String(buf);
}

bool sd_find_student_by_rfid(int collegeID, const char* uid, Student &out) {
  String folder = student_folder(collegeID);
  File dir = SD.open(folder);
  if (!dir) return false;
  File file = dir.openNextFile();
  while (file) {
    if (!file.isDirectory()) {
      // parse file
      DynamicJsonDocument doc(200000); // big because file can be large; adjust per device RAM
      DeserializationError err = deserializeJson(doc, file);
      file.close();
      if (err) {
        // continue next
        file = dir.openNextFile();
        continue;
      }
      for (JsonObject stu : doc.as<JsonArray>()) {
        const char* rfid = stu["rfid"];
        if (rfid && strcmp(rfid, uid) == 0) {
          // fill out
          strncpy(out.roll_no, stu["roll_no"] | "", sizeof(out.roll_no));
          strncpy(out.name, stu["name"] | "", sizeof(out.name));
          strncpy(out.rfid, rfid, sizeof(out.rfid));
          return true;
        }
      }
    }
    file = dir.openNextFile();
  }
  return false;
}

bool sd_attendance_already_marked(int collegeID, const char* dateStr, const char* uid) {
  char path[200];
  sprintf(path, "%s/%d/attendance_%s.csv", ATTENDANCE_LOG_FOLDER, collegeID, dateStr);
  if (!SD.exists(path)) return false;
  File f = SD.open(path);
  if(!f) return false;
  while(f.available()){
    String line = f.readStringUntil('\n');
    // CSV: timestamp,roll_no,name,rfid,collegeID
    if (line.indexOf(uid) != -1) {
      f.close();
      return true;
    }
  }
  f.close();
  return false;
}

bool sd_mark_attendance(int collegeID, const char* dateStr, const Student &s) {
  // ensure path exists
  char folder[200];
  sprintf(folder, "%s/%d", ATTENDANCE_LOG_FOLDER, collegeID);
  SD.mkdir(folder);
  char path[256];
  sprintf(path, "%s/%d/attendance_%s.csv", ATTENDANCE_LOG_FOLDER, collegeID, dateStr);
  File f = SD.open(path, FILE_APPEND);
  if (!f) return false;
  extern String rtc_get_timestamp();
  String ts = rtc_get_timestamp();
  String line = ts + "," + String(s.roll_no) + "," + String(s.name) + "," + String(s.rfid) + "," + String(collegeID) + "\n";
  f.print(line);
  f.close();
  return true;
}

bool sd_append_student_delta(const Student &s) {
  // append to students_delta.json as a JSON lines file
  File f = SD.open("/students_delta.json", FILE_APPEND);
  if (!f) return false;
  DynamicJsonDocument doc(256);
  doc["roll_no"] = s.roll_no;
  doc["name"] = s.name;
  doc["rfid"] = s.rfid;
  String out;
  serializeJson(doc, out);
  out += "\n";
  f.print(out);
  f.close();
  return true;
}
