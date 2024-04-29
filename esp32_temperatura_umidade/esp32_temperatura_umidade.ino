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
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

void setupWiFi() {
  //   WiFi.begin("Moto G (5S) 1511", "clayste2112");
  WiFi.begin("moto22", "moisesdcil");

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

void limparLcd() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Buscando Dados ");
  
  for (int i=0; i < 16; i++) {
    delay(200);
    lcd.setCursor(i, 1);
    lcd.print(".");
  }
}

void mostrarDadosLcd(float umidade, float temperatura) {
  String temp = "Temp: " + String(temperatura) + " C";
  String umi = " Hum: " + String(umidade) + " %";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(temp);
  lcd.setCursor(0, 1);
  lcd.print(umi);
}

void enviarDadosBroker() {
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();
  mostrarDadosLcd(umidade, temperatura);

  char dados[50];
  sprintf(dados, "%.2f|%.2f", temperatura, umidade);

  char topico[] = "senai_temp_umid";
  client.publish(topico, dados);

  delay(3000);
  client.publish(topico, "0.00|0.00");
  limparLcd();
}

void setup() {
  Serial.begin(115200);

  setupWiFi();
  setupMqtt();

  dht.begin();

  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
}

void loop() {
  client.loop();
  delay(2000);
  enviarDadosBroker();
}
