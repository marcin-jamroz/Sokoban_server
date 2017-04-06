#pragma once
#include "Element.h"
// Jednokierunkowa lista niepowtarzalnych obiektów
template <class T>
class List {
private:
	Element<T>* first;
	Element<T>* last;
	size_t size;
public:

	List(); // konstruktor listy
	~List(); // destruktor listy

	bool isExist(T element); // Sprawdza czy dany element istnieje w liœcie
	bool remove(T element); // Usuwa wskazany element. Zwraca false gdy element nie istnieje
	bool add(T element);  // Dodaje nowy element do list. Zwraca false gdy taki istnieje
};
#include "List.cpp"