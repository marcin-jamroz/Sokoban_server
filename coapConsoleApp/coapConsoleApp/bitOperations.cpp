#include "stdafx.h"
// Funkcja zwracaj�ca reprezentacj� bitu z tablicy bajtow (char - 1 bajt)
char getBit(char byte, int position) {
	/* Operacja AND na bajcie byte oraz bajcie z 1-k� w miejscu kt�ry bit chcemy sprawdzi�
	Np. byte = 0110, bitNumber = 2
	0110 & 0010 = 0010
	0010 = 2 != 0 -> funkcja zwraca 1
	*/
	if ((byte & (1 << position)) != 0) {
		return 1;
	}
	return 0;
}

// Funkcja ustawiaj�ca bity z byte w ustalonym miejscu
void setBits(char *src, char byte, int position) {
	/* Przyk�ad dzia�ania:
	src = 10001, bit = 1, bitNumber = 3
	wynik: 10101
	*/
	*src |= (byte << position);
}