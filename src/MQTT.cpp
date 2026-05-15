#include "MQTT.h"
#include <cstdlib>

extern char mqtt_server[40];        // Broker MQTT
extern char mqtt_port[6];           // Porta MQTT
const char* mqtt_id = "bandeja01"; // ID do cliente MQTT
const char* mqtt_topic[] = { // topicos em que estará escrito
  "bandeja01/calha01/acionar/",  
  "bandeja01/calha01/reabastecer/",  
};
const int N_TOPICOS = sizeof(mqtt_topic)/sizeof(mqtt_topic[0]);

mqtt_recebido_t mqtt_recebido;
mqtt_recebido_t *p_mqtt_recebido;

// Cliente 
WiFiClient espClient;
PubSubClient client(espClient);

int mqtt_acionar = LOW, mqtt_parar = LOW, mqtt_error = 0;
extern int contador;

void publicar_mensagem(const char* topico, const char* mensagem){
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
  //char bandeja[64], calha[64], comando[64];
  
  message[length] = 0; // Esse filtro é necessário pois junto com a mensagem, vêm outros bytes aleatórios
  Serial.println((char*)message);

 /* Explicação sobre o strtok:
  * Um tópico como bandeja01/calha01/status
  * é divido em várias strings com base no "/".
  * String1 = bandeja01
  * String2 = calha01
  * String3 = status */
  char* token = strtok(topic, "/");
  for(int i=0; i < 3 && (token != NULL); i++){
    if(i==0){
      strcpy(p_mqtt_recebido->bandeja, token);
      p_mqtt_recebido->bandeja[strlen(token)]=0;
    }
    else if(i==1){
      strcpy(p_mqtt_recebido->calha, token);
      p_mqtt_recebido->calha[strlen(token)]=0;
    }
    else {
      strcpy(p_mqtt_recebido->comando, token);
      p_mqtt_recebido->comando[strlen(token)]=0;
    }
    token = strtok(NULL, "/");
  }
  if(message) strcpy(p_mqtt_recebido->subcomando, (char*)message);
  p_mqtt_recebido->recebido   = true;
  /*
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
*/
}

// Função para conectar ao broker MQTT
void reconnect() {
  // Tenta se conectar ao broker MQTT
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    
    if (client.connect(mqtt_id)) {
      Serial.println("Conectado!");
      for(int i = 0; i < N_TOPICOS; i++)
      client.subscribe(mqtt_topic[i]);
      //client.subscribe(MQTT_COMANDO);
      //client.subscribe(MQTT_ERROR);
    } else {
      Serial.println("Falha, tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup_mqtt(){
  p_mqtt_recebido = &mqtt_recebido;
  p_mqtt_recebido->recebido = false;

  Serial.print("Servidor MQTT:");
  Serial.print(mqtt_server);
  Serial.print(" | Porta: ");
  Serial.println(mqtt_port);
  client.setServer(mqtt_server, atoi(mqtt_port));
  client.setCallback(callback);
}
