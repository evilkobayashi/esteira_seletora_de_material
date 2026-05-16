# Esteira Seletora de Material — Design Spec

**Data:** 2026-05-16  
**Hardware:** Arduino Uno, OLED SSD1306, L298N, KY-036, Motor DC c/ caixa de redução plástica

---

## Objetivo

Esteira transportadora que detecta materiais metálicos via sensor KY-036. Metal → motor reverte → item vai ao bin metal. Não-metal → motor segue forward → item cai no bin não-metal. OLED exibe status atual e contagem de cada tipo.

---

## Arquitetura — Máquina de Estados

```
DETECTANDO ──[D0 HIGH + debounce]──► METAL_DETECTADO ──[200ms]──► REVERTENDO ──[1500ms]──► DETECTANDO
     ▲                                                                                            │
     └────────────────────────────────────────────────────────────────────────────────────────────┘
```

### Estados

| Estado | Motor | OLED | Ação |
|---|---|---|---|
| DETECTANDO | Forward (PWM) | "DETECTANDO" | Poll KY-036 D0 a cada 50ms |
| METAL_DETECTADO | Stop | "METAL!" | Incrementa contador metal, atualiza OLED |
| REVERTENDO | Reverse (PWM) | "REVERTENDO" | Aguarda TEMPO_REVERSO_MS |

### Debounce
3 leituras consecutivas D0=HIGH em intervalos de 50ms (150ms total) antes de transicionar para METAL_DETECTADO. Evita falso trigger por vibração.

---

## Pinagem

| Componente | Pino Arduino |
|---|---|
| OLED SDA | A4 |
| OLED SCL | A5 |
| L298N IN1 | D4 |
| L298N IN2 | D5 |
| L298N ENA (PWM) | D9 |
| KY-036 D0 (digital) | D2 |
| KY-036 A0 (analógico) | A0 |

---

## Alimentação

- Arduino: USB (5V)
- Motor: fonte externa → L298N VCC motor
- L298N lógica: 5V do Arduino → pino 5V do L298N
- **Atenção:** L298N queda interna ~2V. Fonte motor = tensão nominal motor + 2V mínimo.
- Jumper ENA: remover, conectar D9 para controle PWM de velocidade.

---

## Estrutura de Arquivos

```
esteira_seletora/
├── esteira_seletora.ino    ← setup(), loop(), state machine
├── config.h                ← constantes de pinos e timing
├── motor.h / motor.cpp     ← motorForward(), motorReverse(), motorStop()
├── display.h / display.cpp ← displayInit(), displayStatus(), displayCounts()
└── sensor.h / sensor.cpp   ← sensorReadDigital(), sensorReadAnalog()
```

---

## Constantes (config.h)

```cpp
#define PIN_MOTOR_IN1        4
#define PIN_MOTOR_IN2        5
#define PIN_MOTOR_ENA        9
#define PIN_SENSOR_DIGITAL   2
#define PIN_SENSOR_ANALOG    A0
#define OLED_ADDRESS         0x3C
#define OLED_WIDTH           128
#define OLED_HEIGHT          64

#define TEMPO_PARADA_MS       200
#define TEMPO_REVERSO_MS     1500
#define INTERVALO_POLLING_MS   50
#define DEBOUNCE_LEITURAS       3
#define VELOCIDADE_MOTOR      180   // 0-255
```

---

## OLED Layout (128×64)

```
┌──────────────────────────┐
│   ESTEIRA SELETORA       │
│                          │
│  Status: DETECTANDO      │
│                          │
│  Metal:       005        │
│  Nao-metal:   ---        │
└──────────────────────────┘
```

Status exibido: `DETECTANDO` / `METAL!` / `REVERTENDO`

> **Nota:** Contador não-metal exibe `---` — KY-036 não detecta passagem de itens não-metálicos. Contagem futura requer botão externo (pino D3) ou segundo sensor de presença (IR/ultrassônico).

---

## Fluxo de Dados

```
KY-036 D0 ──► D2 ──► state machine (debounce) ──► transição de estado
KY-036 A0 ──► A0 ──► debug via Serial (opcional)
Estado      ──►      motor.cpp (forward/reverse/stop)
Estado      ──►      display.cpp (updateStatus, updateCounts)
```

---

## Tratamento de Erros

| Situação | Comportamento |
|---|---|
| OLED não inicializa | `Serial.println` erro + pisca LED_BUILTIN em loop infinito |
| Falso trigger sensor | Debounce 3 leituras antes de aceitar detecção |
| Motor travado | Sem proteção HW neste projeto — não usar `delay()` longo no loop |

---

## Limitações

- KY-036 requer contato físico da sonda com o item — não detecta à distância
- Contagem não-metal: itens que não trigaram o sensor chegam ao fim da esteira; contagem por subtração (total − metal) requer botão externo ou lógica temporal futura
- Motor plástico: não indicado para uso contínuo prolongado — pode aquecer

---

## Libs Necessárias

- `Adafruit_SSD1306`
- `Adafruit_GFX`
- `Wire` (built-in Arduino)

---

## Plano de Testes

| Teste | Método | Critério de sucesso |
|---|---|---|
| Detecção metal | Encostar moeda na sonda | D0=HIGH, motor reverte, OLED mostra "METAL!" |
| Não-metal | Colocar papel/plástico | Motor segue forward, sem transição de estado |
| Debounce | Toque rápido (<150ms) | Não triggera reverso |
| Contador metal | 5 itens metálicos | OLED exibe "Metal: 005" |
| Timing reverso | Serial.print(millis()) | ~1500ms entre stop e retorno forward |
| OLED endereço | I2C scanner sketch | Confirmar 0x3C ou 0x3D |
