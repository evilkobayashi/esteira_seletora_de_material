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
