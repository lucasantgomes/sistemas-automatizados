// Incremento 2 — leitura analógica bruta do potenciômetro (0..1023)
const int POT = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int bruto = analogRead(POT);
  Serial.println(bruto);
  delay(200);
}
