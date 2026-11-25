# Resultados dos testes de simulação

Simulador: PLC Simulator Online (https://app.plcsimulator.online), executado em 24/11/2026.
Capacidade configurada: 5 vagas (C_VAGAS.PV = 5). T_FECHA = 3000 ms, T_DESIST = 5000 ms.
Os tempos foram medidos em tempo real durante a simulação (ciclo de varredura do simulador ≈ 66 ms).

| ID | Cenário | Resultado | Observação |
|---|---|---|---|
| T00 | Estado inicial | OK | M_SIS ligado, vagas livres 5 |
| T01 | Desliga e religa o sistema (selo) | OK | após DESLIGA, M_SIS = False; após LIGA, M_SIS = True e LUZ_LIVRE = True |
| T02 | Entrada normal | OK | cancela abriu; vagas livres 5 → 4; fechou 3101 ms após o carro liberar o sensor (programado 3000 ms) |
| T03 | Segurança: carro parado 6 s sob a cancela | OK | cancela permaneceu aberta durante os 6 s; fechou 3111 ms após liberar; vagas livres 3 |
| T04 | Lotação: 5 carros dentro | OK | CV = 5, vagas livres 0, LOTADO = True, LUZ_LOTADO = True, LUZ_LIVRE = False |
| T05 | 6º carro com estacionamento lotado | OK | carro aguardando 2 s, CANC_ENT = False (entrada bloqueada) |
| T06 | Saída com carro aguardando na entrada | OK | saída abriu; vagas livres 0 → 1; LOTADO = False; a cancela de entrada abriu automaticamente para o carro em espera; saída fechou 3108 ms após liberar |
| T07 | Desistência: carro vai embora sem entrar | OK | cancela fechou 5089 ms depois (programado 5000 ms); contagem inalterada |
| T08 | Reset da contagem e saída com contador em zero | OK | após RESET vagas livres 5; saída extra não deixa a contagem negativa |
| T09 | Emergência acionada com cancela aberta | OK | ALARME = True, M_SIS = False, CANC_ENT = False, LUZ_LIVRE = False; ao soltar a emergência o sistema continua desligado |
| T10 | Religamento após emergência | OK | M_SIS = True, LUZ_LIVRE = True |

Resultado: 11 de 11 cenários aprovados.
