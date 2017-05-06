
#ifndef CoapUtils_h
#define CoapUtils_h

#include <Arduino.h>

class CoapUtils {
public:

	class OptionNumber {
	public:
		static const unsigned char URI_HOST = 3;
		static const unsigned char URI_PORT = 7;
		static const unsigned char URI_PATH = 11;
		static const unsigned char ACCEPT = 17;
		static const unsigned char CONTENT_FORMAT = 12;
	};

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

};

#endif