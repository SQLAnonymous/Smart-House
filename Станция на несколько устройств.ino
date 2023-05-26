#include <ESP8266WiFi.h>
#include <espnow.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

uint8_t broadcastAddress1[] = {0x24, 0xA1, 0x60, 0x21, 0x09, 0xB1};
uint8_t broadcastAddress2[] = {0x24, 0xA2, 0x60, 0x21, 0x09, 0xB1};

int b = 0;
int e;
String status1;
String status2;

unsigned long lastTime = 0; 
unsigned long timerDelay = 100;

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (sendStatus != 0){
    Serial.println("Delivery fail");
  }
}

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&status1, incomingData, sizeof(status1));
  memcpy(&status2, incomingData, sizeof(status2));
  Serial.println(status1);
}

String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };
  k = source.length(); i = 0;
  while (i < k) {
    n = source[i]; i++;
    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
return target;
}

void setup() {
  display.cp437(true);
  
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  esp_now_register_send_cb(OnDataSent);

  esp_now_register_recv_cb(OnDataRecv);
  
  esp_now_add_peer(broadcastAddress1, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_add_peer(broadcastAddress2, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 28);
  display.println(utf8rus("Привет, пользователь!")); 
  display.display();
  delay(5000);
  display.clearDisplay();
  display.display();
}
 
void loop() { 
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(utf8rus("Устр1: "));
  display.print(utf8rus(status1));
  display.setCursor(0, 10);
  display.print(utf8rus("Устр2: "));
  display.display();
  if ((millis() - lastTime) > timerDelay) {
    if (Serial.available() > 0) {

      if(Serial.available() > 0) {
        b = Serial.parseInt();
        delay(10);
        Serial.println(b);
      }
      Serial.println(b);      
      if (b != 0){
        Serial.println(b);
        e = b / 10;
        switch(e){
          case 1:          
            esp_now_send(broadcastAddress1, (uint8_t *) &b, sizeof(b));
            lastTime = millis();
          case 2:
            esp_now_send(broadcastAddress2, (uint8_t *) &b, sizeof(b));
            lastTime = millis();  
          case 3:
            esp_now_send(broadcastAddress2, (uint8_t *) &b, sizeof(b));
            lastTime = millis();         
        }                                     
      }
    }
        b = 0;
  }
}
