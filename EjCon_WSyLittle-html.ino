// Agregar librerías
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include "LittleFS.h"

// Configuración de la red WiFi
const char* ssid = "Wifi Kill 2.4G";
const char* password = "ylyanasergyo9";

// Creación del servidor y del WebSocket
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Pin de entrada GPIO
const int gpioPin = 36;

// Función para manejar los mensajes WebSocket
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.println("Cliente conectado.");
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.println("Cliente desconectado.");
    } else if (type == WS_EVT_DATA) {
        Serial.print("Datos recibidos: ");
        for (size_t i = 0; i < len; i++) {
            Serial.print((char) data[i]);
        }
        Serial.println();
    }
}

// Función para leer el valor del GPIO y enviarlo por WebSocket en formato JSON
void sendGPIOData() {
    int value = analogRead(gpioPin);
    value = map(value, 2820, 4095, 100, 0);
    if(value>100){value=100;}
    JSONVar json;
    json["sensorValue"] = value;
    String jsonString = JSON.stringify(json);
    ws.textAll(jsonString);
}

// Inicializa LittleFS
void initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("Ha ocurrido un error mientras se montaba LittleFS");
  }
  Serial.println("LittleFS se cargó correctamente");
}

void setup() {
    // Inicialización de la comunicación serie
    Serial.begin(115200);

    // Conexión a la red WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando a WiFi...");
    }
    Serial.println("Conectado a la WiFi");
    Serial.println(WiFi.localIP());
    // Inicializa LittleFS
    initLittleFS();
    
    // Inicialización del servidor WebSocket
    ws.onEvent(onWebSocketEvent);
    server.addHandler(&ws);

    // Servir el archivo HTML desde la carpeta /data 
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ 
      request->send(LittleFS, "/index.html", String(), false); 
    });

    // Servir el archivo HTML desde la carpeta /data 
    server.on("/estilo.css", HTTP_GET, [](AsyncWebServerRequest *request){ 
      request->send(LittleFS, "/estilo.css", String(), false); 
    });

    // Servir el archivo HTML desde la carpeta /data 
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){ 
      request->send(LittleFS, "/script.js", String(), false); 
    });
    
    // Inicio del servidor HTTP
    server.begin();
    Serial.println("Servidor HTTP iniciado");

    // Configuración del pin GPIO
    pinMode(gpioPin, INPUT);
}

void loop() {
    // Envía los datos del GPIO cada segundo
    sendGPIOData();
    delay(1000);
}
