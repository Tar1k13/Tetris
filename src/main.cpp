#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <TM1637.h>


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


int figures[2][4][5] = {{{0x0, 0x0, 0x38, 0x10, 0x0},
                         {0x0, 0x10, 0x30, 0x10, 0x0},
                         {0x0, 0x0, 0x10, 0x38, 0x0},
                         {0x0, 0x10, 0x38, 0x0, 0x0}},
                        {{0x0, 0x4, 0x4, 0x4, 0x4},
                         {0x0, 0x0, 0x3C, 0x0, 0x0},
                         {0x0, 0x4, 0x4, 0x4, 0x4},
                         {0x0, 0x0, 0x3C, 0x0, 0x0}}};

int startPos[2][4] = {{1, 1, 2, 1}, {1, 2, 1, 2}};

int matrix[17];
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




int curState = 0;     // current left move
int curState1 = 0;    // current right move
int fig = 1;          // current figure
int rot = 0;          // current rotation
int currentColumn=0;  // current column
int height=0;         // current height
int mv=0;             // current rotation mudslide due to rotation bondaries

boolean checkObjection(int col,int height){
    for (int t = 0; t < height; t++) {
      int elem = figures[fig][rot][startPos[fig][rot] + t] << curState >> curState1 >> mv;
      if ((elem & matrix[col + t]) != 0) {
        Serial.println("OBJECTION");
        return true;
      }
    }
    return false;
}

boolean checkBoundings(int height, boolean right) {
  if (!right) {
    for (int t = 0; t < height; t++) {
      int elem = figures[fig][rot][startPos[fig][rot] + t] << curState + 1 >>curState1 >> mv;
      int curElem = figures[fig][rot][startPos[fig][rot] + t] << curState >>curState1 >> mv;
      if (countOneDigitsInHex(elem) != countOneDigitsInHex(curElem)) {
        return false;
      }
    }
    return true;
  } else {
    for (int t = 0; t < height; t++) {
      int elem = figures[fig][rot][startPos[fig][rot] + t]<< curState >> (curState1+1) >> mv;
      int curElem = figures[fig][rot][startPos[fig][rot] + t] << curState >> curState1 >> mv;
      curElem=curElem>>mv;
      Serial.println(elem);
      if (elem==0 || (countOneDigitsInHex(elem) != countOneDigitsInHex(curElem))) {
        return false;
      }
    }
    return true;
  }
  return true;
}

void move(int col,int height, int prevStep, int nextStep,boolean right){
  if(right) curState1=nextStep;
  else curState=nextStep;
  if(!checkObjection(col,height)){
    for (int t = 0; t < height; t++) {
      int elem =
          ((figures[fig][rot][startPos[fig][rot] + t] << curState >> curState1 >> mv) +
          matrix[col + t]);
      // safe put
      while (countOnes(elem, 8) < countOnes(elem, 12)) {
        elem = (elem >> 1) + matrix[col + t];
      }

      mx.setColumn(mapping[col + t], elem);
    }
  } else{
    Serial.println("HER");
    if(right) curState1=prevStep;
    else curState=prevStep;
  }
}


void rotate(int col,int heigh){
  for (int t = 0; t < heigh; t++) {
    //clear
    mx.setColumn(mapping[col+t],matrix[col+t]);
  }
  rot+=1;
  int newHeight=calculateHeight(figures[fig][rot]);
  height=newHeight;
  for (int t = 0; t < newHeight; t++) {
    //put
    int elem = ((figures[fig][rot][startPos[fig][rot] + t] << curState >>
                curState1 >> mv) +
                matrix[col + t]);
        while (countOnes(elem, 8) < countOnes(elem, 12)) {
          elem = (elem >> 1) + matrix[col+t];
          mv++;
          // rotateBias++;
        }

    mx.setColumn(mapping[col+t],elem);
  }
}

// one-click flags
boolean flag1=false;
boolean flag2=false;
boolean flag3=false;

//input listener
void control_listener(void *pvParameters) {
  while (true) {
    int height = calculateHeight(figures[fig][rot]);

    //left
    if (digitalRead(BUTTON_PIN) == LOW && checkBoundings(height,false) && !flag1){
      flag1=true;
      move(currentColumn,height,curState,curState+1,false);
    }
    if(digitalRead(BUTTON_PIN)== HIGH) flag1=false;

    //right
    if (digitalRead(4) == LOW && checkBoundings(height,true) && !flag2){
      flag2=true;
      move(currentColumn,height,curState1,curState1+1,true);
    }
    if(digitalRead(4)== HIGH) flag2=false;
    
    //rotate
    if (digitalRead(16) == LOW && !flag3){
      flag3=true;
      rotate(currentColumn,height);
    }
    if(digitalRead(16) == HIGH) flag3=false;
    vTaskDelay(50 / portTICK_RATE_MS);
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


void loop() {


  boolean objectionX = false;
  int heightX = calculateHeight(figures[fig][rot]);
  // int width=countOnesInHexList(figures[fig][rot],5);

  height = calculateHeight(figures[fig][rot]);
  boolean objection = false;

  int rotateBias=0;
  int currentCol=0;

  for (size_t col = 0; col < 17 - height; col++) {
    Serial.println(col);
    currentColumn=col;
    objection=checkObjection(col,height);

    if (!objection) {
      for (int t = 0; t < height; t++) {
        int elem = ((figures[fig][rot][startPos[fig][rot] + t] << curState >>
                    curState1 >> mv) +
                   matrix[col + t]);
        mx.setColumn(mapping[col + t], elem);                           // put
        if (col != 0) mx.setColumn(mapping[col - 1], matrix[col - 1]);  // move
      }
    } else
      break;
    currentCol = col;

    delay(1000);
  }
  for (int t = 0; t < height; t++) {
    int sum =
        (figures[fig][rot][startPos[fig][rot] + t] << curState >> curState1 >> mv) +
        matrix[currentCol + t];
    while (countOnes(sum, 8) < countOnes(sum, 12)) {
      sum = (sum >> 1) + matrix[currentCol + t];
    }
    matrix[currentCol + t] = sum;
  }
  curState=0;
  curState1=0;
  rot=0;
  mv=0;
}

//problem with objection(round case)