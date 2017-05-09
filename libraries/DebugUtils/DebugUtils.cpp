#include "DebugUtils.h"

void debugVariable(char *name, int value, char *funcName) {
	Serial.print("DEBUG: Method: ");
	Serial.print(funcName);
	Serial.print(", variable '");
	Serial.print(name);
	Serial.print("' = ");
	Serial.println(value);
}
void debugVariable(char *name, uint8_t value, char *funcName) {
	Serial.print("DEBUG: Method: ");
	Serial.print(funcName);
	Serial.print(", variable '");
	Serial.print(name);
	Serial.print("' = ");
	Serial.println(value);
}
void debugVariable(char *name, char value, char *funcName) {
	Serial.print("DEBUG: Method: ");
	Serial.print(funcName);
	Serial.print(", variable '");
	Serial.print(name);
	Serial.print("' = ");
	Serial.println(value);
}