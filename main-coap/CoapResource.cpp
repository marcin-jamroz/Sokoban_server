// Odkrywanie zasobow za pomoca Discover w Copperze
// Uri-Path: .well-known/core


// W .ino - do usuni�cia
//#define String char *
//#define uint8_t unsigned char


#include "CoapResource.h"
    CoapResource::CoapResource(String name, String resourceType, String interfaceDescription, uint8_t contentType, bool isObservable)
      : name(name), resourceType(resourceType), interfaceDescription(interfaceDescription), contentType(contentType), isObservable(isObservable)
    {}

    void CoapResource::getCoreLinkFormat(String &coreLinkFormat) {
      coreLinkFormat = String("</" + name + ">;rt=\"" + resourceType + "\";if=\"" + interfaceDescription + "\";ct=" + contentType);
      if (isObservable) {
        coreLinkFormat = String(coreLinkFormat + ";obs");
      }
      coreLinkFormat = String(coreLinkFormat + ",");
    }
/// TODO: 3 metryki do badania jakosci polaczenia internetowego
