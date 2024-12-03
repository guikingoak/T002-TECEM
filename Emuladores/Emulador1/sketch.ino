#include <WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"

#define DHTPIN 4 
DHTesp dht;


const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttTopic = "esp32/umidade";

WiFiClient espClient;
PubSubClient client(espClient);

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
  Serial.println("Inicializando...");


  dht.setup(DHTPIN, DHTesp::DHT22);
  delay(2000); 
  float humidity = dht.getHumidity();
  if (isnan(humidity)) {
    Serial.println("Erro inicial ao ler o DHT22. Verifique conexões e configuração.");
  } else {
    Serial.print("Umidade inicial lida: ");
    Serial.println(humidity);
  }

  
  setupWiFi();
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  if (!client.connected()) {
    while (!client.connect("ESP32Emulador1")) {
      delay(1000);
      Serial.println("Reconectando ao broker MQTT...");
    }
  }

  static unsigned long lastReadTime = 0;
  const unsigned long readInterval = 2000; 

  if (millis() - lastReadTime >= readInterval) {
    lastReadTime = millis();

    float humidity = dht.getHumidity();

    if (!isnan(humidity)) {
      char message[10];
      dtostrf(humidity, 1, 2, message);
      client.publish(mqttTopic, message);
      Serial.print("Umidade enviada: ");
      Serial.println(message);
    } else {
      Serial.println("Erro ao ler a umidade.");
      dht.setup(DHTPIN, DHTesp::DHT22); 
    }
  }

  delay(10); 
}
