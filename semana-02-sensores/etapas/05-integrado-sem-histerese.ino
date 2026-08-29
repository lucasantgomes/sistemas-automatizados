// Incremento 5 — decisão integrada (versão do exercício obrigatório, sem histerese)
#include <DHT.h>

const int BTN = 2;
const int LED = 8;
const int POT = A0;
#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(BTN, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  bool detectado = digitalRead(BTN) == LOW;
  digitalWrite(LED, detectado ? HIGH : LOW);

  int bruto = analogRead(POT);
  float t = dht.readTemperature();

  Serial.print("BTN=");
  Serial.print(detectado ? "PRESSIONADO" : "LIBERADO");
  Serial.print(" | POT=");
  Serial.print(bruto);
  Serial.print(" | TEMP=");

  if (isnan(t)) {
    Serial.print("INVALIDA | ESTADO=FALHA DE SENSOR");
  } else {
    Serial.print(t, 1);
    Serial.print(" C | ESTADO=");
    if (t >= 40.0 || bruto >= 750) {
      Serial.print("ALARME");
    } else if (t >= 30.0 || bruto >= 400) {
      Serial.print("ATENCAO");
    } else {
      Serial.print("NORMAL");
    }
  }

  Serial.println();
  delay(1000);
}
