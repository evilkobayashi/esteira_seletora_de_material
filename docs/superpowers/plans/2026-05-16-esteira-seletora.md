# Esteira Seletora de Material — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implementar firmware Arduino para esteira seletora que detecta metal via KY-036, reverte motor via L298N, e exibe status + contador no OLED SSD1306.

**Architecture:** Máquina de estados sem delay() bloqueante usando millis() — DETECTANDO → METAL_DETECTADO → REVERTENDO. Módulos separados para sensor, motor e display com interfaces limpas. Toda configuração centralizada em config.h.

**Tech Stack:** Arduino IDE, C++, Adafruit_SSD1306 v2.x, Adafruit_GFX, Wire (built-in)

---

## Mapa de Arquivos

| Arquivo | Responsabilidade |
|---|---|
| `esteira_seletora/config.h` | Todos os pinos, timing e constantes |
| `esteira_seletora/sensor.h` | Interface KY-036 |
| `esteira_seletora/sensor.cpp` | Implementação leitura digital/analógica |
| `esteira_seletora/motor.h` | Interface L298N |
| `esteira_seletora/motor.cpp` | Forward, reverse, stop via PWM |
| `esteira_seletora/display.h` | Interface OLED |
| `esteira_seletora/display.cpp` | Init SSD1306, status, contadores |
| `esteira_seletora/esteira_seletora.ino` | setup(), loop(), state machine |

---

### Task 1: Estrutura do projeto + config.h

**Files:**
- Create: `esteira_seletora/config.h`
- Modify: `esteira_seletora/esteira_seletora.ino`

- [ ] **Step 1: Criar sketch no Arduino IDE**

File → New → salvar como `esteira_seletora`. Isso cria pasta `esteira_seletora/` com `esteira_seletora.ino` vazio.

- [ ] **Step 2: Instalar bibliotecas**

Arduino IDE → Tools → Manage Libraries:
- Buscar `Adafruit SSD1306` → instalar v2.5.x (instala `Adafruit GFX Library` automaticamente como dependência)

- [ ] **Step 3: Criar config.h**

Na pasta `esteira_seletora/`, criar `config.h`:

```cpp
#pragma once

// Pinos motor (L298N)
#define PIN_MOTOR_IN1        4
#define PIN_MOTOR_IN2        5
#define PIN_MOTOR_ENA        9   // PWM — remover jumper ENA do L298N

// Pinos sensor (KY-036)
#define PIN_SENSOR_DIGITAL   2
#define PIN_SENSOR_ANALOG    A0

// OLED (SSD1306 I2C)
#define OLED_ADDRESS         0x3C  // se nao funcionar, tentar 0x3D
#define OLED_WIDTH           128
#define OLED_HEIGHT          64
#define OLED_RESET           -1    // sem pino reset dedicado

// Timing (ms)
#define TEMPO_PARADA_MS       200   // parado antes de reverter
#define TEMPO_REVERSO_MS     1500   // duracao do reverso
#define INTERVALO_POLLING_MS   50   // frequencia de leitura do sensor

// Motor
#define VELOCIDADE_MOTOR      180   // 0-255 PWM

// Debounce
#define DEBOUNCE_LEITURAS       3   // leituras consecutivas HIGH para confirmar metal
```

- [ ] **Step 4: Verificar compilacao minima**

Em `esteira_seletora.ino`, adicionar:

```cpp
#include "config.h"

void setup() {
    Serial.begin(9600);
    Serial.println("config.h OK");
}

void loop() {}
```

Sketch → Verify/Compile (Ctrl+R) → 0 errors. Upload → Serial Monitor → verificar: `config.h OK`

- [ ] **Step 5: Commit**

```bash
git init
git add esteira_seletora/config.h esteira_seletora/esteira_seletora.ino
git commit -m "feat: project structure and config constants"
```

---

### Task 2: Modulo sensor (KY-036)

