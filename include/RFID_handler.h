#pragma once
#include <Arduino.h>
#include <MFRC522.h>

void rfid_init(int ssPin, int rstPin);
bool rfid_card_present();
String rfid_read_uid();
