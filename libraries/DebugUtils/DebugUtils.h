// DebugUtils - biblioteka wspomagaj�ca debugowanie w Arduino
// Autor: Jaros�aw Butaj�o
// Data powstania: 07.05.2017

#include <Arduino.h>

#define debugVar(name) debugVariable(#name, name, __FUNCTION__);

void debugVariable(char *name, int value, char *funcName);