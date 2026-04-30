#include "MQTT.h"

extern char mqtt_server[40]; // Broker MQTT
extern char mqtt_port[6];                        // Porta MQTT
const char* mqttClientID = "maq01";     // ID do cliente MQTT

// Cliente 
WiFiClient espClient;
PubSubClient client(espClient);

int mqtt_acionar = LOW, mqtt_parar = LOW, mqtt_error = 0;
extern int contador;

void publicar_mensagem(char* topico, char* mensagem){
  // Publica a mensagem no tópico MQTT
  Serial.print("Enviando para o MQTT no tópico: ");
  Serial.print(topico);
  Serial.print(" | Mensagem: ");
  Serial.println(mensagem);
  
  client.publish(topico, mensagem);
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(length);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (strcmp(topic,MQTT_ERROR)==0 && messageTemp=="limpar") {Serial.println("mqtt_error = 0");mqtt_error = 0;}
  if (messageTemp == "acionar") mqtt_acionar = HIGH;
  if (messageTemp == "parar") mqtt_parar = HIGH;
  if (messageTemp.startsWith("reabastecer")){
    String produtos_reabastecer_str = messageTemp.substring(12);
    contador = produtos_reabastecer_str.toInt();
    char mensagem[10];
    sprintf(mensagem, "%d\0", contador);
    publicar_mensagem(MQTT_STATUS, mensagem);
  }
}

// Função para conectar ao broker MQTT
void reconnect() {
  // Tenta se conectar ao broker MQTT
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    
    if (client.connect(mqttClientID)) {
      Serial.println("Conectado!");
      client.subscribe(MQTT_COMANDO);
      client.subscribe(MQTT_ERROR);
    } else {
      Serial.println("Falha, tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup_mqtt(){
  Serial.print("Servidor MQTT:");
  Serial.print(mqtt_server);
  Serial.print(" | Porta: ");
  Serial.println(mqtt_port);
  client.setServer(mqtt_server, atoi(mqtt_port));
  client.setCallback(callback);
}