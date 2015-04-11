#include "../include/mdata.h"

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
		tcb->next = NULL;
		*tail = tcb;
	}
}

TCB_t* dequeue(TCB_t** head, TCB_t** tail) {
	if (!*head)
		return NULL;
	TCB_t* ret = *head;
	*head = (*head)->next;
	if (!*head)
		*tail = NULL;
	else
		(*head)->prev = NULL;
	return ret;
}

void init() {
	TCB_t *main_thread = main_thread_init();
	getcontext(&(main_thread->context));
	if (_next_tid == 0) {
		_next_tid++;
		enqueue(&_ready_head[HIGH], &_ready_tail[HIGH], main_thread);
		getcontext(&_sched_context);
		_sched_context.uc_link = NULL;
		_sched_context.uc_stack.ss_sp = _sched_stack;
		_sched_context.uc_stack.ss_size = sizeof(_sched_stack);
		makecontext(&_sched_context, (void (*)(void)) schedule, 0);
		setcontext(&_sched_context);
	}
}

TCB_t* main_thread_init(){
	TCB_t* main_thread = (TCB_t*) malloc(sizeof(TCB_t));
	main_thread->tid = 0;
	main_thread->state = CREATION;
	main_thread->prio = HIGH;
	main_thread->prev = NULL;
	main_thread->next = NULL;
	return main_thread;
}

TCB_t* thread_init(int tid, int state, int prio, void *(*start)(void*), void* arg) {
	TCB_t* new_thread = (TCB_t*) malloc(sizeof(TCB_t));
	new_thread->tid = tid;
	new_thread->state = state;
	new_thread->prio = prio;
	new_thread->prev = NULL;
	new_thread->next = NULL;
	getcontext(&(new_thread->context));
	new_thread->context.uc_link = NULL;
	new_thread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
	new_thread->context.uc_stack.ss_size = SIGSTKSZ;
	makecontext(&(new_thread->context), (void (*)(void))start, 1, arg);
	return new_thread;
}

void schedule() {
	int i;

	for (i = 0; i < 3; i++) {
		if (_ready_head[i] != NULL) {
			TCB_t* tcb = dequeue(&_ready_head[i], &_ready_tail[i]);
			enqueue(&_run_head, &_run_tail, tcb);
			tcb->state = RUNNING;
			setcontext(&(tcb->context));
		}
	}
}
