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
// int buttonStateLeft = 0;
// int buttonStateRight = 0;
// int buttonStateFlip = 0;

int figures[7][4][5] = {{{0x0, 0x10, 0x18, 0x10, 0x0},  // T
                         {0x0, 0x0, 0x38, 0x10, 0x0},
                         {0x0, 0x10, 0x30, 0x10, 0x0},
                         {0x0, 0x10, 0x38, 0x0, 0x0}},
                        {{0x0, 0x8, 0x8, 0x8, 0x8},  // I
                         {0x0, 0x0, 0x3C, 0x0, 0x0},
                         {0x0, 0x8, 0x8, 0x8, 0x8},
                         {0x0, 0x0, 0x3C, 0x0, 0x0}},
                        {{0x0, 0x0, 0x18, 0x18, 0x0},  // O
                         {0x0, 0x0, 0x18, 0x18, 0x0},
                         {0x0, 0x0, 0x18, 0x18, 0x0},
                         {0x0, 0x0, 0x18, 0x18, 0x0}},
                        {{0x0, 0xC, 0x8, 0x8, 0x0},  // L
                         {0x0, 0x0, 0x1C, 0x4, 0x0},
                         {0x0, 0x8, 0x8, 0x18, 0x0},
                         {0x0, 0x10, 0x1C, 0x0, 0x0}},
                        {{0x0, 0x8, 0x8, 0xC, 0x0},  // J
                         {0x0, 0x0, 0x1C, 0x10, 0x0},
                         {0x0, 0x18, 0x8, 0x8, 0x0},
                         {0x0, 0x4, 0x1C, 0x0, 0x0}},
                        {{0x0, 0x8, 0xC, 0x4, 0x0},  // S
                         {0x0, 0x0, 0xC, 0x18, 0x0},
                         {0x0, 0x10, 0x18, 0x8, 0x0},
                         {0x0, 0xC, 0x18, 0x0, 0x0}},
                        {{0x0, 0x4, 0xC, 0x8, 0x0},  // Z
                         {0x0, 0x0, 0x18, 0xC, 0x0},
                         {0x0, 0x8, 0x18, 0x10, 0x0},
                         {0x0, 0x18, 0xC, 0x0, 0x0}}};

int startPos[7][4] = {{1, 2, 1, 1},   // T
                      {1, 2, 1, 2},   // I
                      {2, 2, 2, 2},   // O
                      {1, 2, 1, 1},   // L
                      {1, 2, 1, 1},   // J
                      {1, 2, 1, 1},   // S
                      {1, 2, 1, 1}};  // Z

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

int curState = 0;       // current left move
int curState1 = 0;      // current right move
int fig = 3;            // current figure
int rot = 0;            // current rotation
int currentColumn = 0;  // current column
int height = 0;         // current height
int mv = 0;             // current rotation mudslide due to rotation bondaries
int ml = 0;
int currentfigure[5];

boolean checkObjection(int col) {
  int bias=0;
  for(int i=startPos[fig][rot];i<startPos[fig][rot]+height;i++){
    int elem=currentfigure[i];
    if ((elem & matrix[col + bias]) != 0) {
      Serial.println("OBJECTION");
      return true;
    }
    bias++;
  }
  return false;
}

boolean checkBoundings(boolean right) {
  if (!right) {
    int bias=0;
      for(int i=startPos[fig][rot];i<startPos[fig][rot]+height;i++){
        int elem=currentfigure[i]<<1;
        if (countOneDigitsInHex(elem) != countOneDigitsInHex(currentfigure[i])) {
          return false;
        }
        bias++;
      }
      return true;
    // for (int t = 0; t < height; t++) {
    //   int elem = figures[fig][rot][startPos[fig][rot] + t] << curState + 1 >>
    //              curState1 >> mv;
    //   int curElem = figures[fig][rot][startPos[fig][rot] + t] << curState >>
    //                 curState1 >> mv;
    //   if (countOneDigitsInHex(elem) != countOneDigitsInHex(curElem)) {
    //     return false;
    //   }
    // }
    // return true;
  } else {
        int bias=0;
      for(int i=startPos[fig][rot];i<startPos[fig][rot]+height;i++){
        int elem=currentfigure[i]>>1;
        if (countOneDigitsInHex(elem) != countOneDigitsInHex(currentfigure[i])) {
          return false;
        }
        bias++;
      }
      return true;
  }
  return true;
}

