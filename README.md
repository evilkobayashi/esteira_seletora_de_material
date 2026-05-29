# Esteira Seletora de Material

Firmware Arduino para esteira transportadora que detecta e separa materiais por tamanho usando dois sensores ultrassônicos HC-SR04 e exibe o status em LCD 16x2.

## Como funciona

Dois sensores HC-SR04 trabalham em conjunto:

- **Sensor 1 (presença)** — montado horizontalmente na entrada da zona de classificação. Detecta qualquer objeto que passe.
- **Sensor 2 (tamanho)** — montado verticalmente acima da esteira, a uma altura intermediária. Se a distância lida for menor que o limiar configurado, o objeto é "grande/alto"; caso contrário, é "pequeno/baixo".

```
[objeto entra] → [S1 detecta] → aguarda 300ms → [S2 mede altura]
                                                    │
                              GRANDE ───────────────┤ dist < DIST_GRANDE_CM
                              motor reverte → bin GRANDE
                                                    │
                              PEQUENO ──────────────┘ dist >= DIST_GRANDE_CM
                              motor segue → bin PEQUENO
```

### Estados da máquina de estados

```
AGUARDANDO ──[S1 detecta]──► CLASSIFICANDO ──[300ms]──► GRANDE ──[150ms]──► REVERTENDO ──[1800ms]──► AGUARDANDO
                                                     └──► PEQUENO ──[S1 livre]──► AGUARDANDO
```

## Hardware

| Componente | Função |
|---|---|
| Arduino Uno | Controlador principal |
| HC-SR04 #1 | Sensor de presença (entrada da zona de classificação) |
| HC-SR04 #2 | Sensor de tamanho (mede altura do objeto) |
| L298N | Ponte H — controle bidirecional do motor |
| LCD 16x2 | Exibe estado atual e contagem por categoria |
| Motor DC c/ caixa de redução | Movimenta a esteira |

## Pinagem

| Componente | Sinal | Pino Arduino |
|---|---|---|
| L298N | IN1 | D4 |
| L298N | IN2 | D5 |
| L298N | ENA (PWM) | **D3** |
| HC-SR04 #1 | TRIG | D6 |
| HC-SR04 #1 | ECHO | D7 |
| HC-SR04 #2 | TRIG | D10 |
| HC-SR04 #2 | ECHO | D11 |
| LCD 16x2 + módulo I2C | SDA | A4 |
| LCD 16x2 + módulo I2C | SCL | A5 |

> **L298N:** remover o jumper ENA e conectar ao **D3** para controle PWM. A fonte do motor deve fornecer a tensão nominal do motor + 2 V (queda interna da ponte H).

> **HC-SR04:** alimentar com 5 V do Arduino. O pino ECHO já é compatível com os 5 V lógicos do Uno.

> **LCD I2C:** o endereço padrão do módulo PCF8574 é `0x27`. Se o display não iniciar, tente `0x3F` em `config.h`. Você pode confirmar o endereço rodando um sketch de I2C scanner.

## Alimentação

| Componente | Fonte |
|---|---|
| Arduino Uno | USB ou fonte 7–12 V no jack |
| Lógica L298N (pino 5 V) | 5 V do Arduino |
| Motor | Fonte externa via terminal VCC do L298N |
| Sensores HC-SR04 | 5 V do Arduino |
| LCD 16x2 | 5 V do Arduino |

## Estrutura do projeto

```
esteira_seletora/
├── esteira_seletora.ino    ← sketch principal, máquina de estados
├── config.h                ← pinos, thresholds e constantes de timing
├── sensor.h / sensor.cpp   ← módulo HC-SR04 (presença e tamanho)
├── motor.h / motor.cpp     ← módulo L298N
└── display.h / display.cpp ← módulo LCD 16x2
```

## Bibliotecas necessárias

Instalar via Arduino IDE → Tools → Manage Libraries:

- **LiquidCrystal I2C** by Frank de Brabander (buscar `LiquidCrystal I2C`)

## Configuração

Todas as constantes relevantes estão em `config.h`:

