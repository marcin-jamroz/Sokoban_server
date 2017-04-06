#pragma once
#include "Element.h"
#include "stdafx.h"


template<class T>
Element<T>::Element(T element) {
	this->element = element;
}

template<class T>
T Element<T>::get() {
	return element;
}