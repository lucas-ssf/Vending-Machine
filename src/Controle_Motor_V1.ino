// Nesta versão o motor de passo gira até que caia o produto
#include <Stepper.h>
#include "WifiConfig.h"
#include "MQTT.h"

// TODO:
// - erro bloqueante[feito]
// - 2ndo sensor[]
// - adicionar parametro idmaq
// - led

#define DEBUG 1

#define MAX_PRODUTOS 5
#define VELOC_MOTOR 15 //RPM

#define TEMPO_MAX (60/VELOC_MOTOR * MAX_PRODUTOS * 1e3) // tempo do ultimo produto chegar ate o sensor_parada, em milissegundos

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

Stepper myStepper(stepsPerRevolution,  27, 26, 25, 33);

const int buttonPin = 13, redpin = 22, yellowpin = 23;
int buttonState = LOW;
int tempoInicialButton = 0;

const int sensor_produto = 4; // Sensor optico
const int sensor_parada = 19;

int estado_sensor = 0;
int contador = 0;

void setup() {
  // initialize the serial port:
  Serial.begin(9600);

#ifndef DEBUG
  // Conecta-se ao Wi-Fi
  setup_wifi();
  conectar_wifi();

  // Configura o broker MQTT
  setup_mqtt();
#else
  Serial.println("modo DEBUG");
#endif

  pinMode(sensor_produto, INPUT_PULLUP);

  pinMode(sensor_parada, INPUT_PULLUP);

  // set the speed at 60 rpm:
  myStepper.setSpeed(VELOC_MOTOR);
  
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(redpin, OUTPUT);
  pinMode(yellowpin, OUTPUT);
  //pinMode(pino_sensor, INPUT);

  digitalWrite(redpin, HIGH);
  digitalWrite(yellowpin, LOW);
}

void movimenta(){
  int tempo_inicial = millis();

  digitalWrite(redpin, LOW);
  while (digitalRead(sensor_parada) != HIGH && !(mqtt_error)) {
    digitalWrite(yellowpin, HIGH);
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
    // freio_motor()
  }
  Serial.println(millis()-tempo_inicial);
  mqtt_parar = LOW;
}

void sensor_otico() {
  delay(5);
  if (contador > 0)contador = contador - 1;
  char mensagem[10];
  sprintf(mensagem, "%d\0", contador);
  publicar_mensagem(MQTT_STATUS, mensagem);
}


void loop() {

#ifndef DEBUG
  if (!client.connected()) {
    reconnect();
  }


  // Processa as mensagens recebidas e mantém a conexão com o broker MQTT
  client.loop();
#endif

  if (digitalRead(sensor_parada)==HIGH) {
    delay(100);
    while (digitalRead(sensor_produto)==LOW) ; //trava o codigo ate que o produto seja retirado
    Serial.println("Produto retirado!");
    myStepper.step(0);//substituir por freio_motor()
    sensor_otico();
  }

 buttonState = (digitalRead(buttonPin) || mqtt_acionar);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    if(tempoInicialButton == 0)
      tempoInicialButton = millis();
    else if (buttonState == HIGH && ((millis() - tempoInicialButton) >= 10)) {
      //movimenta();
      Serial.println("movimenta");
      mqtt_acionar = LOW;
      tempoInicialButton = 0;
      buttonState = LOW;
    }
  }
}

