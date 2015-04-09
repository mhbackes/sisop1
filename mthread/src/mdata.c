#include "mdata.h"

// Exemplo:
// 1, 2, 3, 4
// H        T
// 1->next = 2; 2->next = 3; 3->next = 4; 4->next = NULL;
// 1->prev = NULL; 2->prev = 1; 3->prev = 2; 4->prev = 3;
// dequeue() -> 2, 3, 4
//              H     T
// enqueue(5) -> 2, 3, 4, 5
//               H        T

void enqueue(TCB_t** head, TCB_t** tail, TCB_t* tcb) {
	if (!*head) {
		*head = tcb;
		*tail = tcb;
		tcb->prev = NULL;
		tcb->next = NULL;
	} else {
		(*tail)->next = tcb;
		tcb->prev = *tail;
		TCB->next = NULL;
		*tail = tcb;
	}
}

TCB_t* dequeue(TCB_t** head, TCB_t** tail){
	if (!*head)
		return NULL;
	TCB* ret = *head;
	*head = (*head)->next;
	if(!*head)
		*tail = NULL;
	else
		(*head)->prev = NULL;
	return ret;
}

