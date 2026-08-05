// Nesta versão o calha de passo gira até que caia o produto
#include "Debug.h"
#include "WifiConfig.h"
#include "MQTT.h"
#include "Calha.h"
extern const int stepsPerRevolution = 200;

extern mqtt_recebido_t *p_mqtt_recebido; //ponteiro para uma struct do tipo mqtt_recebido, está definida em MQTT.h

const int N_CALHAS = 1; //número de calhas
char*ids_calhas[N_CALHAS]; //vetor com id de cada uma das calhas

char*id_bandeja = (char*)"bandeja01";

Calha calha[N_CALHAS]={
  Calha(
    (char*)"calha01", //id da calha
    18e3,             //tempo_max 
    15,               //velocidade
    21,               //enable_calha[0]
    32,               //enabla_calha2
    4,                //sensor_produto
    19,               //sensor_parada
    22,               //led_parado
    23,               //led_movimento
    Stepper(stepsPerRevolution, 27, 26, 25, 33) //myStepper
  )
};

void setup() {
  Serial.begin(9600);
  
  setup_wifi();
  conectar_wifi();

  for(int i = 0; i < N_CALHAS; i++){
    ids_calhas[i] = calha[i].getId();
    calha[i].setup_calha();
  }

  setup_mqtt(id_bandeja, N_CALHAS, ids_calhas); // é necessário informar o id da bandeja para que o tópico seja escrito corretamente
}

void loop() {
  int atual; // índice do calha que recebeu um comando
  if (!client.connected()) {
    reconnect(N_CALHAS);
  }
   // Processa as mensagens recebidas e mantém a conexão com o broker MQTT
  client.loop();

  if(p_mqtt_recebido->recebido){
    p_mqtt_recebido->recebido = false;

    for(int i = 0; i < N_CALHAS; i++){
      atual = i;

      if(strcmp(p_mqtt_recebido->calha, calha[i].getId()) == 0){
        break;
      }
    } 

    char*comando = p_mqtt_recebido->comando;
    int subcomando = atoi(p_mqtt_recebido->subcomando);
    int processa = calha[atual].processa_comando(comando,subcomando);
    if(processa == -1){ // se processa_mensagem retorna -1, quer dizer que houve um erro
      Serial.println("enviando erro...");
      publicar_status(atual,(char*)"erro");
    }
    else if (processa>=0){
      char contagem[3]; // máximo = 999
      sprintf(contagem,"%d",processa);
      publicar_status(atual,contagem);
    }
  } 
}