**Files:**
- Create: `esteira_seletora/sensor.h`
- Create: `esteira_seletora/sensor.cpp`
- Modify: `esteira_seletora/esteira_seletora.ino` (teste)

- [ ] **Step 1: Criar sensor.h**

```cpp
#pragma once
#include <Arduino.h>

void sensorInit();
bool sensorDigital();
int  sensorAnalog();
```

- [ ] **Step 2: Criar sensor.cpp**

```cpp
#include "sensor.h"
#include "config.h"

void sensorInit() {
    pinMode(PIN_SENSOR_DIGITAL, INPUT);
}

bool sensorDigital() {
    return digitalRead(PIN_SENSOR_DIGITAL) == HIGH;
}

int sensorAnalog() {
    return analogRead(PIN_SENSOR_ANALOG);
}
```

- [ ] **Step 3: Escrever teste serial em esteira_seletora.ino**

```cpp
#include "config.h"
#include "sensor.h"

void setup() {
    Serial.begin(9600);
    sensorInit();
    Serial.println("Sensor test. Encoste metal na sonda...");
}

void loop() {
    bool metal = sensorDigital();
    int  valor = sensorAnalog();
    Serial.print("D0="); Serial.print(metal);
    Serial.print("  A0="); Serial.println(valor);
    delay(200);
}
```

- [ ] **Step 4: Verificar comportamento**

Upload → Serial Monitor (9600 baud):
- Sem toque: `D0=0  A0=<valor baixo>`
- Moeda na sonda: `D0=1  A0=<valor alto>`
- Se D0 nunca vai a 1: girar potenciometro do KY-036 ate threshold correto

- [ ] **Step 5: Commit**

```bash
git add esteira_seletora/sensor.h esteira_seletora/sensor.cpp esteira_seletora/esteira_seletora.ino
git commit -m "feat: add KY-036 sensor module"
```

---

### Task 3: Modulo motor (L298N)

**Files:**
- Create: `esteira_seletora/motor.h`
- Create: `esteira_seletora/motor.cpp`
- Modify: `esteira_seletora/esteira_seletora.ino` (teste)

- [ ] **Step 1: Criar motor.h**

```cpp
#pragma once
#include <Arduino.h>

void motorInit();
void motorForward(uint8_t speed);
void motorReverse(uint8_t speed);
void motorStop();
```

- [ ] **Step 2: Criar motor.cpp**

```cpp
#include "motor.h"
#include "config.h"

void motorInit() {
    pinMode(PIN_MOTOR_IN1, OUTPUT);
    pinMode(PIN_MOTOR_IN2, OUTPUT);
    pinMode(PIN_MOTOR_ENA, OUTPUT);
    motorStop();
}

void motorForward(uint8_t speed) {
    digitalWrite(PIN_MOTOR_IN1, HIGH);
    digitalWrite(PIN_MOTOR_IN2, LOW);
    analogWrite(PIN_MOTOR_ENA, speed);
}

void motorReverse(uint8_t speed) {
    digitalWrite(PIN_MOTOR_IN1, LOW);
    digitalWrite(PIN_MOTOR_IN2, HIGH);
    analogWrite(PIN_MOTOR_ENA, speed);
}

void motorStop() {
    digitalWrite(PIN_MOTOR_IN1, LOW);
    digitalWrite(PIN_MOTOR_IN2, LOW);
    analogWrite(PIN_MOTOR_ENA, 0);
}
```

- [ ] **Step 3: Escrever teste serial em esteira_seletora.ino**

```cpp
#include "config.h"
#include "motor.h"

void setup() {
    Serial.begin(9600);
    motorInit();

    Serial.println("Forward 2s");
    motorForward(VELOCIDADE_MOTOR);
    delay(2000);

    Serial.println("Stop 1s");
    motorStop();
    delay(1000);

    Serial.println("Reverse 2s");
    motorReverse(VELOCIDADE_MOTOR);
    delay(2000);

    Serial.println("Stop");
    motorStop();
}

void loop() {}
```

