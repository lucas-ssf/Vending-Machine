#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>
#include <stdlib.h>
#include <string>
#include "WifiConfig.h"

typedef struct{
    char bandeja[64], calha[64], comando[64], subcomando[64];
    bool recebido;
}mqtt_recebido_t;

extern mqtt_recebido_t*p_mqtt_recebido;
extern PubSubClient client;
extern int mqtt_acionar, mqtt_parar, mqtt_error;
extern char mqtt_server[40];
extern char mqtt_port[6];

void publicar_status(int int_calha, char* mensagem);
void callback(char* topic, byte* message, unsigned int length);
void reconnect(int N_CALHAS);
void testar_conexao();
void setup_mqtt(char* id_bandeja, int N_CALHAS, char**ids_calhas);
void inicializar_topicos(int N_MOTORERS, char**ids_calhas);

#endif
