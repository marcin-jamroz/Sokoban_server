#pragma once

#include "stdafx.h"
#include "List.h"


template <class T>
List<T>::List() {
		first = nullptr;
		last = nullptr;
		size = 0;
	}

template<class T>
List<T>::~List()
{
}

template <class T>
bool List<T>::add(T element) {
		Element<T> * newElement = new Element<T>(element);
		if (!first) {
			first = newElement;
			last = first;
			return true;
		}
		else {
			last->next = newElement;
			return true;
		}
		return false;
	}
template <class T>
bool List<T>::isExist(T element) {
		if (!first) {
			return false;
		}
		Element<T> *tmp = first;
		while (tmp) {
			if (tmp->get() == element)
				return true;
			tmp = tmp->next;
		}
		return false;
	}

template <class T>
bool List<T>::remove(T element) {
		
		Element<T> *tmp = first;
		Element<T> *tmp_prev = nullptr;
		while (tmp)
		{
			if(element == tmp->get()) {
				if (tmp == first) {
					first = tmp->next;
					delete tmp;
					return true;
				}
				tmp_prev->next = tmp->next;
				delete tmp;
				return true;
			}
			tmp_prev = tmp;
			tmp = tmp->next;
		}
		return false;
	}
