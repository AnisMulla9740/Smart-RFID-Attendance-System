#include "server_sync.h"
#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SD.h>
#include "rtc_handler.h"

bool wifi_connect(){
  if (WiFi.status() == WL_CONNECTED) return true;
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(200);
  }
  return WiFi.status() == WL_CONNECTED;
}

bool server_fetch_and_save_students(int collegeID) {
  if (!wifi_connect()) return false;
  HTTPClient http;
  String url = String(SERVER_BASE_URL) + "/students?college_id=" + String(collegeID);
  http.begin(url);
  int code = http.GET();
  if (code != 200) {
    http.end();
    return false;
  }
  String payload = http.getString();
  http.end();

  // assume payload is JSON array of students; split into shards of 500 and write to SD
  DynamicJsonDocument doc(1024*50); // adjust
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    Serial.println("JSON parse error");
    return false;
  }
  JsonArray arr = doc.as<JsonArray>();
  int total = arr.size();
  int shardIdx = 0;
  for (int i=0; i<total; i+=STUDENT_SHARD_SIZE) {
    shardIdx++;
    int take = min(STUDENT_SHARD_SIZE, total - i);
    // create folder
    char folder[200];
    String date = rtc_get_date_string();
    sprintf(folder, "%s/%d/%s", ATTENDANCE_DATA_FOLDER, collegeID, date.c_str());
    SD.mkdir(String(folder).c_str());
    char fname[256];
    sprintf(fname, "%s/students_%04d.json", folder, shardIdx);
    File out = SD.open(fname, FILE_WRITE);
    if (!out) {
      Serial.println("Cannot open student shard for write");
      return false;
    }
    // create small JsonArray for this shard and serialize
    DynamicJsonDocument shardDoc(1024 * (take+10));
    JsonArray a = shardDoc.to<JsonArray>();
    for (int j=0; j<take; ++j) {
      JsonObject s = a.createNestedObject();
      JsonObject src = arr[i+j].as<JsonObject>();
      s["roll_no"] = src["roll_no"] | "";
      s["name"] = src["name"] | "";
      s["rfid"] = src["rfid"] | "";
    }
    serializeJson(a, out);
    out.close();
  }
  return true;
}

bool server_add_student(const Student &s) {
  if (!wifi_connect()) return false;
  HTTPClient http;
  String url = String(SERVER_BASE_URL) + "/students/add";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  DynamicJsonDocument doc(256);
  doc["roll_no"] = s.roll_no;
  doc["name"] = s.name;
  doc["rfid"] = s.rfid;
  String payload;
  serializeJson(doc, payload);
  int code = http.POST(payload);
  http.end();
  return (code == 200 || code == 201);
}

bool server_upload_attendance_day() {
  // iterate attendance logs folder and upload today's file for selected college
  // We need selected college id:
  extern Preferences prefs;
  int collegeID = prefs.getInt(PREF_SELECTED_COLLEGE, -1);
  if (collegeID < 0) return false;
  String date = rtc_get_date_string();
  char path[256];
  sprintf(path, "%s/%d/attendance_%s.csv", ATTENDANCE_LOG_FOLDER, collegeID, date.c_str());
  if (!SD.exists(path)) return false;
  // attempt upload
  File f = SD.open(path);
  if (!f) return false;
  String csv = "";
  while (f.available()) csv += (char)f.read();
  f.close();
  // upload
  if (!wifi_connect()) return false;
  HTTPClient http;
  String url = String(SERVER_BASE_URL) + "/attendance/upload";
  http.begin(url);
  http.addHeader("Content-Type", "text/csv");
  http.addHeader("X-College-ID", String(collegeID));
  int code = http.POST(csv);
  if (code == 200 || code == 201) {
    // create OK flag file
    char okPath[256];
    sprintf(okPath, "%s/%d/attendance_%s.ok", ATTENDANCE_LOG_FOLDER, collegeID, date.c_str());
    File ok = SD.open(okPath, FILE_WRITE);
    if (ok) {
      ok.print("OK");
      ok.close();
    }
    return true;
  }
  return false;
}
