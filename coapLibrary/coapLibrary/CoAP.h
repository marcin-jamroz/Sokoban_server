#ifndef CoAP_h
#define CoAP_h

// statyczna klasa udostępniająca metody i stałe do obsługi wiadomości protokołu CoAP
class CoAP {
public:
/*	static class Time {

	};

*/
	class RequestMethod {
	public:
		static const unsigned char EMPTY = 0;
		static const unsigned char GET = 1;
		static const unsigned char POST = 2;
		static const unsigned char PUT = 3;
		static const unsigned char DELETE = 4;
	};
	class ResponseCode {
	public:
		static const unsigned char REQUEST = 0;
		static const unsigned char SUCCESS = 2;
		static const unsigned char CLIENT_ERROR = 4;
		static const unsigned char SERVER_ERROR = 5;
	};
	class MessageType {
	public:
		static const unsigned char CON = 0, NON = 1, ACK = 2, RST = 3;
	};
	// Funkcja ustawiająca nagłówek wiadomości CoAP
	static void setHeader(unsigned char * header, int versionNo, unsigned char msgType, int tokenLength, unsigned char code, int codeDetails, char msgId[]);
	// Funkcja generująca Message ID wiadomości CoAP
	//uint16_t static generateMsgId();
	static void setToken(unsigned char *coapMessage, unsigned char token[]);
};
#endif