#ifndef CALHA_H
#define CALHA_H

// Nesta versão o calha de passo gira até que caia o produto
#include <Stepper.h>
#include "WifiConfig.h"
#include "MQTT.h"

class Calha{
  private:
    char* id;
    int tempo_max, velocidade,
        enable1, enable2,
        sensor_produto, sensor_parada,
        led_parado, led_movimento,
        contagem, erro;
    Stepper myStepper;
  public:
    Calha(
               char* id,
               int tempo_max,
               int velocidade,
               int enable1,
               int enable2,
               int sensor_produto,
               int sensor_parada,
               int led_parado,
               int led_movimento,
               Stepper myStepper
    );
    void setup_calha();
    void desligarCalha();
    void freioCalha();
    int  movimenta();
    void loop_calha();
    void decrementaContagem();
    int  verifica_sensor();
    void liberar_produto();
    char*getId();
    int processa_comando(char*comando,int subcomando);
};

#endif
