// Nesta versão o motor de passo gira até que caia o produto
#include <Stepper.h>
#include "WifiConfig.h"
#include "MQTT.h"

// TODO:
// - adicionar parametro idmaq

void setup_motor();

void desligarMotor();

void freioMotor();

void movimenta();

void sensor_otico();

void loop_motor();

void verifica_sensor();

void liberar_produto();
