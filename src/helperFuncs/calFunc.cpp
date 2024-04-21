#include <helperFuncs/calFunc.h>
#include "Constants.h"
// #include <Constants.h>

int calculateHeight(int fig[]){
    int counter = 0;
    for (size_t i = 0; i < FIGURE_LEN; i++) {
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

boolean isGameOver(int currentfigure[],int fig,int matrix[], int col){
  if(checkObjection(col,currentfigure,matrix,fig) && col<=0){
    return true;
  } 
  return false;
}

void deleteAnimation(int *elemsToDel, int col, int heigh, boolean isRts,MD_MAX72XX &mx) {
  int ts = 2;
  int tx = 1;
  int hp = 0;
  while (hp < 4) {
    for (int t = col; t < col + heigh; t++) {
      if (elemsToDel[t] != 0) {
        int elem = elemsToDel[t] >> ts << tx;
        mx.setColumn(t, elem);
        elemsToDel[t] = elem;
      }
    }
    ts += 2;
    tx++;
    hp++;
    if (!isRts) {
      delay(100);
    } else {
      vTaskDelay(100 / portTICK_RATE_MS);
    }
  }
}

int getColFactor(int numToDel, int level) {
  switch (numToDel) {
    case 2:
      return (40 * (level + 1));
    case 3:
      return (100 * (level + 1));
    case 4:
      return (300 * (level + 1));
    case 5:
      return (1200 * (level + 1));
    default:
      return level + 1;
  }
}

void deleteLine(int* matr,
                void (*deleteAnim)(int* elemToDelete, int col, int heigh,
                                   boolean isRts,MD_MAX72XX &mx),
                int heigh, int col, boolean isRts,int &mainPointCounter,int currentColumn,TM1637Display &display,MD_MAX72XX &mx) {
  int elemToDel[16] = {0};
  boolean isDel = false;
  int delColNum = 0;  // counter
  for (int i = col; i <= col + heigh; i++) {
    // Serial.println(countOnes(matr[i],8));
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
    
    deleteAnim(elemToDel, col, heigh, isRts,mx);
    for (int i = 15; i >= 0; i--) {
      // Serial.println(mapping[i],matrix[i+1]);
      mx.setColumn(i, matr[i]);
    }
    mainPointCounter += getColFactor(delColNum, currentColumn);
    play_game_over();
    display.showNumberDec(mainPointCounter);

  }
}