void move(int col, int prevStep, int nextStep, boolean right) {
  if(right){
    Serial.println("HER");

    if(!checkObjection(col)){
       int bias=0;
      for(int i=startPos[fig][rot];i<startPos[fig][rot]+height;i++){
        currentfigure[i]=(currentfigure[i]>>1);
        int elem=currentfigure[i]+matrix[col+bias];
        mx.setColumn(mapping[col + bias], elem);
        bias++;
      }
    }
  } else {
    int bias=0;
    if(!checkObjection(col)){
      for(int i=startPos[fig][rot];i<startPos[fig][rot]+height;i++){
        currentfigure[i]=currentfigure[i]<<1;
        int elem=currentfigure[i]+matrix[col+bias];
        mx.setColumn(mapping[col + bias], elem);
        bias++;
      }
    }
  }
  // if (right)
  //   curState1 = nextStep;
  // else
  //   curState = nextStep;
  // if (!checkObjection(col, height)) {
  //   for (int t = 0; t < height; t++) {
  //     int elem = ((figures[fig][rot][startPos[fig][rot] + t] << curState >>
  //                  curState1 >> mv) +
  //                 matrix[col + t]);
  //     // safe put
  //     while (countOnes(elem, 8) < countOnes(elem, 12)) {
  //       elem = (elem >> 1) + matrix[col + t];
  //     }

  //     mx.setColumn(mapping[col + t], elem);
  //   }
  // } else {
  //   Serial.println("HER");
  //   if (right)
  //     curState1 = prevStep;
  //   else
  //     curState = prevStep;
  // }
}

// boolean checkRotateObjection(int col,int fg,int rt){
//     for (int t = 0; t < height; t++) {
//     int elem = figures[fg][rt][startPos[fg][rt] + t] << curState >>
//                curState1 >> mv;
//     if ((elem & matrix[col + t]) != 0) {
//       Serial.println("ROTATEOBJECTION");
//       return true;
//     }
//   }
//   return false;
// }

boolean isRotateAble(int rotated,int input){
  if((rotated & input)!=0)
    return false;
  return true;
}
int rotate(int col, int heigh,int mvr) { //returns right-left movement factor
  int bias=0;
  for(int i=startPos[fig][rot];i<startPos[fig][rot]+height;i++){
    mx.setColumn(mapping[col+bias],matrix[col+bias]);
    currentfigure[i]=0x0;
    bias++;
  }
  
  if (rot < 3){
    rot += 1;
  }
  else{
    rot = 0;
  }
  //put
  height=calculateHeight(figures[fig][rot]);
  int sm=0; //required to bound bias due to limit rotate
  for(int i=startPos[fig][rot];i<startPos[fig][rot]+height;i++){
    int inElem=figures[fig][rot][i];
    if(mv>=0){
      int elem=figures[fig][rot][i] >> (mvr);
      for(int t=sm;t<0;t++){
        elem=(elem<<1) | 1;
      }
      while(countOnes(inElem,8)!=countOnes(elem,8)){
        elem=(elem<<1) | 1;
        sm--;
      }
      currentfigure[i]=elem;
    }
    else{
      int elem=(figures[fig][rot][i] << (mvr*(-1)));

    //   Serial.println(elem);
      while(countOnes(inElem,8)!=countOnes(elem,8)){
        elem=elem>>1;
        sm++;
      }
      currentfigure[i]=(figures[fig][rot][i] << (mvr*(-1))) >> sm;
    // }
      
 
  }
  }
  bias=0;
  Serial.println(startPos[0][0]);
  for(int i=startPos[fig][rot];i<startPos[fig][rot]+height;i++){
    int elem=currentfigure[i]+matrix[col+bias];
    // Serial.println(currentfigure[i]);
    mx.setColumn(mapping[col+bias],elem);
    bias++;
  }
  return sm;
}

// one-click flags
boolean flag1 = false;
boolean flag2 = false;
boolean flag3 = false;

// input listener

