// Nesta versão o motor de passo gira até que caia o produto
#include "Debug.h"
#include "WifiConfig.h"
#include "MQTT.h"
#include "Motor.h"

const int buttonPin = 13;
int buttonState = LOW;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);

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
#if DEBUG_MOTOR != 1
  if (!client.connected()) {
    reconnect();
  }
   // Processa as mensagens recebidas e mantém a conexão com o b    roker MQTT
  client.loop();
#endif
  buttonState = ((not digitalRead(buttonPin)) || mqtt_acionar);
  // check if the pushbutton is pressed. If it is, the buttonSt    ate is HIGH:
  verifica_sensor();
  if (buttonState == HIGH) {
    delay(50);
    buttonState = ((not digitalRead(buttonPin)) || mqtt_acionar    );
    if (buttonState == HIGH) {
      Serial.println("movimenta");
      liberar_produto();
      mqtt_acionar = LOW;
      buttonState = LOW;
    }
  }
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
