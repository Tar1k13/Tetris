#include <snake/snake.h>
#include <MD_MAX72xx.h>
#include <cppQueue.h>
#include <Arduino.h>
#include "Constants.h"


const int RIGHT=4;
const int BOTTOM=1;
const int LEFT=2;
const int TOP=3;
int currentDirectionPrsd=RIGHT;
int deleteDirection=RIGHT;

typedef struct pos{
  int x;
  int y;
} cornerPosition;
cppQueue cornerQueue(sizeof(cornerPosition),20,FIFO);
cppQueue removeDirQueue(sizeof(int),20,FIFO);

bool fl,fl1,fl2,fl3;
// bool ml=false,mr=true,mt=false,mb=false;
// bool mlt,mrt=true,mtr,mbr;
int posX=4,posY=-1;
int tailX=4,tailY=-2;
int turnX=-1,turnY=-1;
bool permited=false;


void test_control(void *pvParameters){
  // cornerQueue.push(&RIGHT);
  // removeDirQueue.push(&RIGHT);
  while (1){
    vTaskDelay(50/portTICK_RATE_MS);
    if (digitalRead(LEFT_BUTTON) == LOW && !fl && currentDirectionPrsd!=TOP){ //bottom
      fl=true;
      currentDirectionPrsd=BOTTOM;
      // cornerPosition pos={posX,posY};
      // cornerQueue.push(&pos);
      // removeDirQueue.push(&BOTTOM);
    }
    if(fl && digitalRead(LEFT_BUTTON) == HIGH){
      fl=false;
    }

    if (digitalRead(RIGHT_BUTTON)==LOW && !fl1 && currentDirectionPrsd!=RIGHT){ //left
  
      fl1=true;
      currentDirectionPrsd=LEFT;
      // cornerPosition pos={posX,posY};
      // cornerQueue.push(&pos);
      // removeDirQueue.push(&LEFT);
    }
    if(fl1 && digitalRead(RIGHT_BUTTON) == HIGH){
      fl1=false;
    }

    if (digitalRead(ROTATE_BUTTON)==LOW && !fl2 && currentDirectionPrsd!=BOTTOM){ //top
      fl2=true;
      currentDirectionPrsd=TOP;
      // cornerPosition pos={posX,posY};
      // cornerQueue.push(&pos);
      // removeDirQueue.push(&TOP);
    }
    if(fl2 && digitalRead(ROTATE_BUTTON) == HIGH){
      fl2=false;
    }

     if (digitalRead(SPEED_BUTTON)==LOW && !fl3 && currentDirectionPrsd!=LEFT){ //tight
      fl3=true;
      currentDirectionPrsd=RIGHT;
      // cornerPosition pos={posX,posY};
      // cornerQueue.push(&pos);
    
        // removeDirQueue.push(&RIGHT);
    }
    if(fl3 && digitalRead(SPEED_BUTTON) == HIGH){
      fl3=false;
    }
    vTaskDelay(20/portTICK_RATE_MS);
  }
  
}

uint8_t mtrx[16][8];
int foodX=4,foodY=12;
bool food;
int previousDirection=RIGHT;
long startTime=0;
long speed_s=1000;

// cornerPosition foodPos={-1,-1};
void snake(void *pvParameters){
    TaskHandle_t xSnakeHandle=*(TaskHandle_t*)pvParameters;
    startTime=xTaskGetTickCount();
    
  while (1){
    
    int currentDirection=0;
    if(currentDirectionPrsd!=previousDirection){
      cornerPosition pos={posX,posY};
      cornerQueue.push(&pos);
      switch(currentDirectionPrsd){
        case RIGHT:
          removeDirQueue.push(&RIGHT);
          break;
        case LEFT:
          removeDirQueue.push(&LEFT);
          break;
        case TOP:
          removeDirQueue.push(&TOP);
          break;
        case BOTTOM:
          removeDirQueue.push(&BOTTOM);
          break;
        default:
          break;
      }
      previousDirection=currentDirectionPrsd;
      currentDirection=currentDirectionPrsd;
    } else{
      currentDirection=previousDirection;
    }
    cornerPosition ps;
    cornerQueue.peek(&ps);
    if(ps.x==tailX && ps.y==tailY){
      int newDelDir=0;
      removeDirQueue.pop(&newDelDir);
      cornerPosition pd;
      cornerQueue.pop(&pd);
      deleteDirection=newDelDir;
      
    }
    if(!food){
        food=true;
        foodX=random(8);
        foodY=random(16);
        while(mtrx[foodX][foodY]!=0){
            foodX=random(8);
            foodY=random(16);
        }
        mx.setPoint(foodX,foodY,true);
        
    }


    if(currentDirection==RIGHT && posY+1<=15 && mtrx[posY+1][posX]==0){  //right
      mx.setPoint(posX,++posY,true);
    }else if(currentDirection==BOTTOM && posX-1>=0 && mtrx[posY][posX-1]==0){  //bottom
      mx.setPoint(--posX,posY,true);
    } else if(currentDirection==LEFT && posY-1>=0 && mtrx[posY-1][posX]==0){ //left
      mx.setPoint(posX,--posY,true);
    } else if(currentDirection==TOP && posX+1<=7 && mtrx[posY][posX+1]==0){  //top
      mx.setPoint(++posX,posY,true);
    } else {
      Serial.println("GAME OVER");
      uint8_t buffer[16];
      for (size_t i = 0; i < 16; i++){
        mx.getBuffer(i,8,&buffer[i]);
      }
      for (size_t i = 0; i < 4; i++){
        mx.clear();
        vTaskDelay(500/portTICK_RATE_MS);
        for (size_t i = 0; i < 16; i++){
          mx.setColumn(i,buffer[i]);
        }
        vTaskDelay(500/portTICK_RATE_MS);
      }
      mx.clear();
      vTaskDelete(xSnakeHandle);
    
    }
    mtrx[posY][posX]=1;

        if(posY==foodY && posX==foodX){
      Serial.println("FOOD");
      switch(deleteDirection){
        case RIGHT:
          tailY--;
          break;
        case LEFT:
          tailY++;
          break;
        case TOP:
          tailX--;
          break;
        case BOTTOM:
          tailX++;
          break;
        default:
          break;
      }
      food=false;
    }
    // cornerPosition ps;
    // cornerQueue.peek(&ps);
    // if(ps.x==tailX && ps.y==tailY){
    //   int newDelDir=0;
    //   removeDirQueue.pop(&newDelDir);
    //   cornerPosition pd;
    //   cornerQueue.pop(&pd);
    //   deleteDirection=newDelDir;
      
    // }
    // Serial.println(deleteDirection);
    Serial.println(deleteDirection);
    if(deleteDirection==RIGHT){
      mx.setPoint(tailX,++tailY,false);
    }else if(deleteDirection==BOTTOM){
      // Serial.println(tailX);
      mx.setPoint(--tailX,tailY,false);
      
    } else if(deleteDirection==LEFT){
      mx.setPoint(tailX,--tailY,false);
    } 
    else if(deleteDirection==TOP){
      mx.setPoint(++tailX,tailY,false);
    }
    
    mtrx[tailY][tailX]=0;


    vTaskDelay(speed_s/portTICK_RATE_MS);
    if(xTaskGetTickCount()-startTime>5000){
      startTime=xTaskGetTickCount();
      speed_s-=10;
  
    } 
  };
  
};