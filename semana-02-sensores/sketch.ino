/*
 * UC Sistemas Automatizados — Semana 2
 * Estação de monitoramento: processos, sensores e transdutores
 * Autor: Lucas Antonio Gomes (RA 823126987)
 *
 * Mapa de pinos
 *   BTN    D2  pushbutton (outro terminal em GND)  INPUT_PULLUP: liberado = HIGH, pressionado = LOW
 *   LED    D8  LED + resistor 220 ohm até GND       saída de sinalização
 *   POT    A0  potenciômetro (VCC 5V, GND, SIG A0)  variável contínua, 0..1023
 *   DHTPIN D4  DHT22 (VCC 5V, GND, DATA D4)         temperatura simulada em °C
 *
 * Regras (prioridade de cima para baixo):
 *   1. FALHA DE SENSOR : temperatura inválida (NaN)
 *   2. ALARME          : alarme de temperatura (histerese 40/37 °C) OU bruto >= 750
 *   3. ATENCAO         : t >= 30 °C OU bruto >= 400
 *   4. NORMAL          : nenhuma regra anterior satisfeita
 *   O botão comanda o LED diretamente e não altera o estado.
 */
#include <DHT.h>

const int BTN = 2;
const int LED = 8;
const int POT = A0;
#define DHTPIN 4
#define DHTTYPE DHT22

// Limites do protótipo (requisitos)
const int   POT_ATENCAO   = 400;   // 400..749  -> ATENCAO
const int   POT_ALARME    = 750;   // 750..1023 -> ALARME
const float T_ATENCAO     = 30.0;  // 30 a <40 °C -> ATENCAO
const float T_ALARME_LIGA = 40.0;  // liga o alarme de temperatura
const float T_ALARME_DESL = 37.0;  // desliga somente em 37 °C ou menos (histerese)

DHT dht(DHTPIN, DHTTYPE);

// Precisa ficar fora de loop(): guarda o estado anterior entre ciclos
bool alarmeTemperatura = false;

// Exercício adicional 1 — classificação com prioridade explícita (sem histerese)
const char* classificar(float t, int bruto) {
  if (isnan(t)) return "FALHA DE SENSOR";
  if (t >= T_ALARME_LIGA || bruto >= POT_ALARME) return "ALARME";
  if (t >= T_ATENCAO || bruto >= POT_ATENCAO) return "ATENCAO";
  return "NORMAL";
}

// Exercício adicional 2 — histerese do alarme de temperatura
void atualizarAlarmeTemperatura(float t) {
  if (isnan(t)) return;
  if (!alarmeTemperatura && t >= T_ALARME_LIGA) {
    alarmeTemperatura = true;          // liga em 40 °C
  } else if (alarmeTemperatura && t <= T_ALARME_DESL) {
    alarmeTemperatura = false;         // desliga em 37 °C
  }
}

const char* classificarComHisterese(float t, int bruto) {
  if (isnan(t)) return "FALHA DE SENSOR";
  atualizarAlarmeTemperatura(t);
  if (alarmeTemperatura || bruto >= POT_ALARME) return "ALARME";
  if (t >= T_ATENCAO || bruto >= POT_ATENCAO) return "ATENCAO";
  return "NORMAL";
}

void setup() {
  pinMode(BTN, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  dht.begin();
  Serial.println("Estacao de monitoramento - Semana 2");
}

void loop() {
  // 1. Adquirir entradas
  bool detectado = digitalRead(BTN) == LOW;   // corrige a inversão do pull-up
  int bruto = analogRead(POT);
  float t = dht.readTemperature();

  // 2. Atuar: o botão comanda o LED diretamente
  digitalWrite(LED, detectado ? HIGH : LOW);

  // 3. Decidir
  const char* estado = classificarComHisterese(t, bruto);

  // 4. Diagnosticar / publicar
  Serial.print("BTN=");
  Serial.print(detectado ? "PRESSIONADO" : "LIBERADO");
  Serial.print(" | LED=");
  Serial.print(detectado ? "ON" : "OFF");
  Serial.print(" | POT=");
  Serial.print(bruto);
  Serial.print(" | TEMP=");
  if (isnan(t)) {
    Serial.print("INVALIDA");
  } else {
    Serial.print(t, 1);
    Serial.print(" C");
  }
  Serial.print(" | ESTADO=");
  Serial.println(estado);

  delay(1000);
}
