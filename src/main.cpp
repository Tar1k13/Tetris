#include <Arduino.h>

// #include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <TM1637.h>

// using namespace std;

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
int buttonStateFlip = 0;
void control_listener(void *pvParameters) {
  while (true) {
    if (digitalRead(BUTTON_PIN) == LOW) buttonStateLeft = 1;
    if (digitalRead(4) == LOW) buttonStateRight = 1;
    if (digitalRead(16) == LOW) buttonStateFlip = 1;
    vTaskDelay(100 / portTICK_RATE_MS);
  }
}
void setup() {
  Serial.begin(9600);
  mx.begin();
  tm.begin();
  mapn();
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // left button
  pinMode(4, INPUT_PULLUP);           // right button
  pinMode(16, INPUT_PULLUP);
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

  // Method 1: Bitwise approach (optimized for Arduino)
  for (int i = 0; i < 8; i++) {  // Iterate through 8 digits
    count += (hexNum & 1);       // Add 1 if least significant bit is 1
    hexNum >>= 1;                // Shift number right by 1 bit
  }

  return count;
}

int countZeroBits(int num, int numBits) {
  int count = 0;
  for (int i = 0; i < numBits; i++) {
    if ((num & (1 << i)) == 0) {
      count++;
    }
  }
  return count;
}
int countOnes(int num, int numBits) {
  int count = 0;
  for (int i = 0; i < numBits; i++) {
    // Check if bit is set (1) using bitwise AND
    if (num & (1 << i)) {
      count++;
    }
  }
  return count;
}

void loop() {
  int fig = 1;
  int rot = 0;
  boolean objectionX = false;
  int heightX = calculateHeight(figures[fig][rot]);
  // int width=countOnesInHexList(figures[fig][rot],5);

  int height = calculateHeight(figures[fig][rot]);
  int currentCol = 0;
  boolean objection = false;
  int curState = 0;
  int curState1 = 0;
  for (size_t col = 0; col < 17 - height; col++) {
    // rotate figures
    if (buttonStateFlip == 1) {
      int newHeight = calculateHeight(figures[fig][rot+1]);
      boolean freeSpace=true;
      //TODO: check rotate ability
      for (int t = 0; t < newHeight; t++) {
         int elem = (figures[fig][rot][startPos[fig][rot] + t] << curState >>
                    curState1) +
                   matrix[col + t];
        // safe put
        while (countOnes(elem, 8) < countOnes(elem, 12)) {
          elem = (elem >> 1) + matrix[col+t];
        }

        if(matrix[col+t]!=0 && (matrix[col+t] | elem) == elem){
          freeSpace=false;
          break;
        };
      }
      if(freeSpace){
        for (int i = 0; i < height; i++) {
          // Serial.println(col+i);
          mx.setColumn(mapping[col + i], matrix[col + i]);
        }
        rot += 1;
        // int newHeight = calculateHeight(figures[fig][rot]);
        height = newHeight;
      }
    }

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
      for (int t = 0; t <= height; t++) {
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
      while (countOnes(elem, 8) < countOnes(elem, 12)) {
        elem = (elem >> 1) + matrix[col+t];
      }
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
        while (countOnes(elem, 8) < countOnes(elem, 12)) {
          elem = (elem >> 1) + matrix[col+t];
        }
        mx.setColumn(mapping[col + t], elem);                           // put
        if (col != 0) mx.setColumn(mapping[col - 1], matrix[col - 1]);  // move
      }
      Serial.println("------");
    } else
      break;
    currentCol = col;

    buttonStateLeft = 0;
    buttonStateRight = 0;
    buttonStateFlip = 0;
    delay(1000);
  }
  for (int t = 0; t < height; t++) {
    int sum =
        (figures[fig][rot][startPos[fig][rot] + t] << curState >> curState1) +
        matrix[currentCol + t];
    while (countOnes(sum, 8) < countOnes(sum, 12)) {
      sum = (sum >> 1) + matrix[currentCol + t];
    }
    matrix[currentCol + t] = sum;
  }
}
