#include <helperFuncs/calFunc.h>
#include "Constants.h"
// #include <Constants.h>

int calculateHeight(int fig[]){
    int counter = 0;
    for (size_t i = 0; i <= sizeof(fig); i++) {
        if (fig[i] != 0) counter++;
    }
    return counter;
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

boolean checkObjection(int col,int currentfigure[],int matrix[],int fig) {
  int bias = 0;
  for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
    int elem = currentfigure[i];
    if ((elem & matrix[col + bias]) != 0) {
      return true;
    }
    bias++;
  }
  return false;
}

boolean checkFigureObjection(int* mvdFig, int col,int matrix[],int fig) {
  int bias = 0;
  for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
    if ((mvdFig[i] & matrix[col + bias]) != 0) {
      return true;
    }
    bias++;
  }
  return false;
}

boolean checkBoundings(boolean right,int currentfigure[],int fig) {
  if (!right) {
    int bias = 0;
    for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
      int elem = currentfigure[i] << 1;
      if (countOnes(elem,8) != countOnes(currentfigure[i],8)) {
        return false;
      }
      bias++;
    }
    return true;
  } else {
    int bias = 0;
    for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
      int elem = currentfigure[i] >> 1;
      if (countOnes(elem,8) != countOnes(currentfigure[i],8)) {
        return false;
      }
      bias++;
    }
    return true;
  }
  return true;
}

boolean isRotateAble(int col, int rotated[], int matrix[],int fig) {
  int bias = 0;
  for (int i = scope[fig][0]; i <= scope[fig][1]; i++) {
    if ((matrix[col + bias] & rotated[i]) != 0) return false;
    bias++;
  }
  return true;
}

void deleteAnimation(int elemsToDel[], int col, int heigh,boolean isRts,MD_MAX72XX &mx) {
  int ts = 2;
  int tx = 1;
  for (int t = 0; t < 16; t++) {
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

boolean isGameOver(int currentfigure[],int fig,int matrix[], int col){
  if(checkObjection(col,currentfigure,matrix,fig) && col<=0){
    return true;
  } 
  return false;
}
