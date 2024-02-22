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
void mapn(){
  int counter=8;
  for (size_t i = 0; i < 8; i++){
    mapping[i]=counter++;
  }
  counter=1;
  for (size_t i = 9; i < 16; i++){
    mapping[i]=counter++;
  }
}
int buttonState=0;
void control_listener(void *pvParameters){
  while (true){
    if(digitalRead(BUTTON_PIN)==LOW)
      buttonState=1;
    vTaskDelay(100 / portTICK_RATE_MS);
  }
  
}
void setup()
{
  Serial.begin(9600);
  mx.begin();
  tm.begin();
  mapn();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  xTaskCreate(control_listener, "calculate_water_temp", 2048, NULL, 2, NULL);
}


int figures[2][4][5] = {
  {
    {0x0,0x0,0x1C,0x8,0x0},{0x0,0x8,0xC,0x8,0x0},{0x0,0x0,0x8,0x1C,0x0},{0x0,0x8,0x18,0x8,0x0}
  },
  {
    {0x0,0x4,0x4,0x4,0x4},{0x0,0x0,0x3C,0x0,0x0},{0x0,0x4,0x4,0x4,0x4},{0x0,0x0,0x3C,0x0,0x0}
  }
};

int startPos[2][4]={
  {
    2,1,2,1
  },
  {
    1,2,1,2
  }
};




int matrix[16];
int calculateHeight(int fig[]){
  int counter=0;
  for (size_t i = 0; i < sizeof(fig); i++){
    if(fig[i]!=0) counter++;
  }
  return counter;
}
int tst=0;
void loop(){
  int fig=0;
  int rot =0;
  boolean objectionX=false;
  int heightX=calculateHeight(figures[fig][rot]);


  int height=calculateHeight(figures[fig][rot]);
  int currentCol=0;
  boolean objection=false;
    int curState=0;
    for (size_t col = 0; col < 17-height; col++){
      
      if(buttonState==1) curState+=1;
          for(int t=0;t<height;t++){
            if((figures[fig][rot][startPos[fig][rot]+t]<<curState & matrix[col+t]) != 0){
              Serial.println("OBJECTION");
              // Serial.println()
              objection=true;
              break;
            }
          }
        //moving blocks
        if(!objection){
            for(int t=0;t<height;t++){
              int sum=(figures[fig][rot][startPos[fig][rot]+t]<<curState)+matrix[col+t]; //safe put
              // if(mapping[col+t]!=8)
              // Serial.println("col:");
              // Serial.println(mapping[col+t]);
              // Serial.println("val:");
              // Serial.println(sum);
                mx.setColumn(mapping[col+t],sum); //put
              // Serial.println(sum);
              // Serial.println(mapping[col-1]);
              Serial.println(col);
              if(col!=0)
                mx.setColumn(mapping[col-1],matrix[col-1]); //move
            }
        } else break;
      currentCol=col;

      buttonState=0;
      delay(1000);
  }
  for(int t=0;t<height;t++){
    int sum=(figures[fig][rot][startPos[fig][rot]+t]<<curState)+matrix[currentCol+t];
    matrix[currentCol+t]=sum;
    // Serial.println(matrix[currentCol-t]);
  }
  Serial.println();
  for(int t=0;t<16;t++){
    
    Serial.println(matrix[t]);
    
    // Serial.println(matrix[currentCol-t]);
  }
  Serial.println();
}
