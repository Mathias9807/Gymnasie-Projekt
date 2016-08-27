/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include "def.h"

void* ListGet(List* l, int index) {
	ListEntry* cur = l->first;
	int i = 0;
	
	if (index < 0) return NULL;
	
	while (cur) {
		if (i == index) return cur->value;
		
		i++;
		cur = cur->next;
	}
	
	return NULL;
}

int ListFind(List* l, void* value) {
	ListEntry* cur = l->first;
	
	int index = 0;

	while (cur) {
		if (cur->value == value) return index;
		
		index++;
		cur = cur->next;
	}

	return -1;
}

int ListSize(List* l) {
	ListEntry* cur = l->first;
	int i = 0;
	
	while (cur) {
		cur = cur->next;
		i++;
	}
	
	l->size = i;
	
	return i;
}

void ListAdd(List* l, void* value) {
	ListEntry* cur = l->first;
	if (cur) {
		while (cur->next) 
			cur = cur->next;
		cur->next = malloc(sizeof(ListEntry));
		cur->next->value = value;
		cur->next->prev = cur;
		cur->next->next = NULL;
	}else {
		l->first = malloc(sizeof(ListEntry));
		l->first->value = value;
		l->first->prev = l->first->next = NULL;
	}
	
	l->size++;
}

void ListRemove(List* l, int index) {
	ListEntry* cur = l->first;
	int i = 0;
	
	if (index < 0) return;
	
	while (cur && i < index) {
		cur = cur->next;
		i++;
	}
	
	if (!cur) return;
	
	if (i == 0)		l->first = cur->next;
	if (cur->prev)	cur->prev->next = cur->next;
	if (cur->next)	cur->next->prev = cur->prev;
	free(cur);
	
	l->size--;
}

void ListBubbleSort(List* l, int (*comparator)(void* a, void* b)) {
	for (int i = 1; i < l->size; i++) {
		ListEntry* prev = l->first;
		ListEntry* cur = prev->next;
		bool changed = false;
		
		for (int j = i; j < l->size; j++) {
			if (!cur) break;
			
			int comp = comparator(prev->value, cur->value);
			if (comp < 0) {
				
				// Byt cur och prev
				prev->next = cur->next;
				if (prev->next)
					prev->next->prev = prev;
				if (l->first == prev) {
					l->first = cur;
					cur->prev = NULL;
				}else {
					prev->prev->next = cur;
					cur->prev = prev->prev;
				}
				cur->next = prev;
				prev->prev = cur;
				
				ListEntry* tmp = cur;
				cur = prev;
				prev = tmp;
				
				changed = true;
			}
			
			prev = prev->next;
			cur = cur->next;
		}
		
		if (!changed) break;
	}
}

double min(double a, double b) {
	return a < b ? a : b;
}

double max(double a, double b) {
	return a > b ? a : b;
}

double interp(double a, double b, double t) {
	return a * (1 - t) + b * t;
}

