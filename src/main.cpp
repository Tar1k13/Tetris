#include <Arduino.h>
#include <SPI.h>
#include <TM1637Display.h>
#include <MD_MAX72xx.h>

TM1637Display display = TM1637Display(25, 26);

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
int scope[7][2]={{1,3},{1,4},{2,3},{1,3},{1,3},{1,3},{1,3}};
const int startPos[7][4] = {{1, 2, 1, 1},   // T
                      {1, 2, 1, 2},   // I
                      {2, 2, 2, 2},   // O
                      {1, 2, 1, 1},   // L
                      {1, 2, 1, 1},   // J
                      {1, 2, 1, 1},   // S
                      {1, 2, 1, 1}};  // Z
int curState = 0;       // current left move
int curState1 = 0;      // current right move
int fig = 1;            // current figure
int rot = 0;            // current rotation
int currentColumn = 0;  // current column
int height = 0;         // current height
int mv = 0;             // current rotation mudslide due to rotation bondaries
int cl=0;
int currentfigure[5];   // current figure
int matrix[17];
int calculateHeight(int fig[]) {
  int counter = 0;
  for (size_t i = 0; i <= sizeof(fig); i++) {
    if (fig[i] != 0) counter++;
  }
  return counter;
  // return center[0];
}

int countOneDigitsInHex(int hexNum) {
  int count = 0;
  for (int i = 0; i < 8; i++) {  // Iterate through 8 digits
    count += (hexNum & 1);       // Add 1 if least significant bit is 1
    hexNum >>= 1;                // Shift number right by 1 bit
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



boolean checkObjection(int col) {
  int bias=0;
  for(int i=scope[fig][0];i<=scope[fig][1];i++){
    int elem=currentfigure[i];
    if ((elem & matrix[col + bias]) != 0) {
      return true;
    }
    bias++;
  }
  return false;
}

boolean checkFigureObjection(int* mvdFig,int col){
  int bias=0;
  for(int i=scope[fig][0];i<=scope[fig][1];i++){
    if ((mvdFig[i] & matrix[col + bias]) != 0) {
      return true;
    }
    bias++;
  }
  return false;
}

boolean checkBoundings(boolean right) {
  if (!right) {
    int bias=0;
      for(int i=scope[fig][0];i<=scope[fig][1];i++){
        int elem=currentfigure[i]<<1;
        if (countOneDigitsInHex(elem) != countOneDigitsInHex(currentfigure[i])) {
          return false;
        }
        bias++;
      }
      return true;
  } else {
        int bias=0;
      for(int i=scope[fig][0];i<=scope[fig][1];i++){
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

void move(int col, int* currentFig , boolean right) {
  int moved[5];
  if(right){
    for(int i=scope[fig][0];i<=scope[fig][1];i++){
      moved[i]=currentfigure[i]>>1;
    }
  } else {
    for(int i=scope[fig][0];i<=scope[fig][1];i++){
      moved[i]=currentfigure[i]<<1;
    }
  }
    if(!checkFigureObjection(moved,col)){
      int bias=0;
      for(int i=scope[fig][0];i<=scope[fig][1];i++){
        currentfigure[i]=moved[i];
        mx.setColumn(mapping[col+bias],moved[i]+matrix[col+bias]);

        bias++;
      }
    }
}

boolean isRotateAble(int col,int rotated[]){
  int bias=0;
  for(int i=scope[fig][0];i<=scope[fig][1];i++){
    if((matrix[col+bias] & rotated[i]) != 0) return false;
    bias++;
  }
  return true;
}
int rotate(int col, int* currentFig,int mvr) { //returns right-left movement factor
  int inRot=rot;
  if (rot < 3){
    rot += 1;
  }
  else{
    rot = 0;
  }

  
  int horizontalBias=0;
  for(int i=scope[fig][0];i<=scope[fig][1];i++){
    int inElem=figures[fig][rot][i];
    if(mvr>0){
      int elem=figures[fig][rot][i] >> mvr;
      while(countOnes(inElem,8)!=countOnes(elem,8)){
        elem=(elem<<1) | 1;
        horizontalBias--;
      }
    } else {
      int elem=figures[fig][rot][i] << -mvr;
      while(countOnes(inElem,8)!=countOnes(elem,8)){
        elem=elem>>1;
        horizontalBias++;
      }

    }
    
  }
  int rotated[5];
  for(int i=scope[fig][0];i<=scope[fig][1];i++){
    int inElem=figures[fig][rot][i];
    if(mv>0){
      int elem=figures[fig][rot][i] >> (mvr);
      int prp=elem<<(horizontalBias*(-1));
      if(countOnes(prp,8)!=countOnes(inElem,8)){
        for(int t=horizontalBias;t<0;t++){
          elem=(elem<<1) | 1;
        }
        rotated[i]=elem;
      } else rotated[i]=prp;
      
    } else{
      int elem=(figures[fig][rot][i] << (mvr*(-1))) >> horizontalBias;
      rotated[i]=elem;
    }
  }
  if(isRotateAble(col,rotated)){
    int bias=0;
    for(int i=0;i<=5;i++){
      mx.setColumn(mapping[col+bias],matrix[col+bias]);
      currentFig[i]=0x0;
      bias++;
    }
    bias=0;
    for(int i=scope[fig][0];i<=scope[fig][1];i++){
      currentFig[i]=rotated[i];
      mx.setColumn(mapping[bias+col],currentFig[i]+matrix[col+bias]);
      bias++;
    }
    height=calculateHeight(currentFig);
    cl=scope[fig][0]-startPos[fig][rot];
  } else{
    rot=inRot;
  }
  return 0;
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
      move(currentColumn, currentfigure, false);
      mv--;
    }
    if (digitalRead(BUTTON_PIN) == HIGH) flag1 = false;

    // right
    if (digitalRead(4) == LOW && !flag2 && checkBoundings(true)) {
      flag2 = true;
      move(currentColumn, currentfigure, true);
      mv++;
    }
    if (digitalRead(4) == HIGH ) flag2 = false;

    // rotate
    if (digitalRead(16) == LOW && !flag3) {
      flag3 = true;
      mv+=rotate(currentColumn, currentfigure,mv);
    }
    if (digitalRead(16) == HIGH) flag3 = false;
    vTaskDelay(50 / portTICK_RATE_MS);
  }
}

void deleteAnimation(int* elemsToDel){
  int ts=2;
  int tx=1;
  for(int t=0;t<16;t++){
    while(elemsToDel[t]!=0){
      int elem=elemsToDel[t]>>ts << tx;
      mx.setColumn(mapping[t],elem);
      ts+=2;
      tx++;
      elemsToDel[t]=elem;
      delay(100);
    }
  }
}

void setup() {
  Serial.begin(9600);
  mx.begin();
  display.setBrightness(7);
  mapn();
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // left button
  pinMode(4, INPUT_PULLUP);           // right button
  pinMode(16, INPUT_PULLUP);
  xTaskCreate(control_listener, "calculate_water_temp", 2048, NULL, 2, NULL);
}

void loop() {
  display.showNumberDec(56000);
  fig=rand() % 7;
  for(int i=0;i<5;i++){
    currentfigure[i]=figures[fig][rot][i];
  }

  height = calculateHeight(currentfigure);
  boolean objection = false;

  int currentCol = 0;
  cl=scope[fig][0]-startPos[fig][rot];
  Serial.println(cl);
  for (int col=cl; col < 17 - height + cl; col++) {
    if(!checkObjection(col)){
      // Serial.println(col);
      int bias=0;
      for(int i=scope[fig][0];i<=scope[fig][1];i++){
        mx.setColumn(mapping[col+bias],currentfigure[i]+matrix[col+bias]);
        if(col!=0 && col>=0) mx.setColumn(mapping[col-1],matrix[col-1]);
        bias++;
      }
      currentColumn=col;
      delay(1500);
    }
  }
  int bias=0;
   for(int i=scope[fig][0];i<=scope[fig][1];i++){
        int elem=currentfigure[i]+matrix[currentColumn+bias];
        matrix[currentColumn+bias]=elem;
        // Serial.println(currentCol+bias);
        bias++;
  }
  int elemToDel[16]={0};
  boolean isDel=false;
  for(int i=0;i<(sizeof(matrix)/sizeof(matrix[0]));i++){
      if(countOnes(matrix[i],8)==8){
        isDel=true;
        elemToDel[i]=matrix[i];
        matrix[i]=0x0;
        for(int t=i;t>0;t--){
          matrix[t]=matrix[t-1];
        }
      }
  }
  if(isDel){
    deleteAnimation(elemToDel);
    for(int i=15;i>=0;i--){
      // Serial.println(mapping[i],matrix[i+1]);
      mx.setColumn(mapping[i],matrix[i]);
    }
  }
  

  // for(int i=0;i<(sizeof(matrix)/sizeof(matrix[0]));i++){
  //   if(countOnes(matrix[i],8)==8){
  //     matrix[i]=0x0;
  //     for(int t=i+1;t>0;t--){
  //       matrix[t]=matrix[t-2];
  //     }
  //   }
  // }
  // for(int i=0;i<(sizeof(matrix)/sizeof(matrix[0]));i++){
  //   Serial.print(matrix[i]);
  //   Serial.print(" ");
  // }
  rot = 0;
  mv = 0;
  cl=0;
}