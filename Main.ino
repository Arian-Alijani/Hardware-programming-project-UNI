#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define the height of the yellow section
#define YELLOW_SECTION_HEIGHT 16

// Pins
const int BUTTON_PIN = 0;
const int LED_PIN = 2;

// States for the simulation
enum SystemState {
  SPLASH_SCREEN,
  WAITING_FOR_START,
  BOOTING_PHASE,
  BOOT_FINISHED
};
SystemState currentState = SPLASH_SCREEN;

bool bootErrorOccurredGlobal = false;
bool buttonPressedFlag = false;

// --- Helper function for centered, single-line text ---
void displayCenteredMessage(const String& text, bool invertDisplayState, int textSize, int yOffset = 0) {
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE); 
  display.setTextWrap(false);

  if (invertDisplayState) {
    display.invertDisplay(true);
  } else {
    display.invertDisplay(false);
  }

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, ((SCREEN_HEIGHT - h) / 2) + yOffset);
  display.print(text);
  display.display();
}

// --- Simulate Hardware Test ---
void simulateHardwareTest(const String& componentName, bool shouldFail, int durationMs) {
  display.setTextWrap(true); 
  display.invertDisplay(false);

  String bootingText = "Booting..."; 

  int numSteps = 100; 
  if (durationMs < numSteps * 10) durationMs = numSteps * 10; 
  unsigned long stepDelay = (durationMs / 2) / numSteps;
  if (stepDelay < 1) stepDelay = 1; 

  for (int i = 0; i <= numSteps; ++i) {
    display.clearDisplay(); 
    display.setTextColor(SSD1306_WHITE); 

    display.setTextSize(1); 
    int16_t x1_bt, y1_bt; uint16_t w_bt, h_bt;
    display.getTextBounds(bootingText, 0, 0, &x1_bt, &y1_bt, &w_bt, &h_bt);
    display.setCursor((SCREEN_WIDTH - w_bt) / 2, (YELLOW_SECTION_HEIGHT - h_bt) / 2); 
    display.print(bootingText);

    display.setTextSize(1);
    int textStartX = 5; 
    int line1Y_checking = YELLOW_SECTION_HEIGHT + 3;
    int line2Y_component = line1Y_checking + 8 + 2; 

    display.setCursor(textStartX, line1Y_checking);
    display.println("Checking:");

    display.setCursor(textStartX, line2Y_component);
    display.println(componentName); 
    
    int progressBarY = line2Y_component + 8 + 5; 
    if (componentName.length() > 21) { 
         progressBarY = line2Y_component + (8*2) + 5; 
    }
    if (progressBarY + 8 + 8 + 4 > SCREEN_HEIGHT) { 
        progressBarY = SCREEN_HEIGHT - 8 - 8 - 6; 
    }

    int barWidth = SCREEN_WIDTH - 20; 
    int barX = 10;
    int barHeight = 6; 
    display.drawRect(barX - 1, progressBarY - 1, barWidth + 2, barHeight + 2, SSD1306_WHITE);
    int fillWidth = (i * barWidth) / numSteps;
    display.fillRect(barX, progressBarY, fillWidth, barHeight, SSD1306_WHITE); 
    
    String percentageText = String((i * 100) / numSteps) + "%";
    int16_t x1_perc, y1_perc; uint16_t w_perc, h_perc;
    display.getTextBounds(percentageText, 0, 0, &x1_perc, &y1_perc, &w_perc, &h_perc);
    display.setCursor((SCREEN_WIDTH - w_perc) / 2, progressBarY + barHeight + 2); 
    display.print(percentageText);
    
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    if (YELLOW_SECTION_HEIGHT > 0 && YELLOW_SECTION_HEIGHT < SCREEN_HEIGHT) {
        display.drawFastHLine(0, YELLOW_SECTION_HEIGHT, SCREEN_WIDTH, SSD1306_WHITE);
    }

    display.display();
    if (i < numSteps) delay(stepDelay);
  }
  delay(100); 

  if (shouldFail) {
    digitalWrite(LED_PIN, HIGH);
    bootErrorOccurredGlobal = true;
    displayCenteredMessage("ERROR", true, 3); 
    delay(durationMs / 2);
    digitalWrite(LED_PIN, LOW);
  } else { 
    display.fillRect(0, YELLOW_SECTION_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - YELLOW_SECTION_HEIGHT, SSD1306_BLACK);

    display.setTextSize(1); 
    display.setTextColor(SSD1306_WHITE); 
    int16_t x1_bt_ok, y1_bt_ok; uint16_t w_bt_ok, h_bt_ok; 
    display.getTextBounds(bootingText, 0, 0, &x1_bt_ok, &y1_bt_ok, &w_bt_ok, &h_bt_ok);
    display.setCursor((SCREEN_WIDTH - w_bt_ok) / 2, (YELLOW_SECTION_HEIGHT - h_bt_ok) / 2); 
    display.print(bootingText);
    
    display.invertDisplay(false); 
    display.setTextWrap(false);
    display.setTextColor(SSD1306_WHITE); 

    display.setTextSize(2);
    String okMsg = "OK";
    int16_t x1_ok, y1_ok; uint16_t w_ok, h_ok;
    display.getTextBounds(okMsg, 0, 0, &x1_ok, &y1_ok, &w_ok, &h_ok);
    int blueSectionMidY = YELLOW_SECTION_HEIGHT + (SCREEN_HEIGHT - YELLOW_SECTION_HEIGHT) / 2;
    display.setCursor((SCREEN_WIDTH - w_ok) / 2, blueSectionMidY - h_ok - 2);
    display.print(okMsg);

    display.setTextSize(1);
    int16_t x1_comp, y1_comp; uint16_t w_comp, h_comp;
    display.getTextBounds(componentName, 0, 0, &x1_comp, &y1_comp, &w_comp, &h_comp);
    display.setCursor((SCREEN_WIDTH - w_comp) / 2, blueSectionMidY + 2);
    display.print(componentName); 
    
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    if (YELLOW_SECTION_HEIGHT > 0 && YELLOW_SECTION_HEIGHT < SCREEN_HEIGHT) {
        display.drawFastHLine(0, YELLOW_SECTION_HEIGHT, SCREEN_WIDTH, SSD1306_WHITE);
    }

    display.display();
    delay(durationMs / 2); 
  }
}

