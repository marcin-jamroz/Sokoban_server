#pragma once

// Funkcja zwracaj�ca reprezentacj� bitu z tablicy bajtow (char - 1 bajt)
char getBit(char byte, int position);

// Funkcja ustawiaj�ca bity z byte w ustalonym miejscu. Pozycj� ustala najm�odszy bit.
void setBits(char *src, char byte, int position);