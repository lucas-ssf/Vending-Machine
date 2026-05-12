// Nesta versão o motor de passo gira até que caia o produto
#include <Stepper.h>
#include "WifiConfig.h"
#include "MQTT.h"

// TODO:
// - adicionar parametro idmaq

class Motor{
  private:
    const char* id;
    int tempo_max, velocidade,
        enable1, enable2,
        sensor_produto, sensor_parada,
        led_parado, led_movimento;
    Stepper myStepper;
  public:
    Motor(
               const char* id,
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
    void setup_motor();
    void desligarMotor();
    void freioMotor();
    void movimenta();
    void sensor_otico();
    void loop_motor();
    void verifica_sensor();
    void liberar_produto();
    const char*getid();
};
