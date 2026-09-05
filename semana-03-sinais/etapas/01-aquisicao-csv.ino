// Etapa 2 — aquisição mínima em formato CSV: botao,bruto
const int BTN = 2;
const int POT = A0;

void setup() {
  pinMode(BTN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("botao,bruto");
}

void loop() {
  bool ativo = digitalRead(BTN) == LOW;   // 1 quando pressionado
  int bruto = analogRead(POT);
  Serial.print(ativo);
  Serial.print(",");
  Serial.println(bruto);
  delay(500);
}
