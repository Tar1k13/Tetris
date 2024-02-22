#include <Arduino.h>

// #include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <TM1637.h>

using namespace std;

TM1637 tm(25, 26);

// Uncomment according to your hardware type
// #define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// Defining size, and output pins
#define MAX_DEVICES 2
#define CS_PIN 5

// MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

#define BUTTON_PIN 2
// Map led matrix column positions
int mapping[15];
void mapn() {
  int counter = 8;
  for (size_t i = 0; i < 8; i++) {
    mapping[i] = counter++;
  }
  counter = 1;
  for (size_t i = 9; i < 16; i++) {
    mapping[i] = counter++;
  }
}
int buttonStateLeft = 0;
int buttonStateRight = 0;
void control_listener(void *pvParameters) {
  while (true) {
    if (digitalRead(BUTTON_PIN) == LOW) buttonStateLeft = 1;
    if (digitalRead(4) == LOW) buttonStateRight = 1;
    vTaskDelay(100 / portTICK_RATE_MS);
  }
}
void setup() {
  Serial.begin(9600);
  mx.begin();
  tm.begin();
  mapn();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  xTaskCreate(control_listener, "calculate_water_temp", 2048, NULL, 2, NULL);
}

int figures[2][4][5] = {{{0x0, 0x0, 0x38, 0x10, 0x0},
                         {0x0, 0x10, 0x30, 0x10, 0x0},
                         {0x0, 0x0, 0x10, 0x38, 0x0},
                         {0x0, 0x10, 0x38, 0x0, 0x0}},
                        {{0x0, 0x4, 0x4, 0x4, 0x4},
                         {0x0, 0x0, 0x3C, 0x0, 0x0},
                         {0x0, 0x4, 0x4, 0x4, 0x4},
                         {0x0, 0x0, 0x3C, 0x0, 0x0}}};

int startPos[2][4] = {{1, 1, 2, 1}, {1, 2, 1, 2}};

int matrix[16];
int calculateHeight(int fig[]) {
  int counter = 0;
  for (size_t i = 0; i <= sizeof(fig); i++) {
    if (fig[i] != 0) counter++;
  }
  return counter;
}

int countOneDigitsInHex(int hexNum) {
  int count = 0;

  // Shift and mask to isolate each digit
  for (int i = 0; i < 8; i++) {
    if ((hexNum & (0xF << (4 * i))) == 0x1 << (4 * i)) {
      count++;
    }
    hexNum >>= 4;  // Shift right by 4 bits
  }

  return count;
}

void loop() {
  int fig = 0;
  int rot = 2;
  boolean objectionX = false;
  int heightX = calculateHeight(figures[fig][rot]);
  // int width=countOnesInHexList(figures[fig][rot],5);

  int height = calculateHeight(figures[fig][rot]);
  int currentCol = 0;
  boolean objection = false;
  int curState = 0;
  int curState1 = 0;
  for (size_t col = 0; col < 17 - height; col++) {
    // check boundaries

    // left
    if (buttonStateLeft == 1) {
      boolean st = true;
      for (int t = 0; t < height; t++) {
        int elem = figures[fig][rot][startPos[fig][rot] + t] << curState + 1;
        if (countOneDigitsInHex(elem) !=
            countOneDigitsInHex(figures[fig][rot][startPos[fig][rot] + t]
                                << curState)) {
          st = false;
          break;
        }
      }
      if (st) curState += 1;
    }
    // right
    if (buttonStateRight == 1) {
      boolean st = true;
      for (int t = 0; t < height; t++) {
        int elem = figures[fig][rot][startPos[fig][rot] + t] >> curState1 + 1;
        if (countOneDigitsInHex(elem) !=
            countOneDigitsInHex(figures[fig][rot][startPos[fig][rot] + t] >>
                                curState1)) {
          st = false;
          break;
        }
      }
      if (st) curState1 += 1;
    }
    // main part
    for (int t = 0; t < height; t++) {
      int elem =
          figures[fig][rot][startPos[fig][rot] + t] << curState >> curState1;
      if ((elem & matrix[col + t]) != 0) {
        Serial.println("OBJECTION");
        // Serial.println()
        objection = true;
        break;
      }
    }
    // moving blocks
    if (!objection) {
      for (int t = 0; t < height; t++) {
        int elem = (figures[fig][rot][startPos[fig][rot] + t] << curState >>
                    curState1) +
                   matrix[col + t];
        // safe put
        mx.setColumn(mapping[col + t], elem);                           // put
        if (col != 0) mx.setColumn(mapping[col - 1], matrix[col - 1]);  // move
      }
    } else
      break;
    currentCol = col;

    buttonStateLeft = 0;
    buttonStateRight = 0;
    delay(200);
  }
  for (int t = 0; t < height; t++) {
    int sum =
        (figures[fig][rot][startPos[fig][rot] + t] << curState >> curState1) +
        matrix[currentCol + t];
    matrix[currentCol + t] = sum;
  }
}
