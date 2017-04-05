#pragma once

// Funkcja zwracaj¹ca reprezentacjê bitu z tablicy bajtow (char - 1 bajt)
char getBit(char byte, int position);

// Funkcja ustawiaj¹ca bity z byte w ustalonym miejscu. Pozycjê ustala najm³odszy bit.
void setBits(char *src, char byte, int position);