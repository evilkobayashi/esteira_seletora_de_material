# Esteira Seletora de Material

Firmware Arduino para esteira transportadora que detecta e separa materiais metálicos automaticamente.

## Como funciona

O sensor KY-036 detecta toque metálico. Quando metal é identificado, o motor reverte e o item é direcionado ao bin de metal. Materiais não-metálicos seguem pela esteira e caem no bin padrão.

```
[item na esteira] → [sensor KY-036] → metal? → SIM → motor reverte → bin metal
                                              → NÃO → motor segue  → bin não-metal
```

### Estados do sistema

```
DETECTANDO ──[metal confirmado]──► METAL_DETECTADO ──[200ms]──► REVERTENDO ──[1500ms]──► DETECTANDO
```

O OLED exibe o estado atual e a contagem de itens metálicos detectados.

## Hardware

| Componente | Função |
|---|---|
| Arduino Uno | Controlador principal |
| OLED SSD1306 128x64 (I2C) | Display de status e contagem |
| L298N | Ponte H — controle do motor |
| KY-036 | Sensor de toque metálico |
| Motor DC c/ caixa de redução | Movimenta a esteira |

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

> **Atenção L298N:** remover jumper ENA e conectar ao D9 para controle PWM. Fonte do motor deve ser tensão nominal do motor + 2V (queda interna do L298N).

## Alimentação

- Arduino: USB (5V)
- Motor: fonte externa via L298N VCC
- Lógica L298N: 5V do Arduino

## Estrutura do projeto

```
esteira_seletora/
├── esteira_seletora.ino    ← sketch principal, state machine
├── config.h                ← pinos e constantes de timing
├── sensor.h / sensor.cpp   ← módulo KY-036
├── motor.h / motor.cpp     ← módulo L298N
└── display.h / display.cpp ← módulo OLED SSD1306
```

## Bibliotecas necessárias

Instalar via Arduino IDE → Tools → Manage Libraries:

- **Adafruit SSD1306** v2.5.x (instala `Adafruit GFX` automaticamente)

## Configuração

Todas as constantes estão em `config.h`:

```cpp
#define TEMPO_REVERSO_MS     1500   // duração do reverso (ms)
#define VELOCIDADE_MOTOR      180   // velocidade PWM (0-255)
#define DEBOUNCE_LEITURAS       3   // leituras para confirmar metal
#define OLED_ADDRESS         0x3C   // endereço I2C (tentar 0x3D se não funcionar)
```

## Como testar

1. Abrir `esteira_seletora/esteira_seletora.ino` no Arduino IDE
2. Instalar bibliotecas (ver acima)
3. Selecionar placa: **Arduino Uno**
4. Upload
5. Abrir Serial Monitor (9600 baud) → verificar `Sistema iniciado.`
6. Encostar moeda na sonda do KY-036 → motor reverte, OLED exibe `METAL!`
7. Ajustar potenciômetro do KY-036 se necessário

## Display OLED

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

> Contador não-metal exibe `---` — o KY-036 não detecta passagem de itens não-metálicos. Uma implementação futura pode adicionar um sensor de presença IR ou botão manual para contagem.
