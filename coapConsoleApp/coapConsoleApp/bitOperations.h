#pragma once
#include "bitOperations.h"

#include "stdafx.h"

class BitOperations {
public:
	// Funkcja zwracaj�ca reprezentacj� bitu z tablicy bajtow (char - 1 bajt)
	static char getBit(char byte, int position);

	// Funkcja ustawiaj�ca bity z byte w ustalonym miejscu
	static void setBits(char *src, char byte, int position);
};