- [ ] **Step 4: Verificar comportamento**

Upload → observar motor:
- Gira forward 2s → para 1s → gira reverse 2s → para
- Se nao girar: verificar jumper ENA removido e D9 conectado, verificar alimentacao externa L298N (lembrar queda ~2V interna — fonte motor >= tensao nominal + 2V)
- Se girar errado em "forward": trocar `HIGH`/`LOW` em `motorForward()` no motor.cpp

- [ ] **Step 5: Commit**

```bash
git add esteira_seletora/motor.h esteira_seletora/motor.cpp esteira_seletora/esteira_seletora.ino
git commit -m "feat: add L298N motor control module"
```

---

### Task 4: Modulo display (OLED SSD1306)

**Files:**
- Create: `esteira_seletora/display.h`
- Create: `esteira_seletora/display.cpp`
- Modify: `esteira_seletora/esteira_seletora.ino` (teste)

- [ ] **Step 1: Criar display.h**

```cpp
#pragma once
#include <Arduino.h>

bool displayInit();
void displayStatus(const char* status);
void displayCounts(uint16_t metalCount);
```

- [ ] **Step 2: Criar display.cpp**

```cpp
#include "display.h"
#include "config.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

bool displayInit() {
    if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        return false;
    }
    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(10, 0);
    oled.println("ESTEIRA SELETORA");
    oled.display();
    return true;
}

void displayStatus(const char* status) {
    oled.fillRect(0, 20, OLED_WIDTH, 16, SSD1306_BLACK);
    oled.setTextSize(1);
    oled.setCursor(0, 20);
    oled.print("Status: ");
    oled.println(status);
    oled.display();
}

void displayCounts(uint16_t metalCount) {
    oled.fillRect(0, 40, OLED_WIDTH, 24, SSD1306_BLACK);
    oled.setTextSize(1);
    oled.setCursor(0, 40);
    oled.print("Metal:     ");
    oled.println(metalCount);
    oled.setCursor(0, 52);
    oled.println("Nao-metal: ---");
    oled.display();
}
```

- [ ] **Step 3: Escrever teste serial em esteira_seletora.ino**

```cpp
#include "config.h"
#include "display.h"

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    if (!displayInit()) {
        Serial.println("ERRO: OLED nao encontrado. Verificar SDA->A4, SCL->A5 e endereco I2C.");
        while (true) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            delay(300);
        }
    }
    Serial.println("OLED OK");

    delay(1000);
    displayStatus("DETECTANDO");
    delay(1500);
    displayStatus("METAL!");
    displayCounts(3);
    delay(1500);
    displayStatus("REVERTENDO");
}

void loop() {}
```

- [ ] **Step 4: Verificar comportamento**

Upload → observar OLED:
- Aparece "ESTEIRA SELETORA" no topo
- Status muda: DETECTANDO → METAL! → REVERTENDO
- Contador: `Metal: 3`, `Nao-metal: ---`
- Se nada aparecer: verificar SDA→A4, SCL→A5. Testar OLED_ADDRESS 0x3D em config.h se 0x3C falhar

- [ ] **Step 5: Commit**

```bash
git add esteira_seletora/display.h esteira_seletora/display.cpp esteira_seletora/esteira_seletora.ino
git commit -m "feat: add SSD1306 OLED display module"
```

---

### Task 5: State machine principal

**Files:**
- Modify: `esteira_seletora/esteira_seletora.ino` (implementacao final)

- [ ] **Step 1: Substituir esteira_seletora.ino pelo firmware completo**

