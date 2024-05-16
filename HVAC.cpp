#include "thingProperties.h"
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>


#define DHTPIN 17                   // Pin donde estÃ¡ conectado el DHT11
#define DHTTYPE DHT11               // Tipo de DHT
#define LEDPIN 2                    // Pin para controlar el LED o relÃ©
#define TEMPERATURE_THRESHOLD 27.5  // Umbral de temperatura en grados Celsius
#define INTERVAL 10000              // Umbral de temperatura en grados Celsius

DHT dht(DHTPIN, DHTTYPE);

const char* serverName = "https://inmotics.eastus.cloudapp.azure.com/api/collections/consumo/records";
String jsonData = "{\"dispositivo\": [\"vs4s49xwwkbzi6y\",\"7oowl118149myoa\"]}";
HTTPClient http;

unsigned long previousMillis = 0;  // will store last time the HTTP request was sent

void setup() {
  Serial.begin(9600);
  delay(1500);

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  dht.begin();

  initProperties();  // Inicializa las propiedades de IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
  if (control_manual) {
    controlManual();
  } else {
    controlAutomatico();
  }
}

void controlManual() {
  // AquÃ­ puedes aÃ±adir la lÃ³gica que permite controlar manualmente el HVAC
  if (hvac) {
    digitalWrite(LEDPIN, HIGH);  // Ejemplo: Encender algo si hvac es true
    sendPostRequest();           // Enviar solicitud POST
  } else {
    digitalWrite(LEDPIN, LOW);  // Ejemplo: Apagar si hvac es false
  }
}

void controlAutomatico() {
  float temp = dht.readTemperature();
  temperatura = temp;
  if (isnan(temp)) {
    Serial.print("E-DHT11! ");
    return;
  }

  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println(" *C");

  if (temp > TEMPERATURE_THRESHOLD) {
    digitalWrite(LEDPIN, HIGH);
    sendPostRequest();  // Enviar solicitud POST
  } else {
    digitalWrite(LEDPIN, LOW);
  }
}

void sendPostRequest() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= INTERVAL) {
    previousMillis = currentMillis;

    // Optionally disable certificate verification (not recommended for production)
    http.begin(serverName /* rootCA */);

    http.addHeader("Content-Type", "application/json");

    Serial.println("Sending HTTP POST request...");
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}


void onControlManualChange() {
  Serial.println("Modo manual actualizado");
  controlManual();  // Actualiza el estado manualmente
}

void onHvacChange() {
  Serial.println("Estado HVAC actualizado");
  controlManual();  // Actualiza el estado de HVAC en modo manual
}
