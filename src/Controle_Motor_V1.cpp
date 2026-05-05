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
//testar motor

/*************************************************************************

*💡 Resumo do que falta para ficar mais legível e organizado:
*
*    Separar funções pequenas com uma responsabilidade cada.
*
*    Evitar delay()s longos que travam o loop.
*
*    Substituir #define por constexpr ou const.
*
*    Reduzir variáveis globais usando struct ou classe.
*
*    Encapsular MQTT e motor em módulos separados.
*
*    Limpar comentários redundantes e manter só explicações importantes.
*
*    Tornar loop_motor() mais enxuto, chamando funções auxiliares.
*
*    Possível futura refatoração para classe Motor e classe MQTTHandler. 


*************************************************************************/
