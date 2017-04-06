

#include "Arduino.h"
#include "BitOperations.h"

// Funkcja zwracająca reprezentację bitu z tablicy bajtow (char - 1 bajt)
char BitOperations::getBit(char byte, int position) {
	/* Operacja AND na bajcie byte oraz bajcie z 1-ką w miejscu który bit chcemy sprawdzić
	Np. byte = 0110, bitNumber = 2
	0110 & 0010 = 0010
	0010 = 2 != 0 -> funkcja zwraca 1
	*/
	if ((byte & (1 << position)) != 0) {
		return 1;
	}
	return 0;
}

// Funkcja ustawiająca bity z byte w ustalonym miejscu
void BitOperations::setBits(char *src, char byte, int position) {
	/* Przykład działania:
	src = 10001, bit = 1, bitPosition = 3
	wynik: 10101
	*/
	*src |= (byte << position);
}