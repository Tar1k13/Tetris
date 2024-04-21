#include <snake/snake.h>
#include <MD_MAX72xx.h>
#include <cppQueue.h>
#include <Arduino.h>
#include "Constants.h"


int RIGHT=0;
int BOTTOM=1;
int LEFT=2;
int TOP=3;
int currentDirection=RIGHT;
int deleteDirection=RIGHT;

typedef struct pos{
  int x;
  int y;
} cornerPosition;
cppQueue cornerQueue(sizeof(cornerPosition),10,FIFO);
cppQueue removeDirQueue(sizeof(int),10,FIFO);


bool fl,fl1,fl2;
// bool ml=false,mr=true,mt=false,mb=false;
// bool mlt,mrt=true,mtr,mbr;
int posX=4,posY=-1;
int tailX=4,tailY=-6;
int turnX=-1,turnY=-1;
void test_control(void *pvParameters){
  while (1){
    if (digitalRead(LEFT_BUTTON) == LOW && !fl && currentDirection!=TOP){ //bottom
      fl=true;
      currentDirection=BOTTOM;
      cornerPosition pos={posX,posY};
      cornerQueue.push(&pos);
      removeDirQueue.push(&BOTTOM);
    }
    if(fl && digitalRead(LEFT_BUTTON) == HIGH){
      fl=false;
    }

    if (digitalRead(RIGHT_BUTTON)==LOW && !fl1 && currentDirection!=RIGHT){ //left
      fl1=true;
      currentDirection=LEFT;
      cornerPosition pos={posX,posY};
      cornerQueue.push(&pos);
      removeDirQueue.push(&LEFT);
    }
    if(fl1 && digitalRead(RIGHT_BUTTON) == HIGH){
      fl1=false;
    }

    if (digitalRead(ROTATE_BUTTON)==LOW && !fl2 && currentDirection!=BOTTOM){ //top
      fl2=true;
      currentDirection=TOP;
      cornerPosition pos={posX,posY};
      cornerQueue.push(&pos);
      removeDirQueue.push(&TOP);
    }
    if(fl2 && digitalRead(ROTATE_BUTTON) == HIGH){
      fl2=false;
    }
    vTaskDelay(20/portTICK_RATE_MS);
  }
  
}

void snake(void *pvParameters){
    SnakeData *data=(SnakeData*)pvParameters;
    MD_MAX72XX mx=data->dsp;
    TaskHandle_t xSnakeHandle=data->handle;
  while (1){
    if(currentDirection==RIGHT && posY+1<=15){  //right
      mx.setPoint(posX,++posY,true);
    }else if(currentDirection==BOTTOM && posX-1>=0){  //bottom
      mx.setPoint(--posX,posY,true);
    } else if(currentDirection==LEFT && posY-1>=0){ //left
      mx.setPoint(posX,--posY,true);
    } else if(currentDirection==TOP && posX+1<=7){  //top
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

    //size increase
    if(posY==6 && posX==4){
      int tailDirection=0;
      removeDirQueue.peek(&tailDirection);
      if(tailDirection==LEFT || tailDirection==RIGHT){
        --tailY;
      }else{
        --tailX;
      }
      
    }

    cornerPosition ps;
    cornerQueue.peek(&ps);
    if(ps.x==tailX && ps.y==tailY){
      int newDelDir;
      removeDirQueue.pop(&newDelDir);
      cornerPosition pd;
      cornerQueue.pop(&pd);
      deleteDirection=newDelDir;
      
    }
    // Serial.println(deleteDirection);
    if(deleteDirection==RIGHT){
      mx.setPoint(tailX,++tailY,false);
    }else if(deleteDirection==BOTTOM){
      mx.setPoint(--tailX,tailY,false);
    } else if(deleteDirection==LEFT){
      mx.setPoint(tailX,--tailY,false);
    } 
    else if(deleteDirection==TOP){
      mx.setPoint(++tailX,tailY,false);
    }
    vTaskDelay(1000/portTICK_RATE_MS);
  };
  
};