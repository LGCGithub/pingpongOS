#include "queue.h"
#include <stdio.h>

int queue_size(queue_t *queue){
	//printf("Size\n");
	//printf("end: %x\n", queue);
	//printf("\n");

	if(queue == NULL) return 0;
	queue_t* inicio = queue;
	int cont = 1;
	while(queue->next != inicio){
		cont++;
		queue = queue->next;
	}

	return cont;
}

void queue_print(char *name, queue_t *queue, void print_elem (void* ptr)){
	printf("%s: [", name);
	if(queue == NULL) {
		printf("]\n");
		return;
	}
	queue_t *inicio = queue;
	do {
		void* ptr = queue;
		print_elem(ptr);
		queue = queue->next;
		if(queue != inicio) printf(" ");
	} while(queue != inicio);
	printf("]\n");	
}

int queue_append (queue_t **queue, queue_t *elem){
	//printf("Append\n");
	//printf("end: %x\n", (*queue));
	if(elem == NULL) return -1;
	if(elem->next != NULL || elem->prev != NULL) return -1;

	if((*queue) == NULL) {
		//printf("Primeiro elemento\n");
		(*queue) = elem;
		(*queue)->next = (*queue);
		(*queue)->prev = (*queue);
	} else {
		//printf("Outro elemento\n");
		// Pessoa nova vai para o começo da fila. Isso deveria ter sido especificado.
		// Ponteiro da fila aponta para o final, achei que apontasse para o inicio. Perdi um bom tempo aqui.
		queue_t* fim = (*queue)->prev;
		fim->next = elem;
		queue_t* novo = fim->next;
		novo->next = (*queue);
		novo->prev = fim;
		(*queue)->prev = novo;
	}
	//printf("\n");
	return 0;
}

int queue_remove (queue_t **queue, queue_t *elem){
	//printf("Remove\n");
	//printf("end: %x\n", (*queue));
	//printf("\n");

	int size = queue_size((*queue));
	if((*queue) == NULL) return -1;
	if(size == 0) return -1;
	if(elem == NULL) return -1;

	queue_t* inicio = (*queue);
	queue_t *prev, *next;


	if(size == 1 && (*queue) == elem){
		(*queue)->next = NULL;
		(*queue)->prev = NULL;
		(*queue) = NULL;
		return 0;
	}

	// Se for o ponteiro inicial, o ponteiro original é alterado
	if(inicio == elem){
		prev = (*queue)->prev;
		next = (*queue)->next;

		prev->next = next;
		next->prev = prev;

		(*queue)->next = NULL;
		(*queue)->prev = NULL;

		(*queue) = next;

		return 0;
	}

	// Caso contrário, o ponteiro original não muda
	do {
		if((*queue) == elem){
			prev = (*queue)->prev;
			next = (*queue)->next;

			prev->next = next;
			next->prev = prev;

			(*queue)->next = NULL;
			(*queue)->prev = NULL;

			(*queue) = inicio;
			break;
		}

		(*queue) = (*queue)->next;
	} while((*queue) != inicio);

	return 0;
}
