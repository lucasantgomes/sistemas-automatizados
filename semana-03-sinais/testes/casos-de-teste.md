# Casos de teste — Semana 3

- **Esperado:** definido pelos requisitos antes da execução.
- **Obtido:** saída CSV do Monitor Serial do Wokwi (projeto https://wokwi.com/projects/478005589238822913, execução de 16/10/2026). O primeiro campo é o número da amostra.

## Aquisição (botão + potenciômetro bruto)

Formato da linha: `amostra,botao,bruto,pct,tempC,estado`

| Caso | Entrada | Esperado | Obtido | Situação | Wokwi |
|---|---|---|---|---|---|
| A1 | Botão solto; pot mínimo | botao=0; bruto próximo de 0 | `89,0,0,0.00,0.00,NORMAL` | Aprovado | [x] |
| A2 | Botão pressionado; pot mínimo | botao=1; bruto próximo de 0 | `91,1,0,0.00,0.00,NORMAL` | Aprovado | [x] |
| A3 | Botão solto; pot no meio | botao=0; bruto próximo de 512 | `94,0,512,50.05,50.05,NORMAL` | Aprovado | [x] |
| A4 | Botão solto; pot máximo | botao=0; bruto próximo de 1023 | `96,0,1023,100.00,100.00,PERIGO` | Aprovado | [x] |

## Escalonamento

| Bruto | Percentual esperado | Temperatura esperada | Obtido (pct, tempC) | Situação |
|---|---|---|---|---|
| 0 | 0,00 % | 0,00 °C | 0.00, 0.00 | Aprovado |
| 512 | 50,05 % (512/1023 > 0,5) | 50,05 °C | 50.05, 50.05 | Aprovado |
| 1023 | 100,00 % | 100,00 °C | 100.00, 100.00 | Aprovado |

A planilha reproduz os mesmos valores e ainda calcula 4–20 mA e 0–10 V (512 → 12,008 mA e 5,005 V).

## Estados e fronteiras

| Percentual | Esperado | Obtido (`classificar`) | Obtido (planilha) | Situação |
|---|---|---|---|---|
| −1 % | FALHA | FALHA | FALHA | Aprovado |
| 0 % | NORMAL | NORMAL | NORMAL | Aprovado |
| 69,99 % | NORMAL | NORMAL | NORMAL | Aprovado |
| 70 % | ATENÇÃO | ATENCAO | ATENÇÃO | Aprovado |
| 84,99 % | ATENÇÃO | ATENCAO | ATENÇÃO | Aprovado |
| 85 % | PERIGO | PERIGO | PERIGO | Aprovado |
| 100 % | PERIGO | PERIGO | PERIGO | Aprovado |
| 100,01 % | FALHA | FALHA | FALHA | Aprovado |

No Wokwi o potenciômetro só produz 0–1023, ou seja, 0–100 %. Os casos −1 % e 100,01 % foram testados como **teste de software** (chamando `classificar()` com o valor e na aba `fronteiras` da planilha), não como validação elétrica. Fronteira observada no ADC, no Wokwi: bruto 716 → 69,99 % (NORMAL, amostra 24) e bruto 717 → 70,09 % (ATENÇÃO, amostra 98).

## Média móvel (exercício adicional 1)

Sequência aplicada: 500, 504, 498, 502, 500, **1000**, 501, 499, 503, 500, 502. (Este exercício foi verificado executando o código com valores controlados; para gerar o print no Wokwi, cole `etapas/03-media-movel.ino` e gire o potenciômetro.)

| Amostra | Bruto | Filtrado obtido | Comentário |
|---|---|---|---|
| 1–5 | 500…500 | 500,00 → 500,80 | divide pela quantidade real (sem zeros artificiais) |
| 6 | 1000 | 600,80 | pico entra na janela |
| 7–10 | ~500 | 600,20 a 600,60 | pico ainda dentro da janela (5 saídas afetadas) |
| 11 | 502 | 501,00 | pico saiu da janela |

O filtro reduziu o pico de +500 para +100, mas atrasou a resposta: uma mudança real também levaria 5 amostras (2,5 s) para aparecer totalmente.

## Cálculo inverso (exercício adicional 2)

| Temperatura | Corrente esperada | Tensão esperada | Verificação direta | Situação |
|---|---|---|---|---|
| 0 °C | 4 mA | 0 V | 0 °C | Aprovado |
| 25 °C | 8 mA | 2,5 V | 25 °C | Aprovado |
| 50 °C | 12 mA | 5 V | 50 °C | Aprovado |
| 75 °C | 16 mA | 7,5 V | 75 °C | Aprovado |
| 100 °C | 20 mA | 10 V | 100 °C | Aprovado |

## Diagnóstico 4–20 mA (exercício adicional 3)

| Corrente | Percentual matemático | Esperado | Obtido (planilha) | Situação |
|---|---|---|---|---|
| 3,2 mA | −5 % | FALHA (abaixo da faixa) | FALHA | Aprovado |
| 4 mA | 0 % | NORMAL | NORMAL | Aprovado |
| 12 mA | 50 % | NORMAL | NORMAL | Aprovado |
| 15,2 mA | 70 % | ATENÇÃO | ATENÇÃO | Aprovado |
| 17,6 mA | 85 % | PERIGO | PERIGO | Aprovado |
| 20 mA | 100 % | PERIGO | PERIGO | Aprovado |
| 20,8 mA | 105 % | FALHA (acima da faixa) | FALHA | Aprovado |
