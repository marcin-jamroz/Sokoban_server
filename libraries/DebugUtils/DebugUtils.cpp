#include "DebugUtils.h"

void debugVariable(char *name, int value, char *funcName) {
	Serial.print("DEBUG: Method: ");
	Serial.print(funcName);
	Serial.print(", variable '");
	Serial.print(name);
	Serial.print("' = ");
	Serial.println(value);
}