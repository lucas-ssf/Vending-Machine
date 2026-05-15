// Nesta versão o motor de passo gira até que caia o produto
#include "Debug.h"
#include "WifiConfig.h"
#include "MQTT.h"
#include "Motor.h"
const int buttonPin = 13;
int buttonState = LOW;
extern const int stepsPerRevolution = 200;
extern mqtt_recebido_t *p_mqtt_recebido;
const int N_MOTORES = 1; //número de motores

//Processa a informção recebida pelo MQTT e repassa para o motor
void processa_mensagem(Motor*motor, mqtt_recebido_t*p_mqtt_recebido);

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

  // p_mqtt_recebido é uma struct do tipo mqtt_recebido, está definida em MQTT.h
  if(p_mqtt_recebido->recebido){
    p_mqtt_recebido->recebido = false;

    for(int i = 0; i < N_MOTORES; i++){
      if(strcmp(p_mqtt_recebido->calha, motor[i].getid()) == 0){
        atual = i;
        break;
      }
    } 

    motor[atual].verifica_sensor();
    //if (buttonState == HIGH) {
    //  delay(50);
    //  buttonState = (not (digitalRead(buttonPin)) || mqtt_acionar);
    //  if (buttonState == HIGH) {
    processa_mensagem(&motor[atual], p_mqtt_recebido);
//        Serial.println("movimenta");
//        motor[0].liberar_produto();
//        mqtt_acionar = LOW;
//        buttonState = LOW;
    //  }
    //}
  } 
}

void processa_mensagem(Motor*motor, mqtt_recebido_t*p_mqtt_recebido){
  char*comando = p_mqtt_recebido->comando;
  char*subcomando = p_mqtt_recebido->subcomando;

  if(strcmp(comando,"acionar") == 0){
    Serial.println("acionando motor...");
    motor->liberar_produto();
  }
  if(strcmp(comando,"reabastecer") == 0){
    motor->setContagem(atoi(subcomando));
  }
}
