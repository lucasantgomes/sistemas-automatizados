# Sistema Automatizado de Estacionamento Inteligente

Projeto da avaliação **A3 – Sistemas Automatizados** (Engenharia de Software, Universidade São Judas Tadeu, 2026/2, Prof. Robson Calvetti). Tema **6.2 – Estacionamento Inteligente**.

O sistema faz o controle de acesso de um estacionamento com CLP, programado em linguagem **Ladder** no [PLC Simulator Online](https://app.plcsimulator.online). Ele:

- controla a **entrada e a saída** de veículos com duas cancelas automáticas;
- **conta as vagas** disponíveis usando um contador CTU/CTD;
- **bloqueia a entrada** e acende o painel LOTADO quando o estacionamento está cheio.

## Integrantes

| Nome | RA |
|---|---|
| Denner Ferreira | 823127322 |
| Gustavo Modesto | 823144057 |
| Kauã Almansa do Amaral | 824148531 |
| Leonardo Sales | 82417341 |
| Lucas Antonio Gomes | 823126987 |
| Murilo Amorim Castro | 823135117 |

## Estrutura do repositório

```
├── relatorio/
│   ├── Relatorio_Estacionamento_Inteligente.pdf    ← relatório técnico (ABNT, entrega oficial)
│   └── Relatorio_Estacionamento_Inteligente.docx   ← versão editável
├── apresentacao/
│   └── Apresentacao_Estacionamento_Inteligente.pptx
├── logica/
│   ├── gerar_programa.js        ← carrega a lógica Ladder completa no simulador
│   └── resultados_testes.md     ← os 11 cenários testados e os resultados
└── imagens/                     ← diagramas e capturas da simulação
```

## Como abrir a lógica no simulador

O PLC Simulator Online guarda o projeto só no navegador. Para abrir a lógica em outro computador:

1. Abra **https://app.plcsimulator.online** no Chrome ou no Edge e espere o editor carregar. Se aparecer a página "PLC Simulator Online is moving to Rungs Studio", clique em **Keep using PLC Simulator Online**.
2. Em outra aba, abra o script:
   https://raw.githubusercontent.com/lucasantgomes/sistemas-automatizados/main/A3-estacionamento-inteligente/logica/gerar_programa.js
   Depois aperte **Ctrl + A** e **Ctrl + C**.
3. Volte para a aba do simulador e aperte **Ctrl + Shift + J**. O Console abre (se não abrir, aperte F12 e clique na aba **Console**).
4. Clique na última linha do Console, aperte **Ctrl + V** e depois **Enter**.
   - Na primeira vez, o Chrome bloqueia a colagem. Digite `permitir colar` (no Chrome em inglês, `allow pasting`), aperte Enter, cole de novo e aperte Enter.
5. Deve aparecer **✅ Programa do estacionamento carregado**, e a página recarrega sozinha.
6. Confira se deu certo: a lista de variáveis começa com `S_ENT_PRES` e `VAGAS_LIVRES` mostra 5. Feche o Console (Ctrl + Shift + J).

Se aparecer **❌ O simulador ainda não terminou de carregar**, aperte F5, espere o editor aparecer e repita os passos 3 e 4.

> ⚠️ O script **substitui** o projeto que estiver aberto no simulador. Depois de carregado, o programa fica salvo naquele navegador. Não clique em "novo projeto".

## Mapa de entradas e saídas

| Endereço | Tag | Descrição |
|---|---|---|
| I0.0 | S_ENT_PRES | Sensor de presença antes da cancela de entrada |
| I0.1 | S_ENT_PASS | Sensor de passagem depois da cancela de entrada |
| I0.2 | S_SAI_PRES | Sensor de presença antes da cancela de saída |
| I0.3 | S_SAI_PASS | Sensor de passagem depois da cancela de saída |
| I0.4 | B_LIGA | Liga o sistema |
| I0.5 | B_DESLIGA | Desliga o sistema |
| I0.6 | B_EMERG | Emergência |
| I0.7 | B_RESET | Zera a contagem |
| Q0.0 | CANC_ENT | Cancela de entrada |
| Q0.1 | CANC_SAI | Cancela de saída |
| Q0.2 | LUZ_LIVRE | Painel verde "VAGAS DISPONÍVEIS" |
| Q0.3 | LUZ_LOTADO | Painel vermelho "LOTADO" |
| Q0.4 | ALARME | Alarme de emergência |

Componentes internos:
- **Contador** `C_VAGAS`: PV = 5 vagas.
- **Temporizadores TON**:
  - `T_FECHA_ENT` e `T_FECHA_SAI` (3 s): fecham a cancela depois que o carro passa;
  - `T_DESIST_ENT` e `T_DESIST_SAI` (5 s): fecham a cancela se o motorista desistir.
- **Bloco SUB**: calcula `VAGAS_LIVRES`.

## Roteiro da demonstração ao vivo

No simulador, aperte ▶ (Run). Para acionar um sensor ou botão, clique no valor (True/False) da variável.

1. **Ligar o sistema:** `B_LIGA` → True e depois False. Resultado: `M_SIS` e `LUZ_LIVRE` acendem.
2. **Entrada de um carro:**
   - `S_ENT_PRES` → True: a cancela abre.
   - `S_ENT_PRES` → False e `S_ENT_PASS` → True: `VAGAS_LIVRES` cai para 4.
   - `S_ENT_PASS` → False: a cancela fecha em 3 s.
3. **Lotação:** repita o passo 2 até ter 5 carros. Resultado: `LOTADO` e `LUZ_LOTADO` acendem.
4. **Bloqueio:** `S_ENT_PRES` → True com o estacionamento lotado. Resultado: a cancela **não** abre.
5. **Saída liberando a fila:** com o 6º carro ainda esperando na entrada:
   - `S_SAI_PRES` → True e depois False;
   - `S_SAI_PASS` → True e depois False.

   Resultado: as vagas livres passam para 1 e a cancela de entrada abre sozinha.
6. **Emergência:** `B_EMERG` → True. Resultado: o alarme dispara e o sistema desliga. Ao soltar o botão, o sistema só volta a funcionar com um novo `B_LIGA`.

## Resultados

Os **11 cenários de teste foram aprovados**. A lista completa está em [`logica/resultados_testes.md`](logica/resultados_testes.md). Tempos medidos:
- fechamento: **3,10 s** (programado 3,00 s);
- desistência: **5,09 s** (programado 5,00 s).

A diferença vem do ciclo de varredura do simulador, que é de aproximadamente 66 ms.
