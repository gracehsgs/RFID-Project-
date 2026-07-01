🎫 RFID Attendance System

<p align="center">
  <img src="https://img.shields.io/badge/Platform-ESP32-blue?style=for-the-badge&logo=espressif" />
  <img src="https://img.shields.io/badge/Protocol-MQTT-purple?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Storage-MicroSD-green?style=for-the-badge" />
  <img src="https://img.shields.io/badge/RealTime-Google%20Sheets-red?style=for-the-badge&logo=googlesheets" />
  <img src="https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge" />
</p>

Sistem absensi berbasis RFID menggunakan ESP32 yang mampu menyimpan data ke MicroSD, mengirim data secara realtime via MQTT, dan menampilkan informasi di layar OLED.




📋 Daftar Isi


Tentang Project
Fitur
Hardware yang Dibutuhkan
Skema Wiring
Instalasi Software
Konfigurasi
Cara Penggunaan
Struktur Data
Node-RED & Google Sheets
Troubleshooting



🚀 Tentang Project

RFID Attendance System adalah sistem absensi digital yang dirancang untuk menggantikan absensi manual yang memakan waktu dan rentan kesalahan. Dengan menempelkan kartu RFID, data kehadiran langsung tercatat secara otomatis dengan stempel waktu yang akurat.

Sistem ini menggunakan ESP32-WROOM-32D sebagai otak utama, yang terhubung ke berbagai modul seperti RFID reader, layar OLED, RTC untuk waktu akurat, dan MicroSD sebagai penyimpanan lokal. Data juga dikirim secara realtime ke broker MQTT dan dapat diteruskan ke Google Sheets menggunakan Node-RED.


✨ Fitur

FiturKeterangan🔖 Scan RFIDBaca kartu RFID dan identifikasi nama pemilik🖥️ Layar OLEDTampilkan waktu, tanggal, UID, dan nama secara realtime💾 Simpan ke MicroSDData tersimpan dalam format CSV di MicroSD📡 Kirim via MQTTData dikirim ke broker MQTT setiap scan🕐 RTC AkuratWaktu tetap akurat meski ESP32 restart🌐 Integrasi Node-REDData dapat diteruskan ke Google Sheets👤 Database NamaMapping UID ke nama pemilik kartu⚡ Dual SPI BusRFID dan SD Card berjalan di bus SPI terpisah


🛒 Hardware yang Dibutuhkan

KomponenJumlahESP32-WROOM-32D1MFRC522 RFID Reader1Kartu/Tag RFID 13.56MHzSesuai kebutuhanModul MicroSD Card1MicroSD Card (FAT32)1OLED SSD1306 128x641RTC DS32311Kabel JumperSecukupnyaBreadboard / PCB1
