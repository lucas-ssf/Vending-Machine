// Nesta versão o motor de passo gira até que caia o produto
#include "WifiConfig.h"
#include "MQTT.h"
#include "Motor.h"

void setup() {
  setup_motor();
}

void loop() {
  loop_motor();
}
