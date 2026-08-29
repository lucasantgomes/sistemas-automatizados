# Casos de teste — Semana 2

Procedimento: ajuste primeiro o potenciômetro e a temperatura do DHT22, depois posicione o botão, aguarde uma nova linha no Monitor Serial e compare com o esperado.

- **Esperado:** definido a partir dos requisitos, antes da execução.
- **Obtido:** saída do Monitor Serial do Wokwi, executando o `sketch.ino` com o `diagram.json` desta pasta (execução em 16/10/2026).
- **Wokwi:** `[x]` = caso executado no simulador. Capturas em `../evidencias/` (T1–T7 em `../evidencias/casos/`).

> Como forçar valores exatos: clique no potenciômetro ou no DHT22 durante a simulação e use o controle deslizante. O valor bruto do potenciômetro aparece no Monitor Serial (`POT=`). **Leitura inválida do DHT22 (T7/P5):** com a simulação parada, remova o fio DATA (D4 → SDA) e execute; `readTemperature()` devolve NaN.

## 1. Casos digitais (botão + LED)

| Caso | Entrada observada | Esperado | Obtido | Situação | Wokwi |
|---|---|---|---|---|---|
| D1 | liberado / HIGH | LED apagado | `BTN=LIBERADO \| LED=OFF` | Aprovado | [x] |
| D2 | pressionado / LOW | LED aceso | `BTN=PRESSIONADO \| LED=ON` | Aprovado | [x] |
| D3 | cinco acionamentos | cinco respostas | 5 respostas, nenhum evento perdido | Aprovado | [x] |
| D4 | mantido pressionado | LED permanece aceso | `LED=ON` em todos os ciclos | Aprovado | [x] |

Significado lógico: com `INPUT_PULLUP` o pino fica em **HIGH quando liberado** e vai a **LOW quando pressionado**, porque o botão liga D2 ao GND. A variável `detectado = digitalRead(BTN) == LOW` corrige essa inversão.

## 2. Potenciômetro (mínimo, meio, máximo e fronteiras)

| Caso | Valor bruto | Esperado | Obtido | Situação | Wokwi |
|---|---|---|---|---|---|
| A-min | ~0 | NORMAL | `ESTADO=NORMAL` | Aprovado | [x] |
| A-399 | 399 | NORMAL (último da 1ª faixa) | `ESTADO=NORMAL` | Aprovado | [x] |
| A-400 | 400 | ATENÇÃO (primeiro da 2ª faixa) | `ESTADO=ATENCAO` | Aprovado | [x] |
| A-749 | 749 | ATENÇÃO (último da 2ª faixa) | `ESTADO=ATENCAO` | Aprovado | [x] |
| A-750 | 750 | ALARME (primeiro da 3ª faixa) | `ESTADO=ALARME` | Aprovado | [x] |
| A-max | ~1023 | ALARME | `ESTADO=ALARME` | Aprovado | [x] |

Fronteiras de temperatura no Wokwi (pot = 200): 29,9 °C → NORMAL; 30,0 °C → ATENCAO; 39,9 °C → ATENCAO; 40,0 °C → ALARME (todas aprovadas).

## 3. Casos integrados obrigatórios (T1–T7)

| Caso | Pot. | Temp. | Botão | Esperado | Obtido | Situação | Wokwi |
|---|---|---|---|---|---|---|---|
| T1 | 200 | 25 °C | livre | NORMAL; LED off | `LED=OFF ... ESTADO=NORMAL` | Aprovado | [x] |
| T2 | 500 | 25 °C | livre | ATENÇÃO; LED off | `LED=OFF ... ESTADO=ATENCAO` | Aprovado | [x] |
| T3 | 800 | 25 °C | livre | ALARME; LED off | `LED=OFF ... ESTADO=ALARME` | Aprovado | [x] |
| T4 | 200 | 35 °C | livre | ATENÇÃO; LED off | `LED=OFF ... ESTADO=ATENCAO` | Aprovado | [x] |
| T5 | 200 | 45 °C | livre | ALARME; LED off | `LED=OFF ... ESTADO=ALARME` | Aprovado | [x] |
| T6 | 200 | 25 °C | pressionado | NORMAL; LED on | `LED=ON ... ESTADO=NORMAL` | Aprovado | [x] |
| T7 | — | inválida | livre | FALHA DE SENSOR | `TEMP=INVALIDA \| ESTADO=FALHA DE SENSOR` | Aprovado | [x] |

## 4. Exercício adicional 1 — prioridade de alarmes (P1–P5)

Prioridade: **FALHA > ALARME > ATENÇÃO > NORMAL**.

| Caso | Pot. | Temp. | Esperado | Obtido | Situação | Wokwi |
|---|---|---|---|---|---|---|
| P1 | 900 | 45 °C | ALARME (duas causas, saída única) | `ALARME` | Aprovado | [x] |
| P2 | 500 | 45 °C | ALARME (temperatura supera atenção do pot.) | `ALARME` | Aprovado | [x] |
| P3 | 900 | 35 °C | ALARME (pot. supera atenção térmica) | `ALARME` | Aprovado | [x] |
| P4 | 500 | 35 °C | ATENÇÃO (duas atenções, nenhum alarme) | `ATENCAO` | Aprovado | [x] |
| P5 | 900 | inválida | FALHA DE SENSOR (falha supera alarme) | `FALHA DE SENSOR` | Aprovado | [x] |

P5 é o teste decisivo: se o programa respondesse ALARME, a falha estaria sendo avaliada tarde demais.

## 5. Exercício adicional 2 — histerese (pot = 200 em toda a sequência)

| Amostra | Temp. | Alarme antes | Ação | Esperado | Obtido (Wokwi) | Situação |
|---|---|---|---|---|---|---|
| H1 | 36 °C | desligado | não atinge 40 | ATENÇÃO | `ATENCAO` | Aprovado |
| H2 | 39 °C | desligado | ainda não atinge 40 | ATENÇÃO | `ATENCAO` | Aprovado |
| H3 | 40 °C | desligado | liga o alarme | ALARME | `ALARME` | Aprovado |
| H4 | 39 °C | ligado | mantém (> 37) | ALARME | `ALARME` | Aprovado |
| H5 | 38 °C | ligado | mantém (> 37) | ALARME | `ALARME` | Aprovado |
| H6 | 37 °C | ligado | desliga o alarme | ATENÇÃO | `ATENCAO` | Aprovado |
| H7 | 36 °C | desligado | permanece desligado | ATENÇÃO | `ATENCAO` | Aprovado |
| H8 | 29 °C | desligado | abaixo de atenção | NORMAL | `NORMAL` | Aprovado |

Sequência complementar 39,8 → 40,0 → 39,9 → 39,7 → 37,1 → 37,0 (temperatura alterada com a simulação rodando, ~2,6 s por passo): obtido `ATENCAO, ALARME, ALARME, ALARME, ALARME, ATENCAO`. Liga em 40,0, permanece até 37,1 e desliga somente em 37,0 (aprovado).

> Observação da execução: o DHT22 entrega uma leitura nova a cada ~2 s. Na primeira tentativa da sequência complementar, com passos de 1,3 s, o valor 40,0 não chegou a ser lido e o alarme não ligou. Repetida com passos de 2,6 s, o comportamento ficou como esperado. Não é defeito do programa: é o intervalo mínimo de leitura do sensor.
