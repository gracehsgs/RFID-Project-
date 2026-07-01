#include <SPI.h>
#include <SD.h>

#define SD_CS 5

void setup() {

  Serial.begin(115200);

  Serial.println("Memulai SD Card...");

  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card gagal terdeteksi!");
    return;
  }

  Serial.println("SD Card berhasil terhubung!");

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);

  Serial.print("Ukuran SD : ");
  Serial.print(cardSize);
  Serial.println(" MB");
}

void loop() {

}


