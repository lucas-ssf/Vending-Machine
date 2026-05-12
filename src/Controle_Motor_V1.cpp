// Nesta versão o motor de passo gira até que caia o produto
#include "Debug.h"
#include "WifiConfig.h"
#include "MQTT.h"
#include "Motor.h"
const int buttonPin = 13;
int buttonState = LOW;
extern const int stepsPerRevolution = 200;
extern mqtt_recebido *mensagem_recebida;
const int N_MOTORES = 1; //número de motores

Motor motor[N_MOTORES]={Motor(
  "calha01", //id
  18e3,      //tempo_max 
  15,        //velocidade
  21,        //enable_motor[0]
  32,        //enabla_motor2
  4,         //sensor_produto
  19,        //sensor_parada
  22,        //led_parado
  23,        //led_movimento
  Stepper(stepsPerRevolution, 27, 26, 25, 33) //myStepper
)
};

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
  for(int i = 0; i < N_MOTORES; i++) motor[i].setup_motor();
}

void loop() {
  int atual; // índice do motor que recebeu um comando
#if DEBUG_MOTOR != 1
  if (!client.connected()) {
    reconnect();
  }
   // Processa as mensagens recebidas e mantém a conexão com o broker MQTT
  client.loop();
#endif
  buttonState = (not (digitalRead(buttonPin)) || mqtt_acionar);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if(mensagem_recebida->recebido){
    mensagem_recebida->recebido = false;

    for(int i = 0; i < N_MOTORES; i++){
      if(strcmp(mensagem_recebida->calha, motor[i].getid()) == 0){
        atual = i;
        break;
      }
    } 

    motor[atual].verifica_sensor();
    //if (buttonState == HIGH) {
    //  delay(50);
    //  buttonState = (not (digitalRead(buttonPin)) || mqtt_acionar);
    //  if (buttonState == HIGH) {
        Serial.println("movimenta");
        motor[0].liberar_produto();
        mqtt_acionar = LOW;
        buttonState = LOW;
    //  }
    //}
  } 
}
