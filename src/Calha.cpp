// Nesta versão o calha de passo gira até que caia o produto
#include "Debug.h"
#include "Calha.h"

const int stepsPerRevolution = 200; 

Calha::Calha(
               const char* id, // nome da calha no topico mqtt
               int tempo_max,  // tempo máximo para o primeiro produto cair
               int velocidade, // velocidade do motor
               int enable1,    
               int enable2,
               int sensor_produto, // sensor que informa se o produto foi retirado
               int sensor_parada, // sensor que informa que o produto saiu da mola
               int led_parado,
               int led_movimento,
               Stepper myStepper
    ):
    myStepper(myStepper),
    contagem(0) // quantidade de produtos na máquina
                // só pode ser modificado via setContagem(int)
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
      setup_calha();
    } 
               
    void Calha::setup_calha() {
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
    void Calha::desligarCalha() {
      digitalWrite(27, LOW); digitalWrite(26, LOW);
      digitalWrite(25, LOW); digitalWrite(33, LOW);
      digitalWrite(enable1, LOW); digitalWrite(enable2, LOW);
    }
    
    void Calha::freioCalha() {
      Serial.println("[FREIO] Ativado...");
      digitalWrite(27, HIGH); digitalWrite(26, LOW);
      digitalWrite(25, HIGH); digitalWrite(33, LOW);
      delay(150); 
      desligarCalha(); 
    }
    
    void Calha::movimenta(){
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
    
    void Calha::sensor_otico() {
      delay(5);
      if (contagem > 0)contagem--;
      char mensagem[10];
      sprintf(mensagem, "%d\0", contagem);
      publicar_mensagem(MQTT_STATUS, mensagem);
    }
    
    void Calha::verifica_sensor(){
      if (digitalRead(sensor_parada)==HIGH) {
        freioCalha();
        delay(2000);
        while (digitalRead(sensor_produto)==LOW) Serial.println("sensor_produto"); //trava o codigo ate que o produto seja retirado
        Serial.println("Produto retirado!");
        sensor_otico();
      }
    }
    
    void Calha::liberar_produto(){
          digitalWrite(led_parado, LOW);
          digitalWrite(led_movimento, HIGH);
          movimenta();
          digitalWrite(led_parado, HIGH);
          digitalWrite(led_movimento, LOW);
    }

    const char* Calha::getid(){
      return id;
    }
    
    void Calha::setContagem(int reabastecer){
      contagem = reabastecer;
      Serial.print("contagem = ");
      Serial.println(contagem);
    }
