#include <movementFuncs/MovementFuncs.h>
#include "Constants.h"
#include <helperFuncs/calFunc.h>

void move(int col, int currentfigure[], boolean right,int fig,int matrix[],MD_MAX72XX &mx) {
  int moved[FIGURE_LEN];
  if (right) {
    for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
      moved[i] = currentfigure[i] >> 1;
    }
  } else {
    for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
      moved[i] = currentfigure[i] << 1;
    }
  }
  if (!checkFigureObjection(moved, col,matrix,fig)) {
    int bias = 0;
    for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
      currentfigure[i] = moved[i];
      mx.setColumn(col+bias, moved[i] + matrix[col + bias]);
      bias++;
    }
  }
}

int rotate(int col, int currentFig[],int &mvr,int fig,int &rot,int matrix[],MD_MAX72XX &mx,int &height, int &cl) {  // returns right-left movement factor
  int inRot = rot;
  if (rot < 3) {  //set current rotation
    rot += 1;
  } else {
    rot = 0;
  }

  int horizontalBias = 0;  //calculated horizontal bias to apply
  for (int i = scope[fig][0]; i <= scope[fig][1]; i++) { //calculate horizontal bias
    int inElem = figures[fig][rot][i];
    int holH=0;
    if (mvr > 0) {  
      int elem = figures[fig][rot][i] >> mvr;
      while (countOnes(inElem, 8) != countOnes(elem, 8)) { //check if element not out of borders
        elem = (elem << 1) | 1;  //move figure horizontaly to match borders
        horizontalBias--;
      }
    } else {
      int elem = figures[fig][rot][i] << -mvr;
      // Serial.println(elem);
      while (countOnes(inElem, 8) != countOnes(elem, 8)) {
        elem = elem >> 1;
        horizontalBias++;
      }
    }
  }
  int rotated[FIGURE_LEN]={0};
  for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {  //write correct placed figure to raotated array
    int inElem = figures[fig][rot][i];
    if (mvr > 0) {
      int elem = figures[fig][rot][i] << (-horizontalBias) >> mvr;
      rotated[i]=elem;

    } else {
      int elem = (figures[fig][rot][i] << (-mvr)) >> horizontalBias;
      rotated[i] = elem;
    }
  }
  if (isRotateAble(col, rotated,matrix,fig)) {  //sets rotation result on the screen if figure is rotate able
    int bias = 0;
    for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
      mx.setColumn(col+bias, matrix[col + bias]);
      currentFig[i] = 0x0;

      currentFig[i] = rotated[i];
      mx.setColumn(col+bias, currentFig[i] + matrix[col + bias]);
      bias++;
    }
    height = calculateHeight(currentFig);
    cl = scope[fig][0] - startPos[fig][rot];
  } else {  //don't do any rotation
    rot = inRot;
  }
  // Serial.println(mvr);
  mvr+=horizontalBias;
  return 0;
}

void moveDown(int col,int currentfigure[],int matrix[],int fig,MD_MAX72XX &mx) {
  int bias = 0;
  for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
    if (col + bias >= 0)
      mx.setColumn(col + bias, currentfigure[i] + matrix[col + bias]);
    if (col != 0 && col-1<sizeof(mapping)/sizeof(int)) mx.setColumn(col-1, matrix[col - 1]);
    bias++;
  }
}