#include <stdlib.h>
#include <stdio.h>
#include "../include/mdata.h"

TCB_t* head;
TCB_t* tail;

int main() {
	TCB_t* t = malloc(sizeof(TCB_t));
	TCB_t* q = malloc(sizeof(TCB_t));
	t->tid = 0;
	q->tid = 1;
	enqueue(&head, &tail, t);
	printf("Head: %d, Tail: %d\n", head->tid, tail->tid);
	enqueue(&head, &tail, q);
	printf("Head: %d, Tail: %d\n", head->tid, tail->tid);
	dequeue(&head, &tail);
	printf("Head: %d, Tail: %d\n", head->tid, tail->tid);
	dequeue(&head, &tail);
	enqueue(&head, &tail, t);
	printf("Head: %d, Tail: %d\n", head->tid, tail->tid);
	return 0;
}
