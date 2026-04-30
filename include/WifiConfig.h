#ifndef WIFICONFIG_H
#define WIFICONFIG_H

#include <WiFiManager.h>
#include <EEPROM.h>

extern char mqtt_server[40];
extern char mqtt_port[6];

void saveConfigCallback();
void saveCredentials(const char* newSSID, const char* newPass);
void readCredentials();
void setup_wifi();
void conectar_wifi();

#endif