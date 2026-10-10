# Programa Ladder comentado — TP07

Programas para digitar no **PLC Simulator Online** (<https://plcsimulator.online/>). Os desenhos estão em `diagramas/`.

## Tabela de E/S e memórias

| Tag | Tipo | Função |
|---|---|---|
| START | entrada | pedido de partida (mantido enquanto pressionado) |
| STOP_OK | entrada | cadeia de parada saudável (1 = normal) |
| EMG_OK | entrada | emergência liberada (1 = normal) |
| MOTOR_DELAY | saída | motor com atraso de partida de 3 s |
| T1 | TON | atraso de partida, PT = 3 s |
| HAB | entrada | habilita o sinal intermitente |
| PISCA | memória | fase atual do pisca (0 apagada, 1 acesa) |
| T_OFF / T_ON | TON | duração das fases apagada / acesa, PT = 1 s |
| LAMPADA | saída | lâmpada intermitente |
| S_PECA | entrada | sensor de peça/recipiente na estação |
| S_PECA_ANT | memória | valor de S_PECA na varredura anterior |
| P_PECA | memória | pulso de borda de subida (1 varredura) |
| RESET_LOTE | entrada | zeragem do lote |
| C_LOTE | CTU | recipientes/peças concluídos (PV = 5 na extensão 2; PV = 6 no enchimento) |
| LOTE_OK | saída | lote completo |
| E0, E1, E2 | memórias | etapas TRANSPORTAR, ENCHER, LIBERAR |
| T0_P, T1_P, T2_P | memórias | condição de disparo de cada transição |
| INIT_OK | memória | indica que a inicialização já ocorreu |
| M_EST / Y_VALV | saídas | motor da esteira / válvula de enchimento |
| T_ENCH | TON | tempo de abertura da válvula, PT = 4 s |

## Exercício obrigatório — atraso de partida

| Rung | Lógica | Resultado |
|---|---|---|
| 1 | `EMG_OK AND STOP_OK AND START` habilita TON T1 (PT = 3 s) | ET aumenta enquanto a condição permanece 1 |
| 2 | `T1.Q` comanda `MOTOR_DELAY` | a saída liga após 3 s |
| 3 (implícito) | `NOT EMG_OK` ou `NOT STOP_OK` remove a continuidade do rung 1 | timer e saída voltam ao estado seguro |

```iecst
T1(IN := EMG_OK AND STOP_OK AND START, PT := T#3s);
MOTOR_DELAY := T1.Q;
```

## Extensão 1 — sinal intermitente (1 s ligado / 1 s desligado)

```iecst
T_OFF(IN := HAB AND NOT PISCA, PT := T#1s);   (* fase apagada *)
IF T_OFF.Q THEN PISCA := TRUE; END_IF;        (* SET *)
T_ON(IN := HAB AND PISCA, PT := T#1s);        (* fase acesa *)
IF T_ON.Q OR NOT HAB THEN PISCA := FALSE; END_IF; (* RESET *)
LAMPADA := PISCA;
```

**Como foram evitadas múltiplas inversões:** cada fase tem o próprio temporizador, habilitado só pela fase atual. Quando `T_OFF.Q` grava PISCA = 1, a condição de `T_OFF` (NOT PISCA) cai na varredura seguinte, zerando o timer; por isso a inversão acontece **uma única vez por conclusão**. Um único TON realimentado pela própria saída Q geraria pulsos de uma varredura e inversões descontroladas.

## Extensões 2 e 3 — lote com borda de subida e zeragem controlada

| Rung | Lógica |
|---|---|
| 1 | `P_PECA := S_PECA AND NOT S_PECA_ANT` (borda de subida) |
| 2 | `S_PECA_ANT := S_PECA` (atualizado **depois** do cálculo da borda) |
| 3 | CTU `C_LOTE`: CU = P_PECA, R = RESET_LOTE, PV = 5 |
| 4 | `LOTE_OK := C_LOTE.Q` (CV ≥ PV) |

**Prioridade adotada:** `RESET_LOTE` domina. Se o reset e uma nova borda chegarem na mesma varredura, CV vai para 0 (é o comportamento do CTU da IEC 61131-3: R é avaliado antes de CU). Foi usado GE (CV ≥ PV, a saída Q do CTU) e não EQ: se por algum motivo CV passar de PV, o lote continua sinalizado.

## Extensão 4 — GRAFCET do enchimento → Ladder

GRAFCET: `E0 TRANSPORTAR (M_EST=1)` —[S_PECA · EMG_OK · ¬LOTE_OK]→ `E1 ENCHER (Y_VALV=1, TON 4 s)` —[T_ENCH.Q]→ `E2 LIBERAR (M_EST=1)` —[¬S_PECA, conta 1]→ `E0`.

| GRAFCET | Ladder |
|---|---|
| etapa E0/E1/E2 | bit de memória |
| etapa inicial | rung 1: no primeiro ciclo (INIT_OK = 0) SET E0, RESET E1/E2 |
| transição | rung de condição: etapa anterior AND receptividade → bit Tn_P |
| disparo | SET da etapa seguinte e RESET da anterior (rungs 5–7) |
| ação | rung condicionado ao bit da etapa, com permissivo EMG_OK fora da memória sequencial (rungs 8–10) |
| contagem | pulso T2_P (dura uma varredura) → CTU C_LOTE, PV = 6 |

Decisões:
1. As **condições** de todas as transições são calculadas antes dos disparos (rungs 2–4, depois 5–7). Assim, uma etapa recém-ativada não dispara a transição seguinte na mesma varredura e nunca há duas etapas ativas (teste G02).
2. `EMG_OK` aparece só nas **ações**: com emergência as saídas desligam, mas a sequência não "esquece" em que etapa estava. O TON do enchimento reinicia ao voltar, garantindo 4 s completos de válvula.
3. A contagem usa o evento de saída do recipiente (T2_P), não o nível de S_PECA.
