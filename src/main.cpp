#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <TM1637Display.h>
#include <helperFuncs/calFunc.h>
#include <movementFuncs/MovementFuncs.h>

#include "Constants.h"

TM1637Display display = TM1637Display(25, 26);

// Uncomment according to your hardware type
// #define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);


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
boolean cont = true;  // start button listener
int mainPointCounter=0;

// one-click flags
boolean flag1 = false;
boolean flag2 = false;
boolean flag3 = false;
boolean flag4 = false;
// input listener

int getColFactor(int numToDel,int level){
  switch (numToDel){
    case 2:
      return (40*(level+1)); 
    case 3:
      return (100*(level+1));
    case 4:
      return (300*(level+1));
    case 5:
      return (1200*(level+1));
    default:
      return level+1;
  }
}

void deleteAnimation(int elemsToDel[], int col, int heigh, boolean isRts) {
  int ts = 2;
  int tx = 1;
  int hp=0;
  while(hp<4){
    for (int t = col; t < col + heigh; t++) {
      if (elemsToDel[t] != 0) {
        int elem = elemsToDel[t] >> ts << tx;
        mx.setColumn(mapping[t], elem);
        elemsToDel[t] = elem;
      }
    }
    ts += 2;
    tx++;
    hp++;
    if(!isRts){
      delay(100);
    } else vTaskDelay(100/portTICK_RATE_MS);
  }
}

void saveToMatrix(int* matrix, int cCol) {
  int bias = 0;
  for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
    int elem = currentfigure[i] + matrix[cCol + bias];
    matrix[cCol + bias] = elem;
    bias++;
  }
  // for (int i = 0; i < 16; i++) {
  //   Serial.print(matrix[i]);
  //   Serial.print(" ");
  // }
}

void deleteLine(int* matr,
                void (*deleteAnim)(int* elemToDelete, int col, int heigh,boolean isRts),
                int heigh, int col,boolean isRts) {
  int elemToDel[16] = {0};
  boolean isDel = false;
  int delColNum=0; //counter

  for (int i = col; i <= col + heigh; i++) {
    if (countOnes(matr[i], 8) == 8) {
      delColNum++;
      isDel = true;
      elemToDel[i] = matr[i];
      matr[i] = 0x0;
      for (int t = i; t > 0; t--) {
        matr[t] = matr[t - 1];
      }
    }
  }
  if (isDel) {
    deleteAnim(elemToDel, col, heigh,isRts);
    for (int i = 15; i >= 0; i--) {
      // Serial.println(mapping[i],matrix[i+1]);
      mx.setColumn(mapping[i], matr[i]);
    }
    mainPointCounter+=getColFactor(delColNum,currentColumn);
    display.showNumberDec(mainPointCounter);
  }


}
boolean isSpeed = false;
void control_listener(void* pvParameters) {
  while (true) {
    // int height = calculateHeight(figures[fig][rot]);
    // int con=0;
    // left
    if (digitalRead(LEFT_BUTTON) == LOW &&
        checkBoundings(false, currentfigure, fig) && cont) {
        // con++;
        flag1 = true;
        move(currentColumn, currentfigure, false, fig, matrix, mx);
        mv--;
        vTaskDelay(80/portTICK_RATE_MS);
      }
    
    // if (digitalRead(LEFT_BUTTON) == HIGH) flag1 = false;
    // right
    if (digitalRead(RIGHT_BUTTON) == LOW &&
        checkBoundings(true, currentfigure, fig) && cont) {

        // flag2 = true;
        move(currentColumn, currentfigure, true, fig, matrix, mx);
        mv++;
        vTaskDelay(80/portTICK_RATE_MS);
      }
    
    // if (digitalRead(RIGHT_BUTTON) == HIGH) flag2 = false;

    // rotate
    if (digitalRead(ROTATE_BUTTON) == LOW && !flag3 && cont) {

        flag3 = true;
        rotate(currentColumn, currentfigure, mv, fig, rot, matrix, mx, height,
               cl);
      } 
    
    if (digitalRead(ROTATE_BUTTON) == HIGH) flag3 = false;

    // speed up
    if (digitalRead(SPEED_BUTTON) == LOW && cont) {
        currentSpeed = 30;
      }
     else
      currentSpeed = speed;

    // // skip
    if (digitalRead(SKIP_BUTTON) == LOW && !flag4 && cont) {

        flag4 = true;
        isSpeed = true;
        int cCol = 0;
        cl = scope[fig][0] - startPos[fig][rot];
        // Serial.println(cl);
        for (int col = cl + currentColumn; col < 17 - height + cl; col++) {
          if (!checkObjection(col, currentfigure, matrix, fig)) {
            // Serial.println(col);
            moveDown(col, currentfigure, matrix, fig, mx);
            cCol = col;
            vTaskDelay(10);
          } else
            break;
        }
        saveToMatrix(matrix, cCol);
        Serial.println("Ha");
        deleteLine(matrix, &deleteAnimation, height, cCol,true);
        speed -= 5;
        currentSpeed = speed;
        rot = 0;
        mv = 0;
        cl = 0;
    }
    if (digitalRead(SKIP_BUTTON) == HIGH) flag4 = false;

    if(!cont && (digitalRead(SKIP_BUTTON) == LOW || digitalRead(LEFT_BUTTON) == LOW || digitalRead(RIGHT_BUTTON) == LOW || digitalRead(ROTATE_BUTTON) == LOW || digitalRead(SPEED_BUTTON)==LOW)){
      cont=true;
      vTaskDelay(currentSpeed / portTICK_RATE_MS);
    }
    vTaskDelay(40 / portTICK_RATE_MS);
  }
    
}


