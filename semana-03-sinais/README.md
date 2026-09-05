# Semana 3 — Sinais analógicos e digitais

**UC Sistemas Automatizados** · Prof. Calvetti · Engenharia de Software — USJT
**Autor:** Lucas Antonio Gomes (RA 823126987) · **Data:** 05/09/2026

## Links

- Projeto Wokwi: [abrir simulação](https://wokwi.com/projects/478005589238822913)
- Google Planilhas: [abrir planilha](https://docs.google.com/spreadsheets/d/1YLBdpqzNJDpKwzCJlyFs-IPuvSKcwL40Cb6CQU-KXb8/edit?usp=sharing)

## Objetivo

Evoluir a estação da Semana 2 para **adquirir, escalonar, classificar e registrar** uma variável: leitura bruta do ADC → percentual → grandeza de engenharia (0–100 °C) → estado (NORMAL, ATENÇÃO, PERIGO, FALHA), com registro em CSV, verificação independente na planilha e gráfico de tendência.

## Estrutura

```
semana-03-sinais/
├── README.md
├── sketch.ino                 ← programa integrado (aquisição + escala + estado + CSV)
├── diagram.json               ← mesmo circuito da Semana 2
├── libraries.txt
├── etapas/
│   ├── 01-aquisicao-csv.ino   ← Etapa 2: botao,bruto
│   ├── 02-escalonamento.ino   ← Etapa 3: bruto,pct,tempC
│   └── 03-media-movel.ino     ← Exercício adicional 1
├── planilha/
│   ├── escalonamento.xlsx     ← importar no Google Planilhas
│   ├── amostras_monitor_serial.csv
│   ├── media_movel_serial.csv
│   └── grafico-tendencia.png
├── testes/casos-de-teste.md
└── evidencias/                ← capturas do Wokwi e da planilha
```

## Etapa 1 — Classificação dos sinais

| Elemento | Direção | Natureza | Papel |
|---|---|---|---|
| Pushbutton em D2 | Entrada | Discreta | comando/estado |
| Potenciômetro em A0 | Entrada | Analógica, convertida pelo ADC | variável simulada |
| DHT22 em D4 | Entrada | Digital codificada | temperatura/umidade |
| LED em D8 | Saída | Discreta | sinalização |
| Percentual calculado | Interno | Numérico | variável derivada (não é entrada física) |

## Tabela de E/S

| Tag | Endereço | Tipo | Faixa | Função |
|---|---|---|---|---|
| POT_BRUTO | A0 | AI | 0–1023 | adquirir variável |
| BTN_COMANDO | D2 | DI | HIGH/LOW | ler comando |
| LED_ALARME | D8 | DO | OFF/ON | sinalizar PERIGO |
| NIVEL_PCT | memória | REAL | 0–100 % | decidir e registrar |
| TEMP_C | memória | REAL | 0–100 °C | grandeza de engenharia |

## Conversão e escalonamento

- ADC do Arduino Uno: 10 bits → 2¹⁰ = 1024 códigos (0 a 1023); passo ideal q = 5,00 V / 1024 ≈ 4,88 mV.
- Percentual: `pct = 100.0 * bruto / 1023.0` (o `.0` evita divisão inteira).
- Temperatura (faixa 0–100 °C): `tempC = 0 + pct × (100 − 0)/100 = pct`.
- Equação geral usada na função `escalonar()`: `y = ymin + (x − xmin)·(ymax − ymin)/(xmax − xmin)`.
- Simulação das faixas industriais na planilha: `I = 4 + 16·p/100` (4–20 mA) e `V = 10·p/100` (0–10 V).

| Bruto | Percentual | Temperatura | Corrente | Tensão |
|---|---|---|---|---|
| 0 | 0,000 % | 0,000 °C | 4,000 mA | 0,000 V |
| 512 | 50,049 % | 50,049 °C | 12,008 mA | 5,005 V |
| 1023 | 100,000 % | 100,000 °C | 20,000 mA | 10,000 V |

512 não dá exatamente 50 % porque os códigos vão de 0 a 1023 e 512/1023 é ligeiramente maior que 0,5.

## Estados (prioridade da falha)

| Ordem | Estado | Regra |
|---|---|---|
| 1 | FALHA | percentual fora de 0–100 % |
| 2 | PERIGO | ≥ 85 % (LED_ALARME aceso) |
| 3 | ATENÇÃO | ≥ 70 % |
| 4 | NORMAL | < 70 % |

Primeiro a validade, depois a severidade decrescente, e normalidade por exclusão.

## Como reproduzir

1. **Wokwi:** abra o projeto da Semana 2 (ou crie um novo Arduino Uno e cole `diagram.json`), cole `sketch.ino` e execute. O Monitor Serial mostra `amostra,botao,bruto,pct,tempC,estado` a cada 500 ms.
2. Gire o potenciômetro devagar de 0 até o máximo e de volta, copiando pelo menos 10 linhas do Monitor Serial.
3. **Google Planilhas:** *Arquivo → Importar → Fazer upload* → `planilha/escalonamento.xlsx` → *Substituir planilha*. As fórmulas aparecem em português (SE/OU) automaticamente.
4. Na aba `aquisicao`, as colunas **A (amostra), B (bruto_ADC) e H (estado_serial)** são as linhas copiadas do Monitor Serial; as demais colunas são fórmulas. A coluna `confere` compara o estado calculado pela planilha com o estado impresso pelo Arduino (todas as 31 amostras: OK).
5. Compartilhe a planilha (*Qualquer pessoa com o link → Leitor*) e o projeto Wokwi (*Save* → *Share*) e cole os links no topo deste README.

### Abas da planilha

| Aba | Conteúdo |
|---|---|
| `aquisicao` | amostras, fórmulas de %/°C/mA/V/estado, conferência com o Monitor Serial e gráfico de tendência com os limites 70 e 85 °C |
| `fronteiras` | casos −1; 0; 69,99; 70; 84,99; 85; 100; 100,01 % |
| `calculo_inverso` | exercício adicional 2: temperatura → corrente e tensão, com verificação direta |
| `diagnostico_4-20mA` | exercício adicional 3: 3,2 / 4 / 12 / 15,2 / 17,6 / 20 / 20,8 mA |
| `media_movel` | exercício adicional 1: bruto × filtrado (N = 5) e gráfico |

Fórmula de estado (linha 2): `=SE(OU(C2<0;C2>100);"FALHA";SE(C2>=85;"PERIGO";SE(C2>=70;"ATENÇÃO";"NORMAL")))`

## Gráfico de tendência

![Tendência](planilha/grafico-tendencia.png)

**Interpretação:** amostras reais do Monitor Serial do Wokwi (execução de 16/10/2026), girando o potenciômetro de 0 até o máximo e de volta. A temperatura sobe linearmente com o potenciômetro, entra em **ATENÇÃO na amostra 26** (75,07 °C) e em **PERIGO na amostra 30** (85,04 °C), quando o LED_ALARME acende. Chega a 100 °C nas amostras 34–35 e volta a NORMAL na amostra 38 (69,99 °C, logo abaixo do limite de 70 %). Nas amostras 20–22 o botão estava pressionado (`botao=1`), sem efeito no estado, como esperado.

![LED de alarme aceso em PERIGO no Wokwi](evidencias/circuito-wokwi-perigo.png)

## Casos de teste

Tabelas completas em [`testes/casos-de-teste.md`](testes/casos-de-teste.md). Os casos A1–A4 abaixo foram obtidos no Monitor Serial do Wokwi.

| Caso | Entrada | Esperado | Obtido | Situação |
|---|---|---|---|---|
| A1 | Botão solto; pot mínimo | 0; próximo de 0 | `89,0,0,0.00,0.00,NORMAL` | Aprovado |
| A2 | Botão pressionado; pot mínimo | 1; próximo de 0 | `91,1,0,0.00,0.00,NORMAL` | Aprovado |
| A3 | Botão solto; pot meio | 0; próximo de 512 | `94,0,512,50.05,50.05,NORMAL` | Aprovado |
| A4 | Botão solto; pot máximo | 0; próximo de 1023 | `96,0,1023,100.00,100.00,PERIGO` | Aprovado |
| — | Fronteira no ADC: pot 717 | ATENÇÃO (70,09 %) | `98,0,717,70.09,70.09,ATENCAO` | Aprovado |
| F1–F8 | −1 a 100,01 % | FALHA/NORMAL/ATENÇÃO/PERIGO nas fronteiras | todos coincidem | Aprovado |

## Exercícios adicionais

1. **Filtragem (média móvel de 5 amostras):** `etapas/03-media-movel.ino` usa janela circular e soma acumulada, e divide pela quantidade real de amostras no início. Um pico isolado de 1000 afetou exatamente 5 saídas (600,8 em vez de ~500) e depois saiu da janela. O filtro **atrasa** a resposta: uma mudança real só aparece completamente após 5 amostras (2,5 s). Em um alarme, esse atraso pode ser perigoso.
2. **Cálculo inverso:** aba `calculo_inverso`. 0/25/50/75/100 °C → 4/8/12/16/20 mA e 0/2,5/5/7,5/10 V; a verificação direta `=(I−4)·100/16` recupera a temperatura inicial em todos os pontos.
3. **Diagnóstico:** aba `diagnostico_4-20mA`. Limites elétricos de falha: corrente < 4 mA ou > 20 mA. O diagnóstico é feito **antes** de calcular o estado e **sem limitar (clamp)** o valor: 3,2 mA é tratado como falha e não como "−5 %". A prioridade FALHA > PERIGO > ATENÇÃO > NORMAL garante que um instrumento não confiável nunca gere a conclusão "processo normal".

## Conclusão e limitações

Foram validados: aquisição digital e analógica, escalonamento (0, 512 e 1023), equivalência entre 4–20 mA e 0–10 V para o mesmo percentual, estados nas fronteiras, tendência e filtragem.

**O que a simulação não valida:** tolerância de resistores, ruído eletromagnético real, isolação, corrente física do laço 4–20 mA, exatidão metrológica, aterramento e comportamento de um transmissor industrial real. Os casos fora da faixa foram testados como teste de software, pois o potenciômetro do Wokwi só gera 0–1023.

## Active recall

1. **Contínuo × analógico × digital:** contínuo descreve a variável (pode assumir valores intermediários); analógico, uma representação física proporcional (tensão de 0–5 V); digital, a informação codificada em valores discretos (o código 0–1023 ou o dado do DHT22).
2. **10 bits:** 2¹⁰ = 1024 códigos (0–1023); com 5 V, passo ideal ≈ 4,88 mV.
3. **Zero vivo:** 4 mA representa 0 % com corrente circulando, o que permite distinguir o zero válido de um fio rompido (≈ 0 mA).
4. **Escalonamento direto e inverso:** direto `y = ymin + (x − xmin)(ymax − ymin)/(xmax − xmin)`; inverso, a mesma equação com os papéis trocados. Ex.: 12 mA → 50 °C e 50 °C → 12 mA.
5. **Filtragem perigosa:** quando atrasa um alarme, esconde um evento rápido real ou mascara uma falha de sensor.
