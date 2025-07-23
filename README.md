# Bluetooth and WiFi Car 2-in-1 🚗📶

A versatile ESP32-based car project that can be controlled over both **Bluetooth** and **WiFi**! Seamlessly switch between modes and enjoy flexible remote control capabilities.

<img src="requirements/diagram.png" alt="Project Diagram" width="300"/>

---

## 🚦 Features

- Dual-mode control: Bluetooth & WiFi
- Easy setup with Arduino IDE
- Simple library and board manager integration
- Mobile and desktop browser support for WiFi mode

---

## 🛠️ Requirements

- **Arduino IDE:** `arduino-ide_2.3.6_Linux_64bit.AppImage` (Linux)
- **ESP32 Dev Board**

---

## 🔗 Board Manager Setup

For both Bluetooth and WiFi, add the following URLs to **Preferences > Additional Boards Manager URLs**:
1. [http://arduino.esp8266.com/stable/package_esp8266com_index.json](http://arduino.esp8266.com/stable/package_esp8266com_index.json)
2. [https://dl.espressif.com/dl/package_esp32_index.json](https://dl.espressif.com/dl/package_esp32_index.json)
3. [https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json](https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json)



---

## 🔵 ESP32 Car Using Bluetooth

- **Board Manager:** ESP32 by Espressif Systems (`3.2.1` for Bluetooth)
- **Library Manager:** No additional libraries required!

---

## 📶 ESP32 Car Using WiFi

- **Board Manager:** ESP32 by Espressif Systems (`2.0.11` for WiFi)
- **Connection:**  
  `station (mobile/desktop browser)` ⟷ `Access Point (ESP32 car)`

### Required Libraries

| Library | Link | Screenshot |
|---------|------|------------|
| **AsyncTCP** | [AsyncTCP GitHub](https://github.com/dvarrel/AsyncTCP) | ![AsyncTCP](requirements/lib1.png) |
| **ESPAsyncWebServer** | [ESPAsyncWebServer GitHub](https://github.com/lacamera/ESPAsyncWebServer) | ![ESPAsyncWebServer](requirements/lib2.png) |

---

## 📂 Project Structure
.<br>
├── Controller App.zip<br>
├── ESP32-Bluetooth-1<br>
│   └── ESP32-Bluetooth-1.ino<br>
├── ESP32-WIFI-2<br>
│   └── ESP32-WIFI-2.ino<br>
├── README.md<br>
├── requirements<br>
│   ├── 4.2v_3200mah_Litio_BOSSNEY.png<br>
│   ├── diagram.png<br>
│   ├── esp32-devkitc-v4-esp32-wifi-micro-usb.jpg<br>
│   ├── esp32s_38p_v4_goouuExpansionBoard.png<br>
│   ├── L298N_motorDriverModule.jpg<br>
│   ├── lib1.png<br>
│   ├── lib2.png<br>
│   └── Motor_eléctrico_TT_de_CC_de_3V-6V.jpg<br>
├── testing_motor_wheels<br>
│   └── test1.ino<br>
└── testing_motor_wheels++<br>
    └── test2.ino<br>

---

## 🤝 Contributing

Contributions and suggestions are welcome! Please open an issue or pull request.

---

## 📜 License

This project is licensed under the MIT License.

---

> **Nachtlicht**: Turning night drives into a smart experience!