void control_listener(void *pvParameters) {

  while (true) {
    int height = calculateHeight(figures[fig][rot]);

    // left
    if (digitalRead(BUTTON_PIN) == LOW &&
        !flag1 && checkBoundings(false)) {
      flag1 = true;
      move(currentColumn, curState, curState + 1, false);
      mv--;
    }
    if (digitalRead(BUTTON_PIN) == HIGH) flag1 = false;

    // right
    if (digitalRead(4) == LOW && !flag2 && checkBoundings(true)) {
      flag2 = true;
      move(currentColumn, curState1, curState1 + 1, true);
      mv++;
    }
    if (digitalRead(4) == HIGH ) flag2 = false;

    // rotate
    if (digitalRead(16) == LOW && !flag3) {
      flag3 = true;
      mv+=rotate(currentColumn, height,mv);
    }
    if (digitalRead(16) == HIGH) flag3 = false;
    vTaskDelay(50 / portTICK_RATE_MS);
  }
}

void setup() {
  Serial.begin(9600);
  mx.begin();
  tm.begin();
  mapn();
  // for(int i=0;i<5;i++){
  //   currentfigure[i]=figures[fig][rot][i];
  // }
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // left button
  pinMode(4, INPUT_PULLUP);           // right button
  pinMode(16, INPUT_PULLUP);
  xTaskCreate(control_listener, "calculate_water_temp", 2048, NULL, 2, NULL);
}

void loop() {
  // fig=rand() % 7;
  // rot=rand() % 3;

  for(int i=0;i<5;i++){
    currentfigure[i]=figures[fig][rot][i];
  }

  // int width=countOnesInHexList(figures[fig][rot],5);

  height = calculateHeight(currentfigure);
  boolean objection = false;

  int rotateBias = 0;
  int currentCol = 0;
  
  for (size_t col = 0; col < 17 - height; col++) {
    currentColumn = col;
    // objection = checkObjection(col, height);

    if (!checkObjection(col)) {
      int bias=0;
      for(int i=startPos[fig][rot];i<startPos[fig][rot]+height;i++){
        int elem=currentfigure[i]+matrix[col+bias];
        mx.setColumn(mapping[col+bias],elem);
        if(col!=0) mx.setColumn(mapping[col - 1], matrix[col - 1]);
        bias++;
      }
      // for (int t = 0; t < height; t++) {
      //   int elem = ((figures[fig][rot][startPos[fig][rot] + t] << curState >>
      //                curState1 >> mv) +
      //               matrix[col + t]);
      //   // Serial.println(elem);
      //   mx.setColumn(mapping[col + t], elem);                           // put
      //   if (col != 0) mx.setColumn(mapping[col - 1], matrix[col - 1]);  // move
      // }
      // Serial.println();
    // } 
    // else
    //   break;
    currentCol = col;

    delay(1000);
  }
  }
  int bias=0;
   for(int i=startPos[fig][rot];i<startPos[fig][rot]+height;i++){
        int elem=currentfigure[i]+matrix[currentCol+bias];
        matrix[currentCol+bias]=elem;
        Serial.println(currentCol+bias);
        bias++;
  }

  // for (int t = 0; t < height; t++) {
  //   int sum = (figures[fig][rot][startPos[fig][rot] + t] << curState >>
  //              curState1 >> mv) +
  //             matrix[currentCol + t];
  //   while (countOnes(sum, 8) < countOnes(sum, 12)) {
  //     sum = (sum >> 1) + matrix[currentCol + t];
  //   }
  //   matrix[currentCol + t] = sum;
  // }
  //delete line
  // for(int i=0;i<(sizeof(matrix)/sizeof(matrix[0]));i++){
  //   if(countOnes(matrix[i],8)==8){
  //     //animation
  //     int ts=matrix[i];
  //     for(int t=2;t<=4;t++){
  //       ts=ts>>t;
  //       ts=ts<<t-1;
  //       // ts=ts<<i+1;
  //       mx.setColumn(mapping[i],ts);
  //       delay(500);
  //     }
  //     matrix[i]=0;
  //     for(int t=i;t>0;t--){
  //       matrix[t]=matrix[t-1];
  //     }
  //   };
  // }
  // // //re-render
  // for(int i=0;i<(sizeof(matrix)/sizeof(matrix[0]))-1;i++){
  //   mx.setColumn(mapping[i],matrix[i]);
  // }
  // Serial.println();
  // curState = 0;
  // curState1 = 0;
  rot = 0;
  mv = 0;
}