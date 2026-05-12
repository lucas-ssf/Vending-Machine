#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>
#include <stdlib.h>
#include <string>
#include "WifiConfig.h"

#define MQTT_STATUS "vending/status"           // Tópico MQTT para publicar mensagens
#define MQTT_ERROR "vending/error"
#define MQTT_COMANDO "vending/comando"

typedef struct{
    char *bandeja, *calha, *comando, *subcomando;
    bool recebido;
}mqtt_recebido;

extern PubSubClient client;
extern int mqtt_acionar, mqtt_parar, mqtt_error;
// extern int contador;
extern char mqtt_server[40];
extern char mqtt_port[6];

void publicar_mensagem(const char* topico, const char* mensagem);
void callback(char* topic, byte* message, unsigned int length);
void reconnect();
void testar_conexao();
void setup_mqtt();

#endif