#ifndef __IMPLEMENTACION_LISTA_MULTITIPO_H__
#define __IMPLEMENTACION_LISTA_MULTITIPO_H__

// Definicion de tipos
typedef struct dNode *Lpos;
typedef struct lista *List;

void createList(List *l);									// Devuelve una lista vacía
int isEmptyList(List l);									// Comprueba si la lista está vacía
Lpos firstElement(List l);									// Devuelve la primera posicion de la lista
Lpos nextElement(List l, Lpos pos);								// Devuelve la siguiente posicion de la lsita si es válida
Lpos lastElement(List l);									// Devuelve la última posicion de la lista 
Lpos prevElement(List l, Lpos pos);								// Devuelve la posicion anterior de la lista
int insertElement(List l, void *element);							// Inserta un nuevo elemento al final de la lista
int insertElementIn(List l, Lpos p, void *element);
void *deletePosition(List l, Lpos pos);
void *getElement(List l, Lpos p);								// Devuelve el contenido de la lista en 'element'
int getSize(List l);
void updateElement(List l, Lpos pos, void *element);
void clearList(List l, void (*user_mem)(void *data));						// Elimina todos los elementos de la lista
void deleteList(List l, void (*user_mem)(void *data));						// Elimina la lista pasada por referencia

#endif
