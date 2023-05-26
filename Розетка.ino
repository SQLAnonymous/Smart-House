// Подключение библиотек........................................................................................................
#include <ESP8266WiFi.h>
#include <espnow.h>

// MAC адрес станции............................................................................................................
uint8_t broadcastAddress[] = {0xE0, 0x98, 0x06, 0x0F, 0x58, 0x07};

// Блок с переменными...........................................................................................................
int b;// Комманда от станции
const int relay = 5;// Порт реле
String status1 = "";// Статус прибора
int buffer = 0;// Буферная переменная

// Задержки для передачи данных..................................................................................................
unsigned long lastTime = 0;  
unsigned long timerDelay = 100;

// Функция для приёма данных со станции.........................................................................................
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&b, incomingData, sizeof(b));
  Serial.println(b);
}

// Функция передачи данных в станцию............................................................................................
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (sendStatus != 0){
    Serial.println("Delivery fail");
  }
}

// Функия настроек и инициализации датчиков и модулей(стандартная функция, выполняется один раз).................................
void setup() {
  Serial.begin(115200);// Определение скорости передачи данных между микроконтроллером и датчиками
  
  pinMode(relay, OUTPUT);// Определение статуса пина для реле

  WiFi.mode(WIFI_STA);// Определение устройства как Wi-Fi станции

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO); //Роль платы в системе 

  esp_now_register_recv_cb(OnDataRecv); //Определение функции приёма

  esp_now_register_send_cb(OnDataSent); //Определение функции отправки

  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0); //Регистрация пинов

  digitalWrite(relay, HIGH);// Отключение реле при включении розетки, может спасти при выключении электричества
    
  status1 = "OFF";
  esp_now_send(broadcastAddress, (uint8_t *) &status1, sizeof(status1));
  lastTime = millis();

}

// Функция, в которой всё выполняется бесконечно(стандартная)...................................................................
void loop() {
// Включение или выключение розетки, взависимости от команды станции
  if (b != 0) {
    switch (b){
      case 10:
        digitalWrite(relay, HIGH);
        status1 = "Выкл";
        break;
      case 11:
        digitalWrite(relay, LOW);
        status1 = "Вкл";
        break;
      }

// Отправка статуса розетки: вкл. или выкл.
    if ((millis() - lastTime) > timerDelay){
      if (buffer != b){//Костыль, не трогать!
        buffer = b;
        Serial.println(status1);
        esp_now_send(broadcastAddress, (uint8_t *) &status1, sizeof(status1));
        lastTime = millis();
        status1 = "";
      }
    }
  }
}