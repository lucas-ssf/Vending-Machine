// Nesta versão o motor de passo gira até que caia o produto
#include "Debug.h"
#include "Motor.h"

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

int contador = 0; //adaptar para varios contadores

Motor::Motor(
               const char* id,
               int tempo_max, 
               int velocidade, 
               int enable1, 
               int enable2,
               int sensor_produto, 
               int sensor_parada,
               int led_parado,
               int led_movimento,
               Stepper myStepper
    ):
    myStepper(myStepper),
    contagem(0)
    {
      this-> id             = id;
      this-> tempo_max      = tempo_max;
      this-> velocidade     = velocidade;
      this-> enable1        = enable1; 
      this-> enable2        = enable2;
      this-> sensor_produto = sensor_produto; 
      this-> sensor_parada  = sensor_parada;
      this-> led_parado     = led_parado;
      this-> led_movimento  = led_movimento;
      setup_motor();
    } 
               
    void Motor::setup_motor() {
      pinMode(enable1, OUTPUT);
      pinMode(enable2, OUTPUT);
    
      pinMode(sensor_produto, INPUT_PULLUP);
    
      pinMode(sensor_parada, INPUT_PULLUP);
    
      myStepper.setSpeed(velocidade);
      
      pinMode(led_parado, OUTPUT);
      pinMode(led_movimento, OUTPUT);
      digitalWrite(led_parado, HIGH);
      digitalWrite(led_movimento, LOW);
    }
    
    // --- FUNÇÕES DE MOTOR ---
    void Motor::desligarMotor() {
      digitalWrite(27, LOW); digitalWrite(26, LOW);
      digitalWrite(25, LOW); digitalWrite(33, LOW);
      digitalWrite(enable1, LOW); digitalWrite(enable2, LOW);
    }
    
    void Motor::freioMotor() {
      Serial.println("[FREIO] Ativado...");
      digitalWrite(27, HIGH); digitalWrite(26, LOW);
      digitalWrite(25, HIGH); digitalWrite(33, LOW);
      delay(150); 
      desligarMotor(); 
    }
    
    void Motor::movimenta(){
      int tempo_inicial = millis();
      digitalWrite(enable1, HIGH); digitalWrite(enable2, HIGH);
    
      while (digitalRead(sensor_parada) != HIGH && !(mqtt_error)) {
        myStepper.step(20);
        client.loop();
        if (mqtt_parar == HIGH){
          Serial.println("mqtt_parar = HIGH");
          break;
        }
        else if ((millis()-tempo_inicial) >= tempo_max){
          Serial.println("Erro!");
          publicar_mensagem(MQTT_ERROR, "erro");
          mqtt_error = 1;
          break;
        }
      }
      Serial.println(millis()-tempo_inicial);
      mqtt_parar = LOW;
    }
    
    void Motor::sensor_otico() {
      delay(5);
      if (contagem > 0)contagem--;
      char mensagem[10];
      sprintf(mensagem, "%d\0", contagem);
      publicar_mensagem(MQTT_STATUS, mensagem);
    }
    
    void Motor::verifica_sensor(){
      if (digitalRead(sensor_parada)==HIGH) {
        freioMotor();
        delay(2000);
        while (digitalRead(sensor_produto)==LOW) Serial.println("sensor_produto"); //trava o codigo ate que o produto seja retirado
        Serial.println("Produto retirado!");
        sensor_otico();
      }
    }
    
    void Motor::liberar_produto(){
          digitalWrite(led_parado, LOW);
          digitalWrite(led_movimento, HIGH);
          movimenta();
          digitalWrite(led_parado, HIGH);
          digitalWrite(led_movimento, LOW);
    }

    const char* Motor::getid(){
      return id;
    }
    
    void Motor::setContagem(int reabastecer){
      contagem = reabastecer;
      Serial.print("contagem = ");
      Serial.println(contagem);
    }
