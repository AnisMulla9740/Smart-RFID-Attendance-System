#include "rfid_handler.h"
#include <SPI.h>
#include <MFRC522.h>

static MFRC522 mfrc(SS_PIN, RST_PIN);

void rfid_init(int ssPin, int rstPin){
  SPI.begin();
  mfrc.PCD_Init();
  delay(50);
}

bool rfid_card_present(){
  // returns true if new card present
  return mfrc.PICC_IsNewCardPresent() && mfrc.PICC_ReadCardSerial();
}

String rfid_read_uid(){
  if (!mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial()) return "";
  String uid = "";
  for (byte i = 0; i < mfrc.uid.size; ++i) {
    if (mfrc.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(mfrc.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  mfrc.PICC_HaltA();
  return uid;
}
