#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <RTClib.h>
#include <FS.h>
#include <SD.h>

SPIClass spiSD(HSPI);

RTC_DS3231 rtc;

// ================= WIFI =================
const char* ssid = "halo";
const char* password = "51515151";

// ================= MQTT =================
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "kampus/rfid/uid";

// ================= PIN =================
#define SS_PIN  5
#define RST_PIN 4
#define SD_CS   32

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

String getNama(String uid) {
  if (uid == "C591AB04") return "James";
  if (uid == "B55B6B05") return "David";
  if (uid == "7B8FF751") return "Grace";
  return "Tidak Dikenal";
}

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
MFRC522 rfid(SS_PIN, RST_PIN);

WiFiClient espClient;
PubSubClient client(espClient);

// ================= WIFI =================
void connectWiFi() {
  Serial.print("Menghubungkan WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi Terhubung");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connected");
  display.println(WiFi.localIP());
  display.display();
  delay(2000);
}

// ================= MQTT =================
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan MQTT...");
    String clientId = "ESP32-RFID-" + String(random(1000, 9999));
    if (client.connect(clientId.c_str())) {
      Serial.println(" BERHASIL");
    } else {
      Serial.print(" GAGAL, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== BOOT ===");

  // ================= I2C =================
  Wire.begin(21, 22);

  // ================= OLED =================
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED gagal");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("RFID MQTT");
  display.println("Connecting...");
  display.display();

  // ================= RFID (VSPI) =================
  Serial.println("Init RFID...");
  SPI.begin(18, 19, 23, SS_PIN);
  pinMode(SS_PIN, OUTPUT);
  digitalWrite(SS_PIN, HIGH);
  rfid.PCD_Init();
  Serial.println("RFID Siap");

  // ================= SD CARD (HSPI) =================
  Serial.println("Init SD...");
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  delay(200);
  spiSD.begin(25, 26, 27, SD_CS);
  delay(200);
  if (!SD.begin(SD_CS, spiSD, 4000000)) {
    Serial.println("SD Card GAGAL!");
  } else {
    Serial.print("SD Card OK - Size(MB): ");
    Serial.println(SD.cardSize() / (1024 * 1024));
  }

  // ================= RTC =================
  Serial.println("Init RTC...");
  if (!rtc.begin()) {
    Serial.println("RTC tidak ditemukan!");
    while (1);
  }
  // Aktifkan HANYA sekali untuk set jam, lalu komen lagi
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println("RTC Siap");

  // ================= WIFI + MQTT =================
  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);

  Serial.println("=== SETUP SELESAI ===");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  if (!client.connected()) reconnectMQTT();
  client.loop();

  DateTime now = rtc.now();

  // ================= TAMPILAN IDLE =================
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ABSENSI RFID");
  display.setCursor(0, 18);
  display.print(now.day());
  display.print("/");
  display.print(now.month());
  display.print("/");
  display.println(now.year());
  display.setCursor(0, 35);
  display.print(now.hour());
  display.print(":");
  if (now.minute() < 10) display.print("0");
  display.print(now.minute());
  display.print(":");
  if (now.second() < 10) display.print("0");
  display.println(now.second());
  display.setCursor(0, 55);
  display.println("Tempel Kartu...");
  display.display();

  // ================= CEK KARTU =================
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  Serial.println("Kartu terdeteksi!");

  // ================= AMBIL UID =================
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  String nama = getNama(uid);

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  now = rtc.now();

  // Format tanggal dan jam
  String tanggal = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  String jam = "";
  if (now.hour() < 10) jam += "0";
  jam += String(now.hour()) + ":";
  if (now.minute() < 10) jam += "0";
  jam += String(now.minute()) + ":";
  if (now.second() < 10) jam += "0";
  jam += String(now.second());

  // ================= LOG SERIAL =================
  Serial.println("==============================");
  Serial.println("UID      : " + uid);
  Serial.println("Nama     : " + nama);
  Serial.println("Tanggal  : " + tanggal);
  Serial.println("Jam      : " + jam);
  Serial.println("==============================");

  // ================= TAMPILAN KARTU TERBACA =================
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("KARTU TERBACA");
  display.setCursor(0, 15);
  display.print("Nama: ");
  display.println(nama);
  display.setCursor(0, 30);
  display.print("UID: ");
  display.println(uid);
  display.setCursor(0, 45);
  display.println(tanggal);
  display.setCursor(0, 55);
  display.println(jam);
  display.display();
  delay(2000); // Tampilkan 2 detik

  // ================= SIMPAN KE SD CARD =================
  File file = SD.open("/absensi.csv", FILE_APPEND);
  if (file) {
    file.println(tanggal + "," + jam + "," + uid + "," + nama);
    file.close();
    Serial.println("Data tersimpan ke SD");
  } else {
    Serial.println("Gagal simpan ke SD!");
  }

  // ================= KIRIM MQTT =================
  String payload = "{\"uid\":\"" + uid + "\",\"nama\":\"" + nama + "\",\"device\":\"ESP32_01\",\"tanggal\":\"" + tanggal + "\",\"jam\":\"" + jam + "\"}";

  if (client.publish(mqtt_topic, payload.c_str())) {
    Serial.println("MQTT Berhasil dikirim");

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("ABSENSI BERHASIL");
    display.setCursor(0, 15);
    display.print("Nama: ");
    display.println(nama);
    display.setCursor(0, 30);
    display.print("UID: ");
    display.println(uid);
    display.setCursor(0, 45);
    display.println(tanggal);
    display.setCursor(0, 55);
    display.println(jam);
    display.display();
  } else {
    Serial.println("MQTT Gagal dikirim!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("MQTT GAGAL!");
    display.println("Coba lagi...");
    display.display();
  }

  delay(3000); // Jeda 3 detik sebelum scan kartu berikutnya
}
