#include "WifiConfig.h"

WiFiManager wm;

int button_wm = 13;

bool shouldSaveConfig = false;

// Buffer for Wi-Fi credentials
char ssid[32];
char pass[32];

char mqtt_server[40];
char mqtt_port[6];


// id/name, placeholder/prompt, default, length
 WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);

// Callback to save config after web server updates SSID and password
void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// Save SSID and password to EEPROM
void saveCredentials(const char* newSSID, const char* newPass) {
  Serial.println("Saving WiFi credentials to EEPROM...");
  
  // Save SSID
  for (int i = 0; i < 32; i++) {
    EEPROM.write(0 + i, newSSID[i]);
  }
  // Save Password
  for (int i = 0; i < 32; i++) {
    EEPROM.write(100 + i, newPass[i]);
  }

  //MQTT
  for (int i = 0; i < 40; i++) {
   EEPROM.write(200 + i, mqtt_server[i]);
  }
  for (int i = 0; i < 6; i++) {
   EEPROM.write(300 + i, mqtt_port[i]);
  }

  EEPROM.commit();
}

// Read SSID and password from EEPROM
void readCredentials() {
  Serial.println("Reading WiFi credentials from EEPROM...");
  
  for (int i = 0; i < 32; i++) {
    ssid[i] = EEPROM.read(0 + i);
  }
  ssid[31] = '\0';

  for (int i = 0; i < 32; i++) {
    pass[i] = EEPROM.read(100 + i);
  }
  pass[31] = '\0';

  for (int i = 0; i < 40; i++) {
   mqtt_server[i] = EEPROM.read(200 + i);
  }
  for (int i = 0; i < 6; i++) {
   mqtt_port[i] = EEPROM.read(300 + i);
  }

  Serial.println("SSID: ");
  Serial.println(ssid);
  Serial.println("Password: ");
  Serial.println(pass);
  Serial.print("MQTT - ");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.print(mqtt_port);

  delay(5000);
}

void setup_wifi() {
  //Serial.begin(115200);
  EEPROM.begin(512);
  pinMode(button_wm, INPUT_PULLUP);

  // Set WiFiManager save config callback
  wm.setSaveConfigCallback(saveConfigCallback);

  if (digitalRead(button_wm) == LOW) {
    Serial.println("Button pressed, starting WiFiManager...");
    // After connecting, parameter.getValue() will get you the configured value
    // id/name placeholder/prompt default length
    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);

    //add all your parameters here
    wm.addParameter(&custom_mqtt_server);
    wm.addParameter(&custom_mqtt_port);

    //wm.resetSettings();  // Reset to enable setup mode
    wm.startConfigPortal("ESP32_Config");

    strcpy(mqtt_server, custom_mqtt_server.getValue());
    strcpy(mqtt_port, custom_mqtt_port.getValue());
    Serial.println("The values in the file are: ");
    Serial.println("\tmqtt_server : " + String(mqtt_server));
    Serial.println("\tmqtt_port : " + String(mqtt_port));

    // Save config if needed
    if (shouldSaveConfig) {
      saveCredentials(wm.getWiFiSSID().c_str(), wm.getWiFiPass().c_str());
      Serial.println("Credentials saved.");
      ESP.restart();  // Restart to apply settings
    }
  } else {
    // Load credentials from EEPROM
    readCredentials();
    Serial.print("Connecting to: ");
    Serial.println(ssid);
  }
}

// Função para conectar ao Wi-Fi
void conectar_wifi() {
  Serial.print("Conectando-se ao WiFi");
  Serial.println(custom_mqtt_server.getValue());
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println(" Conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}