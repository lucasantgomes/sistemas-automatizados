// Exercício adicional 1 — média móvel de 5 amostras com janela circular
const int POT = A0;
const int N = 5;
int janela[N] = {0, 0, 0, 0, 0};
long soma = 0;
int indice = 0;
int quantidade = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("bruto,filtrado");
}

void loop() {
  int bruto = analogRead(POT);

  soma -= janela[indice];        // retira o valor mais antigo
  janela[indice] = bruto;        // guarda o novo
  soma += bruto;

  indice = (indice + 1) % N;     // avança circularmente
  if (quantidade < N) quantidade++;

  // divide pela quantidade real para não puxar a média para zero no início
  float filtrado = (float)soma / quantidade;

  Serial.print(bruto);
  Serial.print(",");
  Serial.println(filtrado, 2);
  delay(500);
}
