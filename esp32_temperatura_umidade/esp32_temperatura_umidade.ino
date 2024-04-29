/**
 * Script que faz a leitura dos dados do sensor DHT11 e enviar informações
 * de temperatura e umidade via protocolo MQTT
 *  
 * Links úteis das bibliotecas:
 * - PubSub: https://pubsubclient.knolleary.net/api
 * - WiFi: https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi
 * - Instalação esp32 no arduino ide: https://dl.espressif.com/dl/package_esp32_index.json
 */

#include <PubSubClient.h>
#include <WiFi.h>
#include <DHT.h>

#define DHTPIN 4      
#define DHTTYPE DHT11

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setupWiFi() {
  WiFi.begin("Moto G (5S) 1511", "clayste2112");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conectado à rede WiFI.");
  } else {
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Conectando WiFi...");
    }
  }
}

void setupMqtt() {
  client.setServer("broker.hivemq.com", 1883);

  while (!client.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao broker MQTT!");
    } else {
      Serial.print("Falha na conexão. Código de erro: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void enviarDadosBroker() {
  delay(2000);
  float humidity = dht.readHumidity();      
  float temperature = dht.readTemperature();

  char payload[50];
  sprintf(payload, "%.2f|%.2f", temperature, humidity);

  char topico[] = "senai_temp_umid";
  client.publish(topico, payload);

  delay(2000);
  client.publish(topico, "0.00|0.00");
}

void setup() {
  Serial.begin(115200);

  setupWiFi();
  setupMqtt();

  dht.begin();
}

void loop() {
  client.loop();
  delay(2000);

  enviarDadosBroker();
}
