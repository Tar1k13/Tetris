#include <Arduino.h>
#include "Constants.h"
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <TM1637Display.h>
#include <helperFuncs/calFunc.h>
#include <movementFuncs/MovementFuncs.h>

TM1637Display display = TM1637Display(25, 26);

// Uncomment according to your hardware type
// #define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Map led matrix column positions

int fig = 0;            // current figure
int rot = 0;            // current rotation
int currentColumn = 0;  // current column
int height = 0;         // current height
int mv = 0;             // current rotation mudslide due to rotation bondaries
int cl = 0;
int currentfigure[5];  // current figure
int matrix[17];
int speed = 500;
int currentSpeed = 500;

// one-click flags
boolean flag1 = false;
boolean flag2 = false;
boolean flag3 = false;
boolean flag4 = false;
// input listener

void deleteAnimation(int* elemsToDel, int col, int heigh) {
  int ts = 2;
  int tx = 1;
  for (int t = col; t < col + heigh; t++) {
    while (elemsToDel[t] != 0) {
      int elem = elemsToDel[t] >> ts << tx;
      mx.setColumn(mapping[t], elem);
      ts += 2;
      tx++;
      elemsToDel[t] = elem;
      delay(100);
    }
  }
}

// void moveDown(int col,int currentfigure[],int matrix[]) {
//   int bias = 0;
//   for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
//     if (col + bias >= 0)
//       mx.setColumn(mapping[col + bias], currentfigure[i] + matrix[col + bias]);
//     if (col != 0 && col > 0) mx.setColumn(mapping[col - 1], matrix[col - 1]);
//     bias++;
//   }
// }

void saveToMatrix(int* matrix,int cCol) {
  int bias = 0;
  for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
    int elem = currentfigure[i] + matrix[cCol + bias];
    matrix[cCol + bias] = elem;
    bias++;
  }
  for (int i = 0; i < 16; i++) {
    Serial.print(matrix[i]);
    Serial.print(" ");
  }
}

void deleteLine(int* matr,
                void (*deleteAnim)(int* elemToDelete, int col, int heigh),
                int heigh, int col) {
  int elemToDel[16] = {0};
  boolean isDel = false;
  for (int i = col; i < col + heigh; i++) {
    Serial.println(countOnes(matr[i], 8));
    if (countOnes(matr[i], 8) == 8) {
      isDel = true;
      elemToDel[i] = matr[i];
      matr[i] = 0x0;
      for (int t = i; t > 0; t--) {
        matr[t] = matr[t - 1];
      }
    }
  }
  if (isDel) {
    deleteAnim(elemToDel, col, heigh);
    for (int i = 15; i >= 0; i--) {
      // Serial.println(mapping[i],matrix[i+1]);
      mx.setColumn(mapping[i], matr[i]);
    }
  }
}
boolean isSpeed=false;
void control_listener(void* pvParameters) {
  while (true) {
    int height = calculateHeight(figures[fig][rot]);

    // left
    if (digitalRead(LEFT_BUTTON) == LOW && !flag1 && checkBoundings(false,currentfigure,fig)) {
      flag1 = true;
      move(currentColumn, currentfigure, false,fig,matrix,mx);
      mv--;
    }
    if (digitalRead(LEFT_BUTTON) == HIGH) flag1 = false;

    // right
    if (digitalRead(RIGHT_BUTTON) == LOW && !flag2 && checkBoundings(true,currentfigure,fig)) {
      flag2 = true;
      move(currentColumn, currentfigure, true,fig,matrix,mx);
      mv++;
    }
    if (digitalRead(RIGHT_BUTTON) == HIGH) flag2 = false;

    // rotate
    if (digitalRead(ROTATE_BUTTON) == LOW && !flag3) {
      flag3 = true;
      rotate(currentColumn, currentfigure, mv,fig,rot,matrix,mx,height,cl);
    }
    if (digitalRead(ROTATE_BUTTON) == HIGH) flag3 = false;

    // speed up
    if(digitalRead(SPEED_BUTTON) == LOW)
      currentSpeed=30;
    else
      currentSpeed=speed;

    // // skip
    if (digitalRead(SKIP_BUTTON) == LOW && !flag4) {
      flag4 = true;
      isSpeed=true;
      int cCol=0;
      cl = scope[fig][0] - startPos[fig][rot];
    // Serial.println(cl);
      for (int col = cl+currentColumn; col < 17 - height + cl; col++) {
        if (!checkObjection(col,currentfigure,matrix,fig)) {
          // Serial.println(col);
          moveDown(col,currentfigure,matrix,fig,mx);
          cCol = col;
        } else
          break;
      }
      saveToMatrix(matrix,cCol);
      Serial.println("Ha");
      deleteLine(matrix, &deleteAnimation, height, cCol);
      speed -= 5;
      currentSpeed = speed;
      rot = 0;
      mv = 0;
      cl = 0;

    }
    if (digitalRead(SKIP_BUTTON) == HIGH) flag4 = false;
    vTaskDelay(40 / portTICK_RATE_MS);
  }
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));  // randomizer
  mx.begin();                 // led matrix initialization
  display.setBrightness(7);
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(ROTATE_BUTTON, INPUT_PULLUP);
  pinMode(SPEED_BUTTON, INPUT_PULLUP);
  pinMode(SKIP_BUTTON, INPUT_PULLUP);
  xTaskCreate(control_listener, "calculate_water_temp", 2048, NULL, 2, NULL);
}

void loop() {
  // display.showNumberDec(56000);
  if(!isSpeed){
    fig = random(7);
    rot = random(3);
    for (int i = 0; i < 5; i++) {
      currentfigure[i] = figures[fig][rot][i];
    }
  
    height = calculateHeight(currentfigure);
    boolean objection = false;

    // int currentCol = 0;
    cl = scope[fig][0] - startPos[fig][rot];
    // Serial.println(cl);
    for (int col = cl; col < 17 - height + cl; col++) {
      if (!checkObjection(col,currentfigure,matrix,fig) && !isSpeed) {
        // Serial.println(col);
        moveDown(col,currentfigure,matrix,fig,mx);
        currentColumn = col;
        delay(currentSpeed);
      } else
        break;
    }
    if(!isSpeed){
    saveToMatrix(matrix,currentColumn);
    Serial.println("Ha");
    deleteLine(matrix, &deleteAnimation, height, currentColumn);
    speed -= 5;
    currentSpeed = speed;
    rot = 0;
    mv = 0;
    cl = 0;
    } else{
      isSpeed=false;
    }
  }
}
