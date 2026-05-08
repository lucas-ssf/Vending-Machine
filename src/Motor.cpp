// Nesta versão o motor de passo gira até que caia o produto
#include "Debug.h"
#include "Motor.h"

// TODO:
// - adicionar parametro idmaq

#define MAX_PRODUTOS 1
#define VELOC_MOTOR 15 //RPM

                 // 60/VELOC_MOTOR = tempo de uma rotação
#define TEMPO_MAX (60/VELOC_MOTOR * MAX_PRODUTOS * 1e3) // tempo do ultimo produto chegar ate o sensor_parada, em milissegundos

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

Stepper myStepper(stepsPerRevolution,  27, 26, 25, 33);
const int enable_motor1 = 21, enable_motor2 = 32;

const int led_parado = 22, led_movimento = 23;
int tempoInicialButton = 0;

const int sensor_produto = 4; // Sensor optico
const int sensor_parada = 19;

int estado_sensor = 0;
int contador = 0;

class Motor {
  private:
    int enable_motor1, enable_motor2,
        sensor_produto, sensor_parada,
        led_parado, led_movimento;
  public:
    Motor(
               int enable_motor1, 
               int enable_motor2,
               int sensor_produto, 
               int sensor_parada,
               int led_parado,
               int led_movimento
              ){
      this-> enable_motor1  = enable_motor1; 
      this-> enable_motor2  = enable_motor2;
      this-> sensor_produto = sensor_produto; 
      this-> sensor_parada  = sensor_parada;
      this-> led_parado     = led_parado;
      this-> led_movimento  = led_movimento;
      setup_motor();
    } 
               
    void setup_motor() {
      pinMode(enable_motor1, OUTPUT);
      pinMode(enable_motor2, OUTPUT);
    
      pinMode(sensor_produto, INPUT_PULLUP);
    
      pinMode(sensor_parada, INPUT_PULLUP);
    
      // set the speed at 60 rpm:
      myStepper.setSpeed(VELOC_MOTOR);
      
      pinMode(led_parado, OUTPUT);
      pinMode(led_movimento, OUTPUT);
      digitalWrite(led_parado, HIGH);
      digitalWrite(led_movimento, LOW);
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
    
    void verifica_sensor(){
      if (digitalRead(sensor_parada)==HIGH) {
        freioMotor();
        delay(2000);
        while (digitalRead(sensor_produto)==LOW) Serial.println("sensor_produto"); //trava o codigo ate que o produto seja retirado
        Serial.println("Produto retirado!");
        sensor_otico();
      }
    }
    
    void liberar_produto(){
          digitalWrite(led_parado, LOW);
          digitalWrite(led_movimento, HIGH);
          movimenta();
          digitalWrite(led_parado, HIGH);
          digitalWrite(led_movimento, LOW);
    }
};
