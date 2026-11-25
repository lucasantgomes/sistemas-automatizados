// Gerador do programa Ladder "Estacionamento Inteligente" para o PLC Simulator Online
// (app.plcsimulator.online). Monta o estado interno do simulador (runglist, rungs,
// branches, elements, variables) a partir de uma descrição simples dos rungs.
// Uso:
//   1. Abra https://app.plcsimulator.online no Chrome.
//   2. Pressione F12 e vá na aba "Console".
//   3. Cole TODO este arquivo e pressione Enter (se o Chrome pedir, digite "allow pasting").
//   4. A página recarrega já com o programa do estacionamento.
// ATENÇÃO: substitui o projeto que estiver aberto no simulador.

function gerarPrograma() {
  const CAPACIDADE = 5;        // nº de vagas (preset do contador)
  const T_FECHA_MS = 3000;     // tempo para fechar a cancela após o carro passar
  const T_DESIST_MS = 5000;    // tempo para fechar se o carro desistir

  const uuid = () => crypto.randomUUID();
  const variables = {}, elements = {}, rungs = {}, branches = {}, runglist = [];
  const byName = {};

  const mkVar = (name, type, value, parent = '') => {
    const id = uuid();
    variables[id] = { name, type, value, usedBy: [], subVars: {}, parrent: parent };
    byName[name] = id;
    return id;
  };
  const bool = n => mkVar(n, 'Bool', false);
  const timer = (n, pt) => {
    const id = mkVar(n, 'Timer', null);
    const s = variables[id].subVars;
    s.IN = mkVar(n + '.IN', 'Bool', false, id);
    s.PT = mkVar(n + '.PT', 'Time', pt, id);
    s.Q = mkVar(n + '.Q', 'Bool', false, id);
    s.ET = mkVar(n + '.ET', 'Time', 0, id);
    s.R = mkVar(n + '.R', 'Bool', false, id);
  };
  const counter = (n, pv) => {
    const id = mkVar(n, 'Counter', null);
    const s = variables[id].subVars;
    s.CU = mkVar(n + '.CU', 'Bool', false, id);
    s.CD = mkVar(n + '.CD', 'Bool', false, id);
    s.R = mkVar(n + '.R', 'Bool', false, id);
    s.LD = mkVar(n + '.LD', 'Bool', false, id);
    s.PV = mkVar(n + '.PV', 'Number', pv, id);
    s.QU = mkVar(n + '.QU', 'Bool', false, id);
    s.QD = mkVar(n + '.QD', 'Bool', false, id);
    s.CV = mkVar(n + '.CV', 'Number', 0, id);
  };

  // ---------- Tabela de variáveis (ordem de exibição) ----------
  // Entradas
  ['S_ENT_PRES', 'S_ENT_PASS', 'S_SAI_PRES', 'S_SAI_PASS',
   'B_LIGA', 'B_DESLIGA', 'B_EMERG', 'B_RESET'].forEach(bool);
  // Saídas
  ['CANC_ENT', 'CANC_SAI', 'LUZ_LIVRE', 'LUZ_LOTADO', 'ALARME'].forEach(bool);
  // Memórias internas
  ['M_SIS', 'LOTADO', 'M_ENT_PASSOU', 'M_SAI_PASSOU'].forEach(bool);
  mkVar('VAGAS_LIVRES', 'Number', CAPACIDADE);
  // Contador e temporizadores
  counter('C_VAGAS', CAPACIDADE);
  timer('T_FECHA_ENT', T_FECHA_MS);
  timer('T_DESIST_ENT', T_DESIST_MS);
  timer('T_FECHA_SAI', T_FECHA_MS);
  timer('T_DESIST_SAI', T_DESIST_MS);

  // ---------- Construção de elementos ----------
  const use = (varName, elId) => {
    const v = byName[varName];
    if (!v) throw new Error('Variável inexistente: ' + varName);
    variables[v].usedBy.push(elId);
    return { uuid: v };
  };
  const mkElement = item => {
    const [type, ...args] = item;
    const id = uuid();
    let el;
    if (['XIC', 'XIO'].includes(type)) {
      el = { type, parameters: { input: [{ ...use(args[0], id), type: ['Bool'] }] }, memInput: false };
    } else if (['OTE', 'OTL', 'OTU'].includes(type)) {
      el = { type, parameters: { output: [{ ...use(args[0], id), type: ['Bool'] }] } };
    } else if (['TON', 'TOF'].includes(type)) {
      el = { type, parameters: { inOut: [{ ...use(args[0], id), type: ['Timer'] }] } };
    } else if (['CTU', 'CTD'].includes(type)) {
      el = { type, parameters: { inOut: [{ ...use(args[0], id), type: ['Counter'] }] },
             prevCU: false, prevCD: false, memInput: false };
    } else if (['ADD', 'SUB', 'MUL', 'DIV'].includes(type)) {
      el = { type, parameters: {
        input: [{ ...use(args[0], id), type: ['Number'] }, { ...use(args[1], id), type: ['Number'] }],
        output: [{ ...use(args[2], id), type: ['Number'] }] } };
    } else throw new Error('Tipo não suportado: ' + type);
    elements[id] = { ...el, configured: true, out: false };
    return id;
  };
  const mkSeries = (items, comment = '') => {
    const id = uuid();
    const els = items.map(it => {
      if (it[0] === 'BR') {
        const bid = uuid();
        branches[bid] = { rungs: it.slice(1).map(sub => mkSeries(sub)), input: false, out: false };
        return bid;
      }
      return mkElement(it);
    });
    rungs[id] = { comment, elements: els, input: false, out: false };
    return id;
  };
  const rung = (comment, ...items) => runglist.push(mkSeries(items, comment));

  // ---------- Programa Ladder ----------
  rung('1 Liga/desliga do sistema com selo (autorretenção). Desliga ou emergência derrubam M_SIS',
    ['BR', [['XIC', 'B_LIGA']], [['XIC', 'M_SIS']]], ['XIO', 'B_DESLIGA'], ['XIO', 'B_EMERG'], ['OTE', 'M_SIS']);
  rung('2 Alarme de emergência',
    ['XIC', 'B_EMERG'], ['OTE', 'ALARME']);
  rung('3 Reset manual da contagem de vagas (zera C_VAGAS)',
    ['XIC', 'B_RESET'], ['OTE', 'C_VAGAS.R']);
  rung('4 Contagem de ENTRADA: borda de subida de S_ENT_PASS com cancela aberta soma 1 (CTU)',
    ['XIC', 'S_ENT_PASS'], ['XIC', 'CANC_ENT'], ['CTU', 'C_VAGAS']);
  rung('5 Contagem de SAÍDA: borda de subida de S_SAI_PASS com cancela aberta subtrai 1 (CTD), nunca abaixo de 0',
    ['XIC', 'S_SAI_PASS'], ['XIC', 'CANC_SAI'], ['XIO', 'C_VAGAS.QD'], ['CTD', 'C_VAGAS']);
  rung('6 LOTADO quando ocupação (CV) atinge a capacidade (PV)',
    ['XIC', 'C_VAGAS.QU'], ['OTE', 'LOTADO']);
  rung('7 Vagas livres = capacidade - ocupação (para o painel)',
    ['SUB', 'C_VAGAS.PV', 'C_VAGAS.CV', 'VAGAS_LIVRES']);

  rung('8 Abre a CANCELA DE ENTRADA: carro presente e NÃO lotado; selo até fechamento ou desistência',
    ['BR', [['XIC', 'S_ENT_PRES'], ['XIO', 'LOTADO']], [['XIC', 'CANC_ENT']]],
    ['XIC', 'M_SIS'], ['XIO', 'T_FECHA_ENT.Q'], ['XIO', 'T_DESIST_ENT.Q'], ['OTE', 'CANC_ENT']);
  rung('9 Memoriza que o carro passou pela cancela de entrada',
    ['XIC', 'CANC_ENT'], ['XIC', 'S_ENT_PASS'], ['OTL', 'M_ENT_PASSOU']);
  rung('10 Fechamento da entrada: 3 s após o carro liberar o sensor de passagem (não fecha sobre o carro)',
    ['XIC', 'CANC_ENT'], ['XIC', 'M_ENT_PASSOU'], ['XIO', 'S_ENT_PASS'], ['TON', 'T_FECHA_ENT']);
  rung('11 Desistência na entrada: cancela aberta, carro saiu sem passar -> fecha após 5 s',
    ['XIC', 'CANC_ENT'], ['XIO', 'M_ENT_PASSOU'], ['XIO', 'S_ENT_PRES'], ['XIO', 'S_ENT_PASS'], ['TON', 'T_DESIST_ENT']);
  rung('12 Cancela de entrada fechada: limpa a memória de passagem',
    ['XIO', 'CANC_ENT'], ['OTU', 'M_ENT_PASSOU']);

  rung('13 Abre a CANCELA DE SAÍDA: carro presente (saída sempre permitida); selo até fechamento ou desistência',
    ['BR', [['XIC', 'S_SAI_PRES']], [['XIC', 'CANC_SAI']]],
    ['XIC', 'M_SIS'], ['XIO', 'T_FECHA_SAI.Q'], ['XIO', 'T_DESIST_SAI.Q'], ['OTE', 'CANC_SAI']);
  rung('14 Memoriza que o carro passou pela cancela de saída',
    ['XIC', 'CANC_SAI'], ['XIC', 'S_SAI_PASS'], ['OTL', 'M_SAI_PASSOU']);
  rung('15 Fechamento da saída: 3 s após o carro liberar o sensor de passagem',
    ['XIC', 'CANC_SAI'], ['XIC', 'M_SAI_PASSOU'], ['XIO', 'S_SAI_PASS'], ['TON', 'T_FECHA_SAI']);
  rung('16 Desistência na saída: fecha após 5 s se o carro sair sem passar',
    ['XIC', 'CANC_SAI'], ['XIO', 'M_SAI_PASSOU'], ['XIO', 'S_SAI_PRES'], ['XIO', 'S_SAI_PASS'], ['TON', 'T_DESIST_SAI']);
  rung('17 Cancela de saída fechada: limpa a memória de passagem',
    ['XIO', 'CANC_SAI'], ['OTU', 'M_SAI_PASSOU']);

  rung('18 Painel verde VAGAS DISPONÍVEIS',
    ['XIC', 'M_SIS'], ['XIO', 'LOTADO'], ['OTE', 'LUZ_LIVRE']);
  rung('19 Painel vermelho LOTADO',
    ['XIC', 'M_SIS'], ['XIC', 'LOTADO'], ['OTE', 'LUZ_LOTADO']);

  return { runglist, rungs, branches, elements, variables, misc: { displayTab: 'diagram' } };
}

