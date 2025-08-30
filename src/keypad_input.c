#include "keypad_input.h"
#include <Keypad.h>
#include "lcd_display.h"

// Keypad wiring (example 4x3)
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[KEYPAD_ROWS] = {19, 18, 17, 16};
byte colPins[KEYPAD_COLS] = {15, 14, 13};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

int keypad_get_college_id() {
  String s = "";
  lcd_print_line(0, "Enter College ID");
  lcd_print_line(1, "");
  while (true) {
    char k = keypad.getKey();
    if (k) {
      if (k == '#') break; // finish
      if (k == '*') {
        s = "";
        lcd_print_line(1, "");
        continue;
      }
      s += k;
      lcd_print_line(1, s.c_str());
    }
    delay(50);
  }
  return s.toInt();
}

bool keypad_add_pressed() {
  // for simplicity: detect long press of '*' as Add
  char k = keypad.getKey();
  if (k == '*') return true;
  return false;
}

Student keypad_get_new_student() {
  Student s;
  memset(&s, 0, sizeof(s));
  lcd_print_line(0, "Enter Roll:");
  String roll="";
  while (true) {
    char k = keypad.getKey();
    if (k) {
      if (k == '#') break;
      roll += k;
      lcd_print_line(1, roll.c_str());
    }
    delay(50);
  }
  strncpy(s.roll_no, roll.c_str(), sizeof(s.roll_no));

  lcd_print_line(0, "Enter Name:");
  String name="";
  while (true) {
    char k = keypad.getKey();
    if (k) {
      if (k == '#') break;
      name += k;
      lcd_print_line(1, name.c_str());
    }
    delay(50);
  }
  strncpy(s.name, name.c_str(), sizeof(s.name));

  lcd_print_line(0, "Enter RFID:");
  String rfid="";
  while (true) {
    char k = keypad.getKey();
    if (k) {
      if (k == '#') break;
      rfid += k;
      lcd_print_line(1, rfid.c_str());
    }
    delay(50);
  }
  strncpy(s.rfid, rfid.c_str(), sizeof(s.rfid));

  return s;
}
