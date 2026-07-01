#include <SPI.h>
#include <SD.h>

#define SD_CS 4  // Pin CS untuk Arduino Uno

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("=== TEST SD CARD - ARDUINO UNO ===");
  Serial.println("Wiring:");
  Serial.println("CS   -> Pin 4");
  Serial.println("MOSI -> Pin 11");
  Serial.println("MISO -> Pin 12");
  Serial.println("SCK  -> Pin 13");
  Serial.println("");

  Serial.println("Mencoba SD.begin...");

  if (!SD.begin(SD_CS)) {
    Serial.println("GAGAL - SD tidak terdeteksi!");
    return;
  }

  Serial.println("SD Card BERHASIL terdeteksi!");

  // Test tulis file
  Serial.println("Mencoba tulis file...");
  File file = SD.open("test.txt", FILE_WRITE);
  if (file) {
    file.println("SD Card Arduino Uno OK!");
    file.close();
    Serial.println("Tulis file: BERHASIL");
  } else {
    Serial.println("Tulis file: GAGAL");
  }

  // Test baca file
  Serial.println("Mencoba baca file...");
  file = SD.open("test.txt");
  if (file) {
    Serial.print("Isi file: ");
    while (file.available()) {
      Serial.write(file.read());
    }
    file.close();
  } else {
    Serial.println("Baca file: GAGAL");
  }

  Serial.println("=== TEST SELESAI ===");
}

void loop() {}