async function carregarPrograma() {
  const prog = gerarPrograma();
  const root = {};
  for (const [k, v] of Object.entries(prog)) root[k] = JSON.stringify(v);
  root._persist = JSON.stringify({ version: 3, rehydrated: true });
  if (location.hostname !== 'app.plcsimulator.online') {
    throw new Error('Abra https://app.plcsimulator.online e cole o script no Console dessa página.');
  }
  const db = await new Promise(r => { const q = indexedDB.open('localforage'); q.onsuccess = () => r(q.result); });
  if (!db.objectStoreNames.contains('keyvaluepairs')) {
    db.close();
    throw new Error('O simulador ainda não terminou de carregar. Aperte F5, espere o diagrama aparecer e cole o script de novo.');
  }
  await new Promise((res, rej) => {
    const tx = db.transaction('keyvaluepairs', 'readwrite');
    tx.objectStore('keyvaluepairs').put(JSON.stringify(root), 'persist:root');
    tx.oncomplete = res; tx.onerror = rej;
  });
  setTimeout(() => location.reload(), 200);
  return 'ok: ' + prog.runglist.length + ' rungs, ' + Object.keys(prog.variables).length + ' variáveis';
}

carregarPrograma()
  .then(msg => console.log('✅ Programa do estacionamento carregado (' + msg + '). A página vai recarregar.'))
  .catch(e => console.error('❌ ' + e.message));
