#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <RTClib.h>

RTC_DS3231 rtc;

// ================= WIFI =================
const char* ssid = "halo";
const char* password = "51515151";

// ================= MQTT =================
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "kampus/rfid/uid";

// ================= RFID =================
#define SS_PIN 5
#define RST_PIN 4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

MFRC522 rfid(SS_PIN, RST_PIN);

WiFiClient espClient;
PubSubClient client(espClient);

// ================= WIFI CONNECT =================
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

display.setCursor(0,0);
display.println("WiFi Connected");
display.println(WiFi.localIP());

display.display();

delay(2000);
}

// ================= MQTT CONNECT =================
void reconnectMQTT() {
  while (!client.connected()) {

    Serial.print("Menghubungkan MQTT...");

    String clientId = "ESP32-RFID-";
    clientId += String(random(1000, 9999));

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

  display.setCursor(0,0);
  display.println("RFID MQTT");
  display.println("Connecting...");
  display.display();

  // ================= RTC =================
  if (!rtc.begin()) {
    Serial.println("RTC tidak ditemukan!");
    while (1);
  }

  /*
    HANYA JALANKAN SEKALI!
    Setelah jam RTC sudah benar,
    beri komentar pada baris rtc.adjust() di bawah.
  */
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // ================= RFID =================
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID Siap");

  // ================= WIFI =================
  connectWiFi();

  // ================= MQTT =================
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!client.connected()) {
    reconnectMQTT();
  }

  client.loop();

  DateTime now = rtc.now();

  display.clearDisplay();
  display.setTextSize(1);

  display.setCursor(0,0);
  display.println("ABSENSI RFID");

  display.setCursor(0,18);
  display.print(now.day());
  display.print("/");
  display.print(now.month());
  display.print("/");
  display.println(now.year());

  display.setCursor(0,35);
  display.print(now.hour());
  display.print(":");

  if(now.minute()<10) display.print("0");
  display.print(now.minute());
  display.print(":");

  if(now.second()<10) display.print("0");
  display.println(now.second());

  display.setCursor(0,55);
  display.println("Tempel Kartu");

  display.display();

  // Cek kartu
  if (!rfid.PICC_IsNewCardPresent())
    return;

  if (!rfid.PICC_ReadCardSerial())
    return;

  // ================= AMBIL UID =================
  String uid = "";

  for (byte i = 0; i < rfid.uid.size; i++) {

    if (rfid.uid.uidByte[i] < 0x10)
      uid += "0";

    uid += String(rfid.uid.uidByte[i], HEX);
  }

  uid.toUpperCase();

  Serial.println("==============================");
Serial.print("UID      : ");
Serial.println(uid);

Serial.print("Tanggal  : ");
Serial.print(now.day());
Serial.print("/");
Serial.print(now.month());
Serial.print("/");
Serial.println(now.year());

Serial.print("Jam      : ");
Serial.print(now.hour());
Serial.print(":");

if(now.minute() < 10) Serial.print("0");
Serial.print(now.minute());
Serial.print(":");

if(now.second() < 10) Serial.print("0");
Serial.println(now.second());

Serial.println("==============================");


 display.clearDisplay();
display.setTextSize(1);

display.setCursor(0,0);
display.println("ABSENSI RFID");

display.setCursor(0,15);
display.print("UID:");
display.println(uid);

display.setCursor(0,35);

display.print(now.day());
display.print("/");
display.print(now.month());
display.print("/");
display.println(now.year());

display.setCursor(0,50);

display.print(now.hour());
display.print(":");

if(now.minute() < 10)
    display.print("0");

display.print(now.minute());
display.print(":");

if(now.second() < 10)
    display.print("0");

display.println(now.second());

display.display();

  Serial.print("UID: ");
  Serial.println(uid);

  // ================= JSON PAYLOAD =================
  String payload = "{";
payload += "\"uid\":\"" + uid + "\",";
payload += "\"device\":\"ESP32_01\",";
payload += "\"tanggal\":\"";
payload += String(now.day()) + "/";
payload += String(now.month()) + "/";
payload += String(now.year()) + "\",";
payload += "\"jam\":\"";
payload += String(now.hour()) + ":";

if(now.minute() < 10)
    payload += "0";

payload += String(now.minute()) + ":";

if(now.second() < 10)
    payload += "0";

payload += String(now.second()) + "\"}";

  // ================= KIRIM MQTT =================
// ================= KIRIM MQTT =================
if(client.publish(mqtt_topic,payload.c_str())){

    Serial.println("Berhasil");

    display.clearDisplay();
    display.setTextSize(1);

    display.setCursor(0,0);
    display.println("ABSENSI BERHASIL");

    display.setCursor(0,15);
    display.print("UID : ");
    display.println(uid);

    display.setCursor(0,35);
    display.print(now.day());
    display.print("/");
    display.print(now.month());
    display.print("/");
    display.println(now.year());

    display.setCursor(0,50);
    display.print(now.hour());
    display.print(":");

    if(now.minute() < 10)
        display.print("0");

    display.print(now.minute());
    display.print(":");

    if(now.second() < 10)
        display.print("0");

    display.println(now.second());

    display.display();

    delay(2000);   // Tampilkan selama 2 detik
}}