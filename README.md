# ESP32 Smart RFID Attendance System

📌 A smart attendance system using **ESP32, RFID (RC522), DS3231 RTC, and I2C LCD**.  
It logs student entry/exit with timestamps and displays messages on LCD.

---

## 🚀 Features
- RFID card-based student authentication
- Date & Time logging using DS3231 RTC
- LCD display for user interaction
- Modular code structure (easy to maintain)
- Compatible with Arduino IDE

---

## 🛠️ Hardware Required
- ESP32 DevKit V1
- RC522 RFID Reader
- DS3231 RTC Module
- I2C LCD (16x2)
- RFID Cards/Tags
- Jumper wires & Breadboard

---

## 🔌 Circuit Diagram
![circuit](docs/circuit_diagram.png)

---

## 📂 File Structure
ESP32-RFID-Attendance-System/
│── RFID_Attendance_System.ino
│── src/
│── include/
│── examples/
│── docs/
│── README.md
│── LICENSE

---

## ⚡ Getting Started

### 1️⃣ Install Libraries
Install the following libraries in Arduino IDE:
- `MFRC522.h` → [RFID Library](https://github.com/miguelbalboa/rfid)
- `RTClib.h` or `DS3231.h`
- `LiquidCrystal_I2C.h`

### 2️⃣ Setup Arduino IDE
1. Go to **File > Preferences > Additional Board Manager URLs**  
https://dl.espressif.com/dl/package_esp32_index.json

2. Install **ESP32 Boards** from Boards Manager.

3. Select **ESP32 DevKit V1** as the board.

### 3️⃣ Upload Code
- Open `RFID_Attendance_System.ino` in Arduino IDE.
- Connect ESP32 via USB.
- Select **COM Port** & **Board**.
- Click **Upload** ✅.

---

## 📊 Example Output

LCD Display:
Welcome Student
Scan your RFID

Serial Monitor:
RFID Detected: 0xAB12CD34
Student: John Doe
Time: 10:45:12 Date: 30-08-2025
Attendance Marked ✅

---

## 📜 License
This project is licensed under the GPL 3.0 - see the [LICENSE](LICENSE) file for details.

---

## 🙌 Contribution
Pull requests are welcome. For major changes, open an issue first to discuss.