```cpp
#define DIST_PRESENCA_CM    15   // sensor 1: objeto presente se dist < este valor (cm)
#define DIST_GRANDE_CM       8   // sensor 2: objeto "grande" se dist < este valor (cm)

#define TEMPO_CLASSIF_MS   300   // aguarda objeto estabilizar antes de medir (ms)
#define TEMPO_PARADA_MS    150   // pausa antes de reverter o motor (ms)
#define TEMPO_REVERSO_MS  1800   // duração do reverso para desviar o objeto (ms)

#define VELOCIDADE_MOTOR   180   // velocidade PWM do motor (0–255)
#define DEBOUNCE_LEITURAS    3   // leituras consecutivas para confirmar presença
```

Ajuste `DIST_GRANDE_CM` conforme o tamanho de corte desejado para sua aplicação.

## Como testar

1. Abrir `esteira_seletora/esteira_seletora.ino` no Arduino IDE.
2. Selecionar placa **Arduino Uno** e a porta correta.
3. Fazer upload.
4. Abrir o Serial Monitor (9600 baud) e verificar `Sistema iniciado.`
5. Passar a mão a menos de 15 cm do **Sensor 1** → LCD exibe `Objeto detectado`.
6. Manter a mão abaixo do limiar do **Sensor 2** → LCD exibe `GRANDE! Desviando` e o motor reverte.
7. Passar objeto pequeno (acima do limiar do Sensor 2) → LCD exibe `Pequeno. OK.` e o motor continua.
8. Ajustar `DIST_GRANDE_CM` e `DIST_PRESENCA_CM` em `config.h` para o tamanho de corte adequado.

## Display LCD 16x2

Durante a classificação:
```
┌────────────────┐
│Objeto detectado│
│Classificando...│
└────────────────┘
```

Após classificar (resultado + contagem):
```
┌────────────────┐
│Grd:3   Peq:7   │
│Total:10        │
└────────────────┘
```

---

## Aplicação: triagem de resíduos de construção civil

### Problema

Obras de construção e reforma geram grandes volumes de entulho misto — fragmentos de concreto, tijolos e cerâmica de tamanhos variados. O reaproveitamento desses resíduos (base compactada, agregado reciclado) exige que o material seja separado por granulometria antes do processamento. Hoje essa triagem é feita manualmente, expondo trabalhadores a poeira, peso e risco de cortes.

### Como a esteira resolve

A esteira seletora automatiza a primeira etapa de triagem por tamanho:

```
Entulho bruto →  [Esteira Seletora]  →  Fragmentos grandes (> limiar)
                                      →  Fragmentos pequenos (≤ limiar)
```

- **Bin GRANDE:** blocos e lajes quebradas → britagem secundária ou reuso como rachão.
- **Bin PEQUENO:** pó e cacos finos → peneiramento final para produção de agregado miúdo reciclado.

### Integração com o processo

```
Caminhão de entulho
        │
        ▼
  Triturador primário
        │
        ▼
  ┌─────────────┐
  │   ESTEIRA   │  ← firmware deste projeto
  │  SELETORA   │
  └──────┬──────┘
         │
    ┌────┴────┐
    │         │
    ▼         ▼
 Bin Grande  Bin Pequeno
 (rachão)   (agregado fino)
    │         │
    ▼         ▼
 Revenda   Peneira vibratória
 ou aterro  → pó de pedra reciclado
```

### Parâmetros sugeridos para esta aplicação

| Parâmetro | Valor sugerido | Justificativa |
|---|---|---|
| `DIST_GRANDE_CM` | 5 | fragmentos > ~5 cm de altura são "grandes" |
| `DIST_PRESENCA_CM` | 20 | detecta peças na largura total da esteira |
| `TEMPO_CLASSIF_MS` | 400 | material pesado oscila mais, precisa de mais tempo |
| `TEMPO_REVERSO_MS` | 2200 | peças maiores precisam de mais tempo para sair da zona |
| `VELOCIDADE_MOTOR` | 140 | velocidade reduzida para carga pesada |

### Benefícios esperados

- Redução de 60–80% no tempo de triagem manual.
- Menor exposição de trabalhadores a poeira e material cortante.
- Classificação contínua sem intervenção humana na esteira.
- Contagem automática de itens por categoria exibida no LCD para controle de produção.
