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

WiFiClient espClient;
PubSubClient client(espClient);

const char *HOST = "sua rede WiFi";
const char *PASSWORD = "sua senha da rede";
const char *MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;

void setupWiFi()
{

  // Faz a conexão
  WiFi.begin(HOST, PASSWORD);

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Conectado à rede WiFI.");
  }
  else
  {
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
      Serial.println("Conectando WiFi...");
    }
  }
}

void setupMqtt()
{
  client.setServer(MQTT_BROKER, MQTT_PORT);

  while (!client.connected())
  {
    String clientIdStr = "esp32_" + String(random(0xffff), HEX);
    char *clientId = clientIdStr.c_str();

    if (client.connect(clientId))
    {
      Serial.println("Conectado ao broker MQTT!");
    }
    else
    {
      Serial.print("Falha na conexão. Código de erro: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup()
{
  Serial.begin(115200);

  setupWiFi();
  setupMqtt();
}

void loop()
{
  client.loop();
}
