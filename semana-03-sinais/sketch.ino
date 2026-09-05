/*
 * UC Sistemas Automatizados — Semana 3
 * Aquisição documentada: leitura bruta, escalonamento, estado e registro CSV
 * Autor: Lucas Antonio Gomes (RA 823126987)
 *
 * Continua o circuito da Semana 2 (BTN D2, LED D8, POT A0, DHT22 D4).
 *
 * Tabela de E/S
 *   BTN_COMANDO  D2   DI   HIGH/LOW (INPUT_PULLUP)  ler comando
 *   LED_ALARME   D8   DO   OFF/ON                   sinalizar PERIGO
 *   POT_BRUTO    A0   AI   0..1023                  adquirir variável
 *   NIVEL_PCT    mem  REAL 0..100 %                 decidir e registrar
 *
 * Escala adotada: 0..1023 -> 0..100 % -> 0..100 °C
 * Estados: FALHA (fora de 0..100 %) > PERIGO (>= 85 %) > ATENCAO (>= 70 %) > NORMAL
 */

const int BTN = 2;
const int POT = A0;
const int LED_ALARME = 8;

const int   ADC_MAX     = 1023;   // Arduino Uno: 10 bits -> 0..1023
const float LIM_ATENCAO = 70.0;   // %
const float LIM_PERIGO  = 85.0;   // %

unsigned long amostra = 0;

// Escalonamento linear genérico: y = ymin + (x - xmin) * (ymax - ymin) / (xmax - xmin)
float escalonar(float x, float xmin, float xmax, float ymin, float ymax) {
  return ymin + (x - xmin) * (ymax - ymin) / (xmax - xmin);
}

// Prioridade: validade primeiro, depois severidade decrescente, normalidade por exclusão
const char* classificar(float pct) {
  if (pct < 0.0 || pct > 100.0) return "FALHA";
  if (pct >= LIM_PERIGO) return "PERIGO";
  if (pct >= LIM_ATENCAO) return "ATENCAO";
  return "NORMAL";
}

void setup() {
  pinMode(BTN, INPUT_PULLUP);
  pinMode(LED_ALARME, OUTPUT);
  Serial.begin(9600);
  Serial.println("amostra,botao,bruto,pct,tempC,estado");   // cabeçalho CSV
}

void loop() {
  bool botao = digitalRead(BTN) == LOW;                  // 1 = pressionado
  int bruto = analogRead(POT);
  float pct = escalonar(bruto, 0, ADC_MAX, 0.0, 100.0);  // = 100.0 * bruto / 1023.0
  float tempC = escalonar(pct, 0.0, 100.0, 0.0, 100.0);  // faixa 0..100 °C
  const char* estado = classificar(pct);

  digitalWrite(LED_ALARME, pct >= LIM_PERIGO ? HIGH : LOW);

  Serial.print(amostra++); Serial.print(",");
  Serial.print(botao);     Serial.print(",");
  Serial.print(bruto);     Serial.print(",");
  Serial.print(pct, 2);    Serial.print(",");
  Serial.print(tempC, 2);  Serial.print(",");
  Serial.println(estado);
  delay(500);
}
