# Programa Ladder — TP06

Programa para digitar no **PLC Simulator Online** (<https://plcsimulator.online/>). Os desenhos estão em `diagramas/`.

## Tabela de E/S

| Tag | Tipo | Significado | Convenção |
|---|---|---|---|
| START | entrada (BOOL) | botão de partida, momentâneo | 1 quando pressionado |
| STOP_OK | entrada (BOOL) | cadeia de parada saudável | 1 em condição normal; 0 = parada pedida |
| EMG_OK | entrada (BOOL) | emergência liberada | 1 em condição normal; 0 = emergência |
| OL_OK | entrada (BOOL) | relé de sobrecarga saudável | 1 sem disparo; 0 = sobrecarga |
| MOTOR | saída (BOOL) | comando do contator | 1 = comandado |
| RUN | saída (BOOL) | indicação de comando | 1 quando MOTOR = 1 |
| READY | saída (BOOL) | permissivos saudáveis | 1 quando EMG_OK, OL_OK e STOP_OK = 1 |
| START_F / START_R | entrada (BOOL) | partida avante / reversa | 1 quando pressionado |
| FWD / REV | saída (BOOL) | contator avante / reverso | 1 = comandado |

Contato lógico negado e dispositivo físico NF **não** são sinônimos: `STOP_OK` já tem o significado "cadeia saudável", por isso entra como contato **NA** no programa.

## Exercício obrigatório (produto mínimo)

```
      EMG_OK    STOP_OK       START                 MOTOR
 |-----| |--------| |-----+-----| |-----+-------------( )-----|
 |                        |             |                     |
 |                        |     MOTOR   |                     |
 |                        +-----| |-----+                     |
```

`MOTOR = EMG_OK AND STOP_OK AND (START OR MOTOR)`

## Programa completo (com OL_OK, RUN e READY)

```
 Rung 1
      EMG_OK     OL_OK    STOP_OK       START                MOTOR
 |-----| |--------| |--------| |-----+-----| |-----+-----------( )----|
 |                                   |     MOTOR   |                  |
 |                                   +-----| |-----+                  |
 Rung 2
      MOTOR                                                    RUN
 |-----| |---------------------------------------------------( )----|
 Rung 3
      EMG_OK     OL_OK    STOP_OK                              READY
 |-----| |--------| |--------| |-----------------------------( )----|
```

Equivalente em texto estruturado (IEC 61131-3), útil para conferência:

```iecst
MOTOR := EMG_OK AND OL_OK AND STOP_OK AND (START OR MOTOR);
RUN   := MOTOR;
READY := EMG_OK AND OL_OK AND STOP_OK;
```

## Exercício adicional 2 — reversão intertravada

```
 Rung 1
      EMG_OK    STOP_OK     REV         START_F               FWD
 |-----| |--------| |-------|/|-----+-----| |-----+-----------( )----|
 |                                  |      FWD    |                  |
 |                                  +-----| |-----+                  |
 Rung 2
      EMG_OK    STOP_OK     FWD         START_R               REV
 |-----| |--------| |-------|/|-----+-----| |-----+-----------( )----|
 |                                  |      REV    |                  |
 |                                  +-----| |-----+                  |
```

```iecst
FWD := EMG_OK AND STOP_OK AND NOT REV AND (START_F OR FWD);
REV := EMG_OK AND STOP_OK AND NOT FWD AND (START_R OR REV);
```

Política para START_F e START_R simultâneos a partir do repouso: o rung de FWD é executado primeiro na varredura, então FWD liga e, no rung seguinte, `NOT FWD` bloqueia REV. Resultado: **FWD tem prioridade**, e as duas saídas nunca ficam em 1 ao mesmo tempo.

## Passo a passo no PLC Simulator Online

1. Abra <https://plcsimulator.online/> e crie um projeto em branco.
2. Cadastre as tags da tabela acima com esses nomes exatos.
3. **Rung 1:** insira os contatos NA `EMG_OK`, `OL_OK` e `STOP_OK` em série; depois o contato NA `START`; crie um **ramo paralelo** sob `START` com o contato NA `MOTOR`; feche o ramo antes da bobina `MOTOR`. A parada e os permissivos ficam **antes** da divergência.
4. **Rung 2:** contato NA `MOTOR` → bobina `RUN`.
5. **Rung 3:** contatos NA `EMG_OK`, `OL_OK`, `STOP_OK` em série → bobina `READY`.
6. Revise e entre no modo de simulação. Coloque EMG_OK, OL_OK e STOP_OK em 1 antes de testar.
7. Execute os casos de `casos_teste.csv` na ordem, marcando a coluna `conferido_plc_simulator`.
8. Salve/compartilhe o projeto, teste o link em janela anônima e cole-o no README.

**Contingência:** se o simulador estiver fora do ar, use o experimento DOL Starter do Virtual Labs (<https://plc-coep.vlabs.ac.in/exp/dol-starter/>) para START, retenção e STOP. Ele é um roteiro dirigido e não substitui um editor Ladder livre (não permite READY, RUN nem reversão).
