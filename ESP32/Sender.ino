
//LoRa Sender
//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Randy Libraries
#include <TinyGPSPlus.h>


//Libraries for GPS
//#include <TinyGPS++.h>                      
#include <ArduinoJson.h>
//#include <RadioLib.h>
//#include "boards.h"
#define   ERR_NONE   0

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <U8x8lib.h>

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 866E6

//OLED pins
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST 23
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// A sample NMEA stream.
const char *gpsStream =
  "$GPRMC,045103.000,A,3014.1984,N,09749.2872,W,0.67,161.46,030913,,,A*7C\r\n"
  "$GPGGA,045104.000,3014.1985,N,09749.2873,W,1,09,1.2,211.6,M,-22.5,M,,0000*62\r\n"
  "$GPRMC,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
  "$GPGGA,045201.000,3014.3864,N,09748.9411,W,1,10,1.2,200.8,M,-22.5,M,,0000*6C\r\n"
  "$GPRMC,045251.000,A,3014.4275,N,09749.0626,W,0.51,217.94,030913,,,A*7D\r\n"
  "$GPGGA,045252.000,3014.4273,N,09749.0628,W,1,09,1.3,206.9,M,-22.5,M,,0000*6F\r\n";

// The TinyGPSPlus object
TinyGPSPlus gps;
//JSONVar data;


//packet counter
int counter = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);

  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
 
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA SENDER ");
  display.display();
 
  Serial.println("LoRa Sender Test");

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
 
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0,10);
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);
}

void loop() {
   
  Serial.print("Sending packet: ");
  Serial.println(counter);

  gps.encode(*gpsStream);

  DynamicJsonDocument doc(200);
  doc["lat"] = 38.8951;
  doc["lng"] = -77.0364;
  doc["counter"] = counter;

  String data_json;
  serializeJson(doc, data_json);
  Serial.println(data_json);
//  StaticJsonDocument<200> doc;
//  doc["lat"] = gps.location.lat();
//  doc["lng"] = gps.location.lng();
//
//  // Generate the minified JSON and send it to the Serial port.
//  //
//  char out[128];
//  int b =serializeJson(doc, out);
//  Serial.print("bytes = ");
//  Serial.println(b,DEC);

  //Send LoRa packet to receiver
  LoRa.beginPacket();
  //LoRa.print("hello ");
  LoRa.print(data_json);
//  LoRa.print(b);
  LoRa.endPacket();

  String lat = doc["lat"];
  String lng = doc["lng"];
 
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("LORA SENDER");
  display.setCursor(0,20);
  display.setTextSize(1);
  display.print("LoRa packet sent.");
  display.setCursor(0,30);
  display.print("Counter:");
  display.setCursor(50,30);
  display.print(counter);
  display.setCursor(0,40);
  display.print("Latitude:");
  display.setCursor(65,40);
  display.print(lat);
  display.setCursor(0,50);
  display.print("Longitude:");
  display.setCursor(65,50);
  display.print(lng);      
  display.display();

  counter++;
 
  delay(10000);
}
