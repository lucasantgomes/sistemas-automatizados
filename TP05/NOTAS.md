# NOTAS — limites, hipóteses e melhorias (TP05)

## Hipóteses adotadas

- Sensores fornecem sinais binários limpos (sem ruído nem repique). Na implementação real seria preciso filtrar/temporizar entradas.
- Os tempos (8/10/3 s, 5 s, 30 s, 0,5 s) são requisitos do exercício, não valores normativos de trânsito ou de portas.
- Porta: em E99 o motor é desligado (movimento interrompido), conforme o gabarito. Em uma porta de rota de fuga real, o estado seguro pode ser "abrir e manter aberta"; isso depende da análise de risco do edifício.
- Esteira: o retorno dos atuadores é por mola ao desenergizar o solenoide (DESVIO_x = 0).
- Reservatório: NIVEL_BAIXO = 1 significa nível no ou abaixo do sensor baixo; NIVEL_ALTO = 1, nível no ou acima do sensor alto.

## Limites do modelo

- O diagrams.net é apenas editor gráfico: não executa as regras do GRAFCET. A validação foi feita por percurso das regras de evolução, registrado em `casos_teste.csv` e `percurso_semaforo.csv`.
- GRAFCET especifica o comportamento; não define como o CLP implementa temporizadores, prioridades de varredura ou memórias (isso será tratado na conversão para Ladder/SFC, Semana 7).
- Não há tempo-limite (watchdog) para movimentos: se FC_ABERTA ou FC_x_AV nunca chegar, o modelo fica parado na etapa sem sinalizar.
- Emergência modelada como lógica funcional. Não substitui uma função de segurança certificada (ISO 13849 / IEC 62061).

## Melhorias possíveis

1. Acrescentar temporização de supervisão em cada movimento (ex.: `10 s/X1 · ¬FC_ABERTA` → etapa de falha "motor não responde").
2. Na esteira, contar peças A e B e acrescentar etapa de "esteira cheia".
3. No semáforo, modo noturno (amarelo intermitente) selecionado por chave, como seleção a partir de E0.
4. No reservatório, verificar coerência durante E1 (NIVEL_BAIXO e NIVEL_ALTO juntos durante o enchimento → falha).
