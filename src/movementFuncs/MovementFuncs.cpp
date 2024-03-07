#include <movementFuncs/MovementFuncs.h>
#include "Constants.h"
#include <helperFuncs/calFunc.h>

void move(int col, int* currentfigure, boolean right,int fig,int matrix[],MD_MAX72XX &mx) {
  int moved[5];
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
      mx.setColumn(mapping[col + bias], moved[i] + matrix[col + bias]);

      bias++;
    }
  }
}

int rotate(int col, int* currentFig,int mvr,int fig,int &rot,int matrix[],MD_MAX72XX &mx,int &height, int &cl) {  // returns right-left movement factor
  int inRot = rot;
  if (rot < 3) {
    rot += 1;
  } else {
    rot = 0;
  }

  int horizontalBias = 0;
  for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
    int inElem = figures[fig][rot][i];
    if (mvr > 0) {
      int elem = figures[fig][rot][i] >> mvr;
      while (countOnes(inElem, 8) != countOnes(elem, 8)) {
        elem = (elem << 1) | 1;
        horizontalBias--;
      }
    } else {
      int elem = figures[fig][rot][i] << -mvr;
      while (countOnes(inElem, 8) != countOnes(elem, 8)) {
        elem = elem >> 1;
        horizontalBias++;
      }
    }
  }
  int rotated[5];
  for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
    int inElem = figures[fig][rot][i];
    if (mvr > 0) {
      int elem = figures[fig][rot][i] >> (mvr);
      int prp = elem << (horizontalBias * (-1));
      if (countOnes(prp, 8) != countOnes(inElem, 8)) {
        for (int t = horizontalBias; t < 0; t++) {
          elem = (elem << 1) | 1;
        }
        rotated[i] = elem;
      } else
        rotated[i] = prp;

    } else {
      int elem = (figures[fig][rot][i] << (mvr * (-1))) >> horizontalBias;
      rotated[i] = elem;
    }
  }
  if (isRotateAble(col, rotated,matrix,fig)) {
    int bias = 0;
    for (int i = 0; i <= 5; i++) {
      mx.setColumn(mapping[col + bias], matrix[col + bias]);
      currentFig[i] = 0x0;
      bias++;
    }
    bias = 0;
    for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
      currentFig[i] = rotated[i];
      mx.setColumn(mapping[bias + col], currentFig[i] + matrix[col + bias]);
      bias++;
    }
    height = calculateHeight(currentFig);
    cl = scope[fig][0] - startPos[fig][rot];
  } else {
    rot = inRot;
  }
  return 0;
}

void moveDown(int col,int currentfigure[],int matrix[],int fig,MD_MAX72XX &mx) {
  int bias = 0;
  for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
    if (col + bias >= 0)
      mx.setColumn(mapping[col + bias], currentfigure[i] + matrix[col + bias]);
    if (col != 0 && col > 0) mx.setColumn(mapping[col - 1], matrix[col - 1]);
    bias++;
  }
}