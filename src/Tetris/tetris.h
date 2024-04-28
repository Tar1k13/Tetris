#pragma once

#include <Arduino.h>
#include <Tetris/helperFuncs/calFunc.h>
#include <Tetris/movementFuncs/MovementFuncs.h>
#include "Constants.h"
#include <ControlRefs/control_refs.h>

void saveToMatrix(int* matrix, int cCol);

void mainMoving(void* pvParameters);

void control_listener(void* pvParameters);