#ifndef CoapResource_h
#define CoapResource_h

#include <Arduino.h>

class CoapResource
{
private:
	String name;
	String resourceType;
	String interfaceDescription;
	uint8_t contentType;
	bool isObservable;
public:
	CoapResource(String name, String resourceType, String interfaceDescription, uint8_t contentType, bool isObservable);
	void getCoreLinkFormat(String &coreLinkFormat);
};

#endif
