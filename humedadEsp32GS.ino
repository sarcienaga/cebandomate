#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid ="Wifi Kill 2.4G";
const char* password = "ylyanasergyo9";

const char* AppSheetURL ="https://script.google.com/macros/s/AKfycbygNUd0H9523SeguXW_exVWv5RDPss7QJnrharQyAeJRUVWKo9KVDvJjFKg6eq1Z7s3/exec";

void setup() {
  Serial.begin(115200);
  initWifi();
}

void loop() {
  int sensorValue = analogRead(36);
  sensorValue = map(sensorValue, 1400, 4095, 100, 0);
  Serial.print("Humedad = ");
  Serial.print(sensorValue);
  Serial.println(" %");
  if(touchRead(32)< 50){
    Serial.println("Enviar dato");
    enviarDatosAGS(sensorValue);
  }else{
    Serial.println("En espera");
  }
  delay(100);
}

// Procedimiento para la conexión a la red WiFi de la ESP32
void initWifi(){
  Serial.print("Conectando a: ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  if (WiFi.status() != WL_CONNECTED){
    Serial.println("Fallo de conexión!");
  }
}

// Procedimiento para el envío de datos a la planilla
void enviarDatosAGS(int humedad){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    http.begin(AppSheetURL);
    http.addHeader("Content-Type", "application/json");
    String jsonData = "{\"hum\":\"" + String(humedad)+ "\"}";
    int httpResponseCode = http.POST(jsonData);
    if(httpResponseCode > 0){
       String response = http.getString();
       Serial.println(httpResponseCode);
       Serial.println(response);
    } else {
      Serial.print("Wrong Request POST: ");
      Serial.println(httpResponseCode);
    }
  http.end();
  }
}
