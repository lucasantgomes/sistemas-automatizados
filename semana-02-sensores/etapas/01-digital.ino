// Incremento 1 — botão como sensor digital e LED como atuador
const int BTN = 2;
const int LED = 8;

void setup() {
  pinMode(BTN, INPUT_PULLUP);   // liberado = HIGH, pressionado = LOW
  pinMode(LED, OUTPUT);
}

void loop() {
  bool detectado = digitalRead(BTN) == LOW;
  digitalWrite(LED, detectado ? HIGH : LOW);
}
