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
		getcontext(&_terminate_context);
		_terminate_context.uc_link = NULL;
		_terminate_context.uc_stack.ss_sp = _terminate_stack;
		_terminate_context.uc_stack.ss_size = sizeof(_terminate_stack);
		makecontext(&_terminate_context, (void (*)(void)) thread_end, 0);
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
	new_thread->context.uc_link = &_terminate_context; // toda thread retorna para o contexto do terminador quando termina
	new_thread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
	new_thread->context.uc_stack.ss_size = SIGSTKSZ;
	makecontext(&(new_thread->context), (void (*)(void))start, 1, arg);
	return new_thread;
}

void thread_end(){ //FUNÇÃO ARNOLD SCHWARZENEGGER
	TCB_t* tcb = dequeue(&_run_head, &_run_tail);
	tcb->state = TERMINATED;
	TCB_t* waiting_tcb = remove_blocked_thread(tcb->tid);
	if(waiting_tcb){
		enqueue(&_ready_head, &_ready_tail, waiting_tcb);
		waiting_tcb->state = READY;
	}
	free(tcb->context.uc_stack.ss_sp);
	free(tcb);
	setcontext(&_sched_context);
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

TCB_t* find_blocked_thread(int tid) {
	BLOCKED_TCB_t* b_tcb = _blocked_head;
	
	while (b_tcb) {
		if (b_tcb->waited_tid == tid) {
			return b_tcb->waiting_tcb;
		}
		b_tcb = b_tcb->next;
	}
	
	return NULL;	// caso não encontrar
}

void insert_blocked_thread(int tid, TCB_t* tcb) {
	BLOCKED_TCB_t* b_tcb = (BLOCKED_TCB_t*) malloc(sizeof(BLOCKED_TCB_t));
	b_tcb->waiting_tcb = tcb;
	b_tcb->waited_tid = tid;
	b_tcb->next = _blocked_head;
	_blocked_head = b_tcb;
}

TCB_t* remove_blocked_thread(int tid) {
	BLOCKED_TCB_t* b_tcb = _blocked_head;
	BLOCKED_TCB_t* rem = NULL;
	TCB_t* tcb;
	
	if (!b_tcb) {
		return NULL;
	}
	
	if (b_tcb->waited_tid == tid) {
		tcb = b_tcb->waiting_tcb;
		_blocked_head = _blocked_head->next;
		free(b_tcb);
		return tcb;
	}
	
	while (b_tcb->next) {
		if (b_tcb->next->waited_tid == tid) {
			tcb = b_tcb->next->waiting_tcb;
			rem = b_tcb->next;
			b_tcb->next = rem->next;
			free(rem);
			return tcb;
		}
		b_tcb = b_tcb->next;
	}
	
	return NULL;	// caso não encontrar
}
