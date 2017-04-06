#pragma once
template <class T>
class Element
{
private:
	T element;
	Element* next = nullptr;
	template <class T>
	friend class List;
public:
	Element(T element); // konstruktor elementu listy obiektów
	~Element(); 
	

	T get();
};

#include "Element.cpp"