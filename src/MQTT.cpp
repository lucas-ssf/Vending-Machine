#include "MQTT.h"
#include <cstdlib>

extern char mqtt_server[40];        // Broker MQTT
extern char mqtt_port[6];           // Porta MQTT
const char* mqtt_id; // ID do cliente MQTT

char mqtt_topic[10][4][64];// topicos em que estará escrito - tem capacidade para 10 calhas com 3 topicos de 64 bytes
typedef enum { 
  ACIONAR,
  REABASTECER,
  ERRO,
  _STATUS, //a palavra STATUS já é usada por outra lib
} comandos; //Comandos para o mqtt_topic

mqtt_recebido_t mqtt_recebido;
mqtt_recebido_t*p_mqtt_recebido;

// Cliente 
WiFiClient espClient;
PubSubClient client(espClient);

int mqtt_acionar = LOW, mqtt_parar = LOW, mqtt_error = 0;

// Publica a mensagem no tópico MQTT
void publicar_status(int int_calha, char* mensagem){ // int_calha != id_calha
  char*topico = mqtt_topic[int_calha][_STATUS];
  Serial.print("Publicando no tópico: ");
  Serial.println(topico);
  Serial.print("Mensagem: ");
  Serial.print(mensagem);
  
  client.publish(topico, mensagem);
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print("(");
  Serial.print(length);
  Serial.print(")");
  Serial.println("Message: ");
  
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
  
}

// Função para conectar ao broker MQTT
void reconnect(int N_CALHAS) {
  // Tenta se conectar ao broker MQTT
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    
    if (client.connect(mqtt_id)) {
      Serial.println("Conectado!");
      for(int i = 0; i < N_CALHAS; i++){
        client.subscribe(mqtt_topic[i][ACIONAR]);
        client.subscribe(mqtt_topic[i][REABASTECER]);
        client.subscribe(mqtt_topic[i][ERRO]);

	Serial.println("Subscrito em:");
        Serial.println(mqtt_topic[i][ACIONAR]);
        Serial.println(mqtt_topic[i][REABASTECER]);
        Serial.println(mqtt_topic[i][ERRO]);
      }
    } else {
      Serial.println("Falha, tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup_mqtt(char* id_bandeja, int N_CALHAS, char**ids_calhas){
  mqtt_id = id_bandeja;
  p_mqtt_recebido = &mqtt_recebido;
  p_mqtt_recebido->recebido = false;

  Serial.print("Servidor MQTT:");
  Serial.print(mqtt_server);
  Serial.print(" | Porta: ");
  Serial.println(mqtt_port);
  client.setServer(mqtt_server, atoi(mqtt_port));
  client.setCallback(callback);

  inicializar_topicos(N_CALHAS, ids_calhas);
}

// inicializa os topicos tendo em conta:
// a)id da bandeja
// b)id de cada uma das calhas
void inicializar_topicos(int N_CALHAS, char**ids_calhas){
  for(int i = 0; i < N_CALHAS; i++){
    char acionar[64], reabastecer[64], erro[64], status[64];
    sprintf(acionar,     "%s/%s/%s/", mqtt_id, ids_calhas[i], "acionar"    );
    sprintf(reabastecer, "%s/%s/%s/", mqtt_id, ids_calhas[i], "reabastecer");
    sprintf(erro,        "%s/%s/%s/", mqtt_id, ids_calhas[i], "erro"       );
    sprintf(status,     "%s/%s/%s/", mqtt_id, ids_calhas[i], "status"     );

    strcpy(mqtt_topic[i][ACIONAR], acionar);
    strcpy(mqtt_topic[i][REABASTECER], reabastecer);
    strcpy(mqtt_topic[i][ERRO], erro);
    strcpy(mqtt_topic[i][_STATUS], status);
  }
}

