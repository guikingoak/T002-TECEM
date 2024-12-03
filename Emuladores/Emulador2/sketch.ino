#include <WiFi.h>
#include <PubSubClient.h>


#define LED_VERDE 13
#define LED_AMARELO 12
#define LED_VERMELHO 14
#define BUZZER 27


const char* ssid = "Wokwi-GUEST";
const char* password = "";


const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttTopic = "esp32/umidade";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  float humidity = atof(message);
  Serial.print("Umidade recebida: ");
  Serial.println(humidity);


  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  if (humidity >= 0 && humidity <= 25) {
    digitalWrite(LED_VERDE, HIGH);
  } else if (humidity > 25 && humidity <= 75) {
    digitalWrite(LED_AMARELO, HIGH);
  } else if (humidity > 75) {
    digitalWrite(LED_VERMELHO, HIGH);
  }


  if (humidity > 80) {
    for (int i = 0; i < 5; i++) {
      digitalWrite(BUZZER, HIGH);
      delay(200);
      digitalWrite(BUZZER, LOW);
      delay(200);
    }
  }
}

void setupWiFi() {
  Serial.print("Conectando ao WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" Conectado!");
}

void setup() {
  Serial.begin(115200);
  setupWiFi();

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    while (!client.connect("ESP32Emulador2")) {
      delay(1000);
      Serial.println("Reconectando ao MQT");
    }
    client.subscribe(mqttTopic);
  }

  client.loop();
}
