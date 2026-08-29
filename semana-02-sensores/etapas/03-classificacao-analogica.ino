// Incremento 3 — classificação do sinal analógico (fronteiras 399/400 e 749/750)
const int POT = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int bruto = analogRead(POT);
  Serial.print(bruto);
  Serial.print(" -> ");
  if (bruto < 400) {          // 0..399
    Serial.println("NORMAL");
  } else if (bruto < 750) {   // 400..749
    Serial.println("ATENCAO");
  } else {                    // 750..1023
    Serial.println("ALARME");
  }
  delay(500);
}
