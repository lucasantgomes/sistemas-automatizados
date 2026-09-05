// Etapa 3 — bruto -> percentual -> temperatura (0..100 °C)
// 100.0 e 1023.0 forçam cálculo em ponto flutuante (evita divisão inteira)
const int POT = A0;

void setup() {
  Serial.begin(9600);
  Serial.println("bruto,pct,tempC");
}

void loop() {
  int bruto = analogRead(POT);
  float pct = 100.0 * bruto / 1023.0;
  float tempC = pct;   // válido apenas para a faixa 0-100 °C

  Serial.print(bruto);
  Serial.print(",");
  Serial.print(pct, 2);
  Serial.print(",");
  Serial.println(tempC, 2);
  delay(500);
}
