#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

struct dNode {
	void *dato;
	struct dNode *next;
};

struct lista{
	int size;
	struct dNode *last;
	struct dNode *head;
};


// Crea una lista vacía
void createList(List *l){
	if((*l)!=NULL)
		return;
		
	*l = (struct lista *)malloc(sizeof(struct lista));
	if(*l==NULL)
		return;
		
	(*l)->size=0;
	(*l)->head=NULL;
	(*l)->last=NULL;
}

// Comprueba si la lista está vacía
int isEmptyList(List l){
	return (l->size==0 || l->head==NULL || l->last==NULL)? 1 : 0;
}

// Retorna el primer elemento de la lista
Lpos firstElement(List l){
	return l->head;
}

// Retorna el siguiente elemento si es válido
Lpos nextElement(List l, Lpos p){
	return (p->next==NULL)? NULL : p->next;
}

//Retorna el ultimo elemento de la lista
Lpos lastElement(List l){
	return l->head;
}

//Retorna elemento anterior de la lista 
Lpos prevElement(List l, Lpos pos){
	if(l->head==pos) return NULL;
	struct dNode *auxPos=l->head;
	while(auxPos->next!=pos)
		auxPos=auxPos->next;
	return auxPos;
}

// Inserta un elemento en la lista
int insertElement(List l, void *element){
	struct dNode *nwNode;
	if((nwNode=(struct dNode *)malloc(sizeof(struct dNode)))==NULL) return 0;


	// Compia de un duplicado del elemento a introducir
	nwNode->dato= element;
	nwNode->next=NULL;

	if(l->head==NULL)
		l->head=nwNode;
	if(l->last!=NULL)
		l->last->next=nwNode;
	l->last=nwNode;
	l->size++;

	return 1;
}

//Devuelve 1 si se puede insertar 
int insertElementIn(List l,Lpos p,  void *element){
	struct dNode *nwNode;
	if((nwNode=(struct dNode *)malloc(sizeof(struct dNode)))==NULL) return 0;

	nwNode->dato=element;
	nwNode->next=NULL;

	// Inicio de la lista o primera posicion de la lista
	if(p==l->head || p==NULL){
		nwNode->next = l->head;
		l->head = nwNode;

	// Ultima posicion de la lista
	}else if(p->next==NULL){
		p->next = nwNode;

	// Si es un elemento en el medio de la lista
	}else{
		nwNode->next = p->next;
		nwNode->dato = p->dato;
		p->dato = element;
		p->next = nwNode;
	}

	l->size++;
	return 1;
}

void *deletePosition(List l, Lpos pos){
	void *info;
	struct dNode *auxPos, *iter;

	if(l->head==NULL && l->last==NULL && l->size==0)
		return NULL;

	// Inicio de la lista o primera posición de la lista
	if(pos==l->head || pos==NULL){
		auxPos = l->head;
		
		// Solo hay un nodo
		if(l->size==1)
			l->head = l->last = NULL;
		else
			l->head = auxPos->next;

		auxPos->next = NULL;

	// Ultima posicion de la lista
	}else if(pos->next==NULL){
		for(iter=l->head; iter->next!=pos; iter=iter->next);
		auxPos = iter->next;
		l->last = iter;
		iter->next = NULL;

	// Si es un elemento en el medio de la lista
	}else{
		for(iter=l->head; iter->next!=pos; iter=iter->next);
		auxPos = iter->next;
		iter->next = auxPos->next;
		auxPos->next = NULL;	
	}

	l->size--;
	info = auxPos->dato;
	free(auxPos);
	return info;
}


// Retorna el elemento de la posicion en element
void *getElement(List l, Lpos p){
	return p->dato;
}

//Devuelve el tamaño de la lista
int getSize(List l){
	return l->size;
}

//Actualiza el elemento de la lista en la posicion indicada 
void updateElement(List l, Lpos pos, void *element){
	pos->dato=element;
}

// Elimina todos los elementos de la lista
void clearList(List l, void (*user_mem)(void *data)){
	if(isEmptyList(l)==1) return;
	
	struct dNode *auxPos;

	while(l->head!=NULL){
		auxPos = l->head;
		l->head = auxPos->next;
		auxPos->next = NULL;
		if(user_mem!=NULL)
			user_mem(auxPos->dato);

		free(auxPos);
	}
	
	l->size=0;
	l->last=NULL;
}

// Elimina la lista
void deleteList(List l, void (*user_mem)(void *data)){
	clearList(l, user_mem);
	free(l);
}