// --- Button Handling Function ---
void checkButton() {
    static unsigned long lastDebounceTime = 0;
    static int debouncedButtonState = HIGH;
    static int lastRawButtonState = HIGH;
    const int debounceDelay = 50;

    int currentRawButtonState = digitalRead(BUTTON_PIN);

    if (currentRawButtonState != lastRawButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (currentRawButtonState != debouncedButtonState) {
            debouncedButtonState = currentRawButtonState;
            if (debouncedButtonState == LOW) {
                Serial.println("BOOT Button Pressed - Flag SET!");
                buttonPressedFlag = true;
            }
        }
    }
    lastRawButtonState = currentRawButtonState;
}

// --- Arduino Setup Function ---
void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  randomSeed(micros()); 
}

// --- Arduino Main Loop ---
void loop() {
  checkButton();

  switch (currentState) {
    case SPLASH_SCREEN: {
      display.clearDisplay();
      display.setTextWrap(false);
      display.invertDisplay(false);
      display.setTextColor(SSD1306_WHITE);

      display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
      if (YELLOW_SECTION_HEIGHT > 0 && YELLOW_SECTION_HEIGHT < SCREEN_HEIGHT) {
          display.drawFastHLine(0, YELLOW_SECTION_HEIGHT, SCREEN_WIDTH, SSD1306_WHITE);
      }

      // Line 1: "ESP BOOT" (textSize 2, Yellow)
      display.setTextSize(2);
      String s_line1 = "ESP BOOT";
      int16_t x1_s1, y1_s1; uint16_t w_s1, h_s1;
      display.getTextBounds(s_line1, 0, 0, &x1_s1, &y1_s1, &w_s1, &h_s1);
      display.setCursor((SCREEN_WIDTH - w_s1) / 2, (YELLOW_SECTION_HEIGHT - h_s1) / 2);
      display.print(s_line1); 

      // Line 2: "SIMULATOR v1.9" (textSize 1, Blue) - Adjusted Y
      display.setTextSize(1);
      String s_line2 = "SIMULATOR v1.9"; // Version Updated
      int16_t x1_s2, y1_s2; uint16_t w_s2, h_s2;
      display.getTextBounds(s_line2, 0, 0, &x1_s2, &y1_s2, &w_s2, &h_s2);
      int line2_Y = YELLOW_SECTION_HEIGHT + 6; // 6px gap from yellow/blue divider
      display.setCursor((SCREEN_WIDTH - w_s2) / 2, line2_Y); 
      display.print(s_line2);
      
      // Line 3: "by Arian Alijani" (textSize 1, Blue) - Adjusted Y to be closer to Line 2
      String s_line3 = "by Arian Alijani";
      int16_t x1_s3, y1_s3; uint16_t w_s3, h_s3; // h_s2 is height of s_line2 (approx 8px)
      display.getTextBounds(s_line3, 0, 0, &x1_s3, &y1_s3, &w_s3, &h_s3);
      // Position Line 3 below Line 2 with a smaller gap
      display.setCursor((SCREEN_WIDTH - w_s3) / 2, line2_Y + h_s2 + 4); // 4px gap after s_line2
      display.print(s_line3);
      
      display.display();
      delay(3500);
      currentState = WAITING_FOR_START;
      break;
    }

    case WAITING_FOR_START: {
      digitalWrite(LED_PIN, LOW);
      bootErrorOccurredGlobal = false;
      
      display.clearDisplay();
      display.setTextWrap(false);
      display.invertDisplay(false);
      display.setTextColor(SSD1306_WHITE);

      display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
      if (YELLOW_SECTION_HEIGHT > 0 && YELLOW_SECTION_HEIGHT < SCREEN_HEIGHT) {
          display.drawFastHLine(0, YELLOW_SECTION_HEIGHT, SCREEN_WIDTH, SSD1306_WHITE);
      }
      
      display.setTextSize(1);
      String waitMsgL1 = "PRESS BOOT";
      int16_t x1_wm1, y1_wm1; uint16_t w_wm1, h_wm1;
      display.getTextBounds(waitMsgL1, 0, 0, &x1_wm1, &y1_wm1, &w_wm1, &h_wm1);
      display.setCursor((SCREEN_WIDTH - w_wm1) / 2, YELLOW_SECTION_HEIGHT + 10); 
      display.print(waitMsgL1);

      String waitMsgL2 = "TO START";
      int16_t x1_wm2, y1_wm2; uint16_t w_wm2, h_wm2;
      display.getTextBounds(waitMsgL2, 0, 0, &x1_wm2, &y1_wm2, &w_wm2, &h_wm2);
      display.setCursor((SCREEN_WIDTH - w_wm2) / 2, YELLOW_SECTION_HEIGHT + 10 + h_wm1 + 5);
      display.print(waitMsgL2);

      static uint32_t lastBlinkTime = 0;
      static bool showPrompt = true;
      if (millis() - lastBlinkTime > 500) { 
          showPrompt = !showPrompt;
          lastBlinkTime = millis();
      }
      if (showPrompt) {
          display.setTextSize(2);
          String promptChar = ">";
          int16_t x1_p, y1_p; uint16_t w_p, h_p;
          display.getTextBounds(promptChar,0,0, &x1_p, &y1_p, &w_p, &h_p);
          int blueSectionActualHeight = SCREEN_HEIGHT - YELLOW_SECTION_HEIGHT;
          int promptY = YELLOW_SECTION_HEIGHT + (blueSectionActualHeight - h_p) / 2;
          int promptX = SCREEN_WIDTH - w_p - 10; 
          display.setCursor(promptX, promptY); 
          display.print(promptChar);
      }
      
      display.display();
      
      if (buttonPressedFlag) {
        buttonPressedFlag = false;
        currentState = BOOTING_PHASE;
      }
      break;
    }

    case BOOTING_PHASE: {
      simulateHardwareTest("Memory Test", true, 3000);
      simulateHardwareTest("Storage Check", false, 3500);
      simulateHardwareTest("WiFi Module", true, 4000); 
      simulateHardwareTest("Bluetooth Radio", false, 2500); 
      simulateHardwareTest("Sensor Array", false, 3000);
      simulateHardwareTest("Real-Time Clock", false, 2000); 
      simulateHardwareTest("Power System", true, 3500); 
      simulateHardwareTest("Secure Element", false, 3000); 
      simulateHardwareTest("Display Output", false, 2000); 
      simulateHardwareTest("Network Core", false, 3500); 
      
      currentState = BOOT_FINISHED;
      break;
    }

    case BOOT_FINISHED: {
      display.clearDisplay();
      display.invertDisplay(false);
      display.setTextWrap(true); 
      display.setTextColor(SSD1306_WHITE);

      display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
      if (YELLOW_SECTION_HEIGHT > 0 && YELLOW_SECTION_HEIGHT < SCREEN_HEIGHT) {
          display.drawFastHLine(0, YELLOW_SECTION_HEIGHT, SCREEN_WIDTH, SSD1306_WHITE);
      }

      display.setTextSize(1);
      String headerFinish = "BOOT STATUS";
      int16_t x1_hf, y1_hf; uint16_t w_hf, h_hf; 
      display.getTextBounds(headerFinish, 0, 0, &x1_hf, &y1_hf, &w_hf, &h_hf);
      display.setCursor((SCREEN_WIDTH - w_hf) / 2, (YELLOW_SECTION_HEIGHT - h_hf) / 2 ); 
      display.print(headerFinish); 

      display.setTextSize(1);
      int blueSectionTextStartY = YELLOW_SECTION_HEIGHT + 5; 
      int textIndentX = 5;
      int lineSpacing = 1; 
      int fontHeight = 8;  

      String statusMsg1, statusMsg2, actionMsg1, actionMsg2;

      if (bootErrorOccurredGlobal) {
        digitalWrite(LED_PIN, HIGH);
        statusMsg1 = " Status: ISSUES";    
        statusMsg2 = "         DETECTED!"; 
        actionMsg1 = " Action: Press BOOT";
        actionMsg2 = "         to retry";  
      } else {
        digitalWrite(LED_PIN, LOW);
        statusMsg1 = " Status: System";
        statusMsg2 = "         Nominal.";
        actionMsg1 = " Action: Press BOOT";
        actionMsg2 = "         to run";
      }
      
      int currentY = blueSectionTextStartY;

      display.setCursor(textIndentX, currentY);
      display.println(statusMsg1); 
      currentY += fontHeight + lineSpacing; 

      display.setCursor(textIndentX, currentY);
      display.println(statusMsg2);
      currentY += fontHeight + lineSpacing + 2; 

      display.setCursor(textIndentX, currentY);
      display.println(actionMsg1);
      currentY += fontHeight + lineSpacing;

      display.setCursor(textIndentX, currentY);
      display.println(actionMsg2);
      
      display.display();

      if (buttonPressedFlag) {
        buttonPressedFlag = false;
        currentState = SPLASH_SCREEN;
        delay(200); 
      }
      break;
    }
  }
}
