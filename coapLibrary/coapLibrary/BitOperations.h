#ifndef BitOperations_h
#define BitOperations_h



class BitOperations {
public:
	// Funkcja zwracająca reprezentację bitu z tablicy bajtow (char - 1 bajt)
	static char getBit(char byte, int position);

	// Funkcja ustawiająca bity z byte w ustalonym miejscu
	static void setBits(unsigned char *src, char byte, int position);
};

#endif