void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));  // randomizer
  mx.begin();                 // led matrix initialization
  display.setBrightness(7);
  display.showNumberDec(0000);
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(ROTATE_BUTTON, INPUT_PULLUP);
  pinMode(SPEED_BUTTON, INPUT_PULLUP);
  pinMode(SKIP_BUTTON, INPUT_PULLUP);
  xTaskCreate(control_listener, "calculate_water_temp", 2048, NULL, 2, NULL);
}

void loop() {
  // display.showNumberDec(56000);
  if(cont){  //start operator
  display.showNumberDec(0);
  if (!isSpeed && !flag4) {
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
    if (!isGameOver(currentfigure, fig, matrix, cl) && cont) {
      for (int col = cl; col < 17 - height + cl; col++) {
        if (!checkObjection(col, currentfigure, matrix, fig) && !isSpeed) {
          // Serial.println(col);
          moveDown(col, currentfigure, matrix, fig, mx);
          currentColumn = col;
          delay(currentSpeed);
        } else
          break;
      }
      if (!isSpeed) {
        saveToMatrix(matrix, currentColumn);
        Serial.println("Ha");
        deleteLine(matrix, &deleteAnimation, height, currentColumn,false);
        speed -= 5;
        currentSpeed = speed;
        rot = 0;
        mv = 0;
        cl = 0;
      } else {
        isSpeed = false;
      }
    } else if (cont) {  // game over
      // mx.clear();
      for (int i = 15; i >= 0; i--) {
        mx.setColumn(mapping[i], 0xFF);
        matrix[i] = 0;
        delay(80);
      }
      for (int i = 0; i < 16; i++) {
        mx.setColumn(mapping[i], 0x0);
        delay(80);
      }
      fig = 0;            // current figure
      rot = 0;            // current rotation
      currentColumn = 0;  // current column
      height = 0;         // current height
      mv = 0;             // current rotation mudslide due to rotation bondaries
      cl = 0;
      speed = 500;
      currentSpeed = 500;
      cont = false;
      mainPointCounter=0;
      // display.showNumberDec(0);
    } 
  }
  } else{
    delay(40);
  }
}
