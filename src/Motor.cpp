// Nesta versão o motor de passo gira até que caia o produto
#include "Motor.h"

// TODO:
// - adicionar parametro idmaq

#define DEBUG 0

#define MAX_PRODUTOS 1
#define VELOC_MOTOR 15 //RPM

                  //60/VELOC_MOTOR = tempo de uma rotação
#define TEMPO_MAX (60/VELOC_MOTOR * MAX_PRODUTOS * 1e3) // tempo do ultimo produto chegar ate o sensor_parada, em milissegundos

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

Stepper myStepper(stepsPerRevolution,  27, 26, 25, 33);
const int enable_motor1 = 21, enable_motor2 = 32;

const int buttonPin = 13, led_verm = 22, led_amar = 23;
int buttonState = LOW;
int tempoInicialButton = 0;

const int sensor_produto = 4; // Sensor optico
const int sensor_parada = 19;

int estado_sensor = 0;
int contador = 0;

void setup_motor() {
  // initialize the serial port:
  Serial.begin(9600);

#if DEBUG == 1
  Serial.println("modo DEBUG");
#else
  // Conecta-se ao Wi-Fi
  setup_wifi();
  conectar_wifi();

  // Configura o broker MQTT
  setup_mqtt();
#endif
  pinMode(enable_motor1, OUTPUT);
  pinMode(enable_motor2, OUTPUT);

  pinMode(sensor_produto, INPUT_PULLUP);

  pinMode(sensor_parada, INPUT_PULLUP);

  // set the speed at 60 rpm:
  myStepper.setSpeed(VELOC_MOTOR);
  
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(led_verm, OUTPUT);
  pinMode(led_amar, OUTPUT);

}

// --- FUNÇÕES DE MOTOR ---
void desligarMotor() {
  digitalWrite(27, LOW); digitalWrite(26, LOW);
  digitalWrite(25, LOW); digitalWrite(33, LOW);
  digitalWrite(enable_motor1, LOW); digitalWrite(enable_motor2, LOW);
}

void freioMotor() {
  Serial.println("[FREIO] Ativado...");
  digitalWrite(27, HIGH); digitalWrite(26, LOW);
  digitalWrite(25, HIGH); digitalWrite(33, LOW);
  delay(150); 
  desligarMotor(); 
}

void movimenta(){
  int tempo_inicial = millis();
  digitalWrite(enable_motor1, HIGH); digitalWrite(enable_motor2, HIGH);

  while (digitalRead(sensor_parada) != HIGH && !(mqtt_error)) {
    myStepper.step(20);
    client.loop();
    if (mqtt_parar == HIGH){
      Serial.println("mqtt_parar = HIGH");
      break;
    }
    else if ((millis()-tempo_inicial) >= TEMPO_MAX*1.25){ //se o tempo for 25% maior do que o que deveria, aponta um erro
      Serial.println("Erro!");
      publicar_mensagem(MQTT_ERROR, "erro");
      mqtt_error = 1;
      break;
    }
  }
  Serial.println(millis()-tempo_inicial);
  mqtt_parar = LOW;
}

void sensor_otico() {
  int *p_contador = &contador;
  delay(5);
  if (*p_contador > 0)(*p_contador)--;
  char mensagem[10];
  sprintf(mensagem, "%d\0", *p_contador);
  publicar_mensagem(MQTT_STATUS, mensagem);
}


void loop_motor() {
  digitalWrite(led_verm, HIGH);
  digitalWrite(led_amar, LOW);

#if DEBUG != 1
  if (!client.connected()) {
    reconnect();
  }


  // Processa as mensagens recebidas e mantém a conexão com o broker MQTT
  client.loop();
#endif

  if (digitalRead(sensor_parada)==HIGH) {
    freioMotor();
    delay(2000);
    while (digitalRead(sensor_produto)==LOW) Serial.println("sensor_produto"); //trava o codigo ate que o produto seja retirado
    Serial.println("Produto retirado!");
    sensor_otico();
  }

 buttonState = ((not digitalRead(buttonPin)) || mqtt_acionar);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    delay(50);
    buttonState = ((not digitalRead(buttonPin)) || mqtt_acionar);
    if (buttonState == HIGH) {
      digitalWrite(led_verm, LOW);
      digitalWrite(led_amar, HIGH);
      Serial.println("movimenta");
      movimenta();
      mqtt_acionar = LOW;
      buttonState = LOW;
    }
  }
}
