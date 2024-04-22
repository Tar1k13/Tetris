#pragma once

#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <Tetris/helperFuncs/calFunc.h>
#include <Constants.h>

void move(int col, int currentfigure[], boolean right,int fig,int matrix[]) ;
int rotate(int col, int currentFig[],int &mvr,int fig,int &rot,int matrix[],int &height, int &cl);
void moveDown(int col,int currentfigure[],int matrix[],int fig);