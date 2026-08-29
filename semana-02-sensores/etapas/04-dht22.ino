// Incremento 4 — leitura segura da temperatura do DHT22
#include <DHT.h>
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("FALHA DE SENSOR");
  } else {
    Serial.println(t);
  }
  delay(1000);
}
