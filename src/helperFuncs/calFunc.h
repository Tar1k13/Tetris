#ifndef calFunc_h
#define calFunc_h

#include <Arduino.h>
#include <MD_MAX72xx.h>

int calculateHeight(int fig[]);
int countOnes(int num, int numBits);
boolean checkObjection(int col,int currentfigure[],int matrix[],int fig);
boolean checkFigureObjection(int* mvdFig, int col,int matrix[],int fig);
boolean checkBoundings(boolean right,int currentfigure[],int fig);
boolean isRotateAble(int col, int rotated[], int matrix[],int fig);

// void deleteAnimation(int* elemsToDel, int col, int heigh, MD_MAX72XX &mx);

#endif