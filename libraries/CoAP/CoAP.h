#ifndef CoAP_h
#define CoAP_h

// statyczna klasa udostępniająca metody i stałe do obsługi wiadomości protokołu CoAP
class CoAP {
public:
/*	static class Time {

	};

*/
	static class RequestMethod {
	public:
		static const char EMPTY = 0;
		static const char GET = 1;
		static const char POST = 2;
		static const char PUT = 3;
		static const char DELETE = 4;
	};
	static class ResponseCode {
	public:
		static const char REQUEST = 0;
		static const char SUCCESS = 2;
		static const char CLIENT_ERROR = 4;
		static const char SERVER_ERROR = 5;
	};
	static class MessageType {
	public:
		static const char CON = 0, NON = 1, ACK = 2, RST = 3;
	};
	// Funkcja ustawiająca nagłówek wiadomości CoAP
	static void setHeader(char * header, int versionNo, char msgType, int tokenLength, char code, int codeDetails, uint16_t msgId);
	// Funkcja generująca Message ID wiadomości CoAP
	//uint16_t static generateMsgId();
	// Fu
	static void setToken(char *coapMessage, char token[]);
};
#endif