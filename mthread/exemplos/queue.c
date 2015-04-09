#include <stdlib.h>
#include <stdio.h>
#include "../include/mdata.h"

TCB_t* head;
TCB_t* tail;

int main(){
	TCB_t* t = malloc(sizeof(TCB_t));
	t->tid = 0;
	enqueue(&head, &tail, t);
	printf("Head: %d, Tail: %d", head->tid, tail->tid);
	return 0;
}
