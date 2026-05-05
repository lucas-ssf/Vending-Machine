// Nesta versão o motor de passo gira até que caia o produto
#include "Debug.h"
#include "WifiConfig.h"
#include "MQTT.h"
#include "Motor.h"

void setup() {
  Serial.begin(9600);

#if DEBUG_MOTOR == 1
  print_debug(DEBUG MOTOR); 
#else
  setup_wifi();
  conectar_wifi();

  setup_mqtt();
#endif
  setup_motor();
}

void loop() {
  loop_motor();
}

//TODO:
//calha01-maq01