```cpp
#include "config.h"
#include "sensor.h"
#include "motor.h"
#include "display.h"

enum Estado { DETECTANDO, METAL_DETECTADO, REVERTENDO };

static Estado   estadoAtual = DETECTANDO;
static uint32_t tempoEstado = 0;
static uint32_t ultimoPoll  = 0;
static uint16_t contMetal   = 0;
static uint8_t  debounce    = 0;

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    sensorInit();
    motorInit();

    if (!displayInit()) {
        Serial.println("ERRO: OLED falhou. Verificar fiacao I2C.");
        while (true) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            delay(300);
        }
    }

    motorForward(VELOCIDADE_MOTOR);
    displayStatus("DETECTANDO");
    displayCounts(contMetal);
    Serial.println("Sistema iniciado.");
}

void loop() {
    uint32_t agora = millis();

    switch (estadoAtual) {

        case DETECTANDO:
            if (agora - ultimoPoll >= INTERVALO_POLLING_MS) {
                ultimoPoll = agora;
                if (sensorDigital()) {
                    debounce++;
                    Serial.print("Debounce: "); Serial.println(debounce);
                    if (debounce >= DEBOUNCE_LEITURAS) {
                        debounce    = 0;
                        contMetal++;
                        estadoAtual = METAL_DETECTADO;
                        tempoEstado = agora;
                        motorStop();
                        displayStatus("METAL!");
                        displayCounts(contMetal);
                        Serial.print("Metal detectado! Total: "); Serial.println(contMetal);
                    }
                } else {
                    debounce = 0;
                }
            }
            break;

        case METAL_DETECTADO:
            if (agora - tempoEstado >= TEMPO_PARADA_MS) {
                estadoAtual = REVERTENDO;
                tempoEstado = agora;
                motorReverse(VELOCIDADE_MOTOR);
                displayStatus("REVERTENDO");
                Serial.println("Revertendo...");
            }
            break;

        case REVERTENDO:
            if (agora - tempoEstado >= TEMPO_REVERSO_MS) {
                estadoAtual = DETECTANDO;
                motorForward(VELOCIDADE_MOTOR);
                displayStatus("DETECTANDO");
                Serial.println("Detectando...");
            }
            break;
    }
}
```

- [ ] **Step 2: Compilar sem upload**

Sketch → Verify/Compile (Ctrl+R)  
Expected: 0 errors

- [ ] **Step 3: Commit**

```bash
git add esteira_seletora/esteira_seletora.ino
git commit -m "feat: implement state machine DETECTANDO/METAL_DETECTADO/REVERTENDO"
```

---

### Task 6: Teste de integracao

**Files:** nenhum novo — teste via Serial Monitor e hardware

- [ ] **Step 1: Upload e verificar boot**

Upload → Serial Monitor (9600 baud)  
Expected: `Sistema iniciado.`  
OLED: "ESTEIRA SELETORA" + "Status: DETECTANDO" + "Metal: 0" + "Nao-metal: ---"  
Motor: girando forward

- [ ] **Step 2: Testar deteccao metal**

Encostar moeda ou chave na sonda do KY-036 por ~200ms  
Serial Monitor expected:
```
Debounce: 1
Debounce: 2
Debounce: 3
Metal detectado! Total: 1
Revertendo...
Detectando...
```
OLED: METAL! → REVERTENDO → DETECTANDO  
Motor: para → reverte → forward

- [ ] **Step 3: Testar debounce**

Toque rapido (<100ms) na sonda  
Expected: aparece `Debounce: 1` (ou 2) mas NAO triggera reverso

- [ ] **Step 4: Testar contagem acumulada**

Repetir 5 toques metalicos  
Expected: Serial mostra `Total: 5`, OLED mostra `Metal:     5`

- [ ] **Step 5: Verificar timing reverso**

Anotar tempo no Serial: `Revertendo...` → `Detectando...`  
Expected: ~1500ms de diferenca (ajustar TEMPO_REVERSO_MS em config.h se necessario)

- [ ] **Step 6: Commit final**

```bash
git add -A
git commit -m "feat: integration verified - esteira seletora complete"
```
