#pragma once
// Funkcja zwracająca reprezentację bitu z tablicy bajtow (char - 1 bajt)
char getBit(char byte, int position);

// Funkcja ustawiająca bity z byte w ustalonym miejscu
void setBits(char *src, char byte, int position);