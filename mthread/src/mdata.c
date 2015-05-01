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

void enqueue_ready(TCB_t* tcb){
	enqueue(&_ready_head_, &_ready_tail_, tcb);
	tcb->state = READY;
}

void enqueue_running(TCB_t* tcb){
	enqueue(&_running_head_, &_running_head_, tcb);
	tcb->state = RUNNING;
}

void enqueue_mutex(mmutex_t* mtx, TCB_t* tcb){
	enqueue(&(mtx->first), &(mtx->last), tcb);
	tcb->state = BLOCKED;
}

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

TCB_t* dequeue_ready(int prio){
	return dequeue(&(_ready_head_[prio]), &(_ready_tail_[prio]));
}

TCB_t* dequeue_running(){
	return dequeue(&_running_head_, &_running_tail_);
}

TCB_t* dequeue_mutex(mmutex_t* mtx){
	return dequeue(&(mtx->first), &(mtx->last));
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

int thread_exists(int tid){
	if(tid >= _next_tid_ || tid < 0)
		return 0;
	int i;
	TCB_t* tcb = NULL;
	for(i = 0; i < 3 && !tcb; i++){
		tcb = find_thread(_ready_head_[i], _ready_tail_[i], tid);
	}
	if(tcb || find_blocked_thread(tid))
		return 1;
	return 0;
}

TCB_t* find_thread(TCB_t* head, TCB_t* tail, int tid) {
	TCB_t* tcb = head;

	while (tcb) {
		if (tcb->tid == tid) {
			return tcb;
		}
		tcb = tcb->next;
	}

	return NULL;	// caso não encontrar
}

int init() {
	//init terminator context
	getcontext(&_terminator_context_);
	_terminator_context_.uc_link = NULL;
	_terminator_context_.uc_stack.ss_sp = _terminate_stack;
	_terminator_context_.uc_stack.ss_size = sizeof(_terminate_stack);
	makecontext(&_terminator_context_, (void (*)(void)) terminate, 0);

	//init scheduler context
	getcontext(&_scheduler_context_);
	_scheduler_context_.uc_link = NULL;
	_scheduler_context_.uc_stack.ss_sp = _sched_stack;
	_scheduler_context_.uc_stack.ss_size = sizeof(_sched_stack);
	makecontext(&_scheduler_context_, (void (*)(void)) schedule, 0);

	// init main thread
	TCB_t *main_thread = main_thread_init();
	enqueue_ready(main_thread);
	return swapcontext(&(main_thread->context), &_scheduler_context_);
}

TCB_t* main_thread_init() {
	TCB_t* main_thread = (TCB_t*) malloc(sizeof(TCB_t));
	main_thread->tid = _next_tid_++;
	main_thread->state = CREATION;
	main_thread->prio = HIGH;
	main_thread->prev = NULL;
	main_thread->next = NULL;
	return main_thread;
}

TCB_t* thread_init(int tid, int prio, void *(*start)(void*),
		void* arg) {
	TCB_t* new_thread = (TCB_t*) malloc(sizeof(TCB_t));
	new_thread->tid = tid;
	new_thread->state = CREATION;
	new_thread->prio = prio;
	new_thread->prev = NULL;
	new_thread->next = NULL;
	getcontext(&(new_thread->context));
	new_thread->context.uc_link = &_terminator_context_; // toda thread retorna para o contexto do terminador quando termina
	new_thread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
	new_thread->context.uc_stack.ss_size = SIGSTKSZ;
	makecontext(&(new_thread->context), (void (*)(void)) start, 1, arg);
	return new_thread;
}

void terminate() { //FUNÇÃO ARNOLD SCHWARZENEGGER
	TCB_t* tcb = dequeue_running();
	tcb->state = TERMINATED;
	TCB_t* b_tcb = remove_blocked_thread(tcb->tid);
	if (b_tcb) {
		enqueue_ready(b_tcb);
	}
	free(tcb->context.uc_stack.ss_sp);
	free(tcb);
	setcontext(&_scheduler_context_);
}

void schedule() {
	int i;

	for (i = 0; i < 3; i++) {
		if (_ready_head_[i] != NULL) {
			TCB_t* tcb = dequeue_ready(i);
			enqueue_running(tcb);
			setcontext(&(tcb->context));
		}
	}
}

TCB_t* find_waited_thread(int tid) {
	BLOCKED_TCB_t* b_tcb = _blocked_head;

	while (b_tcb) {
		if (b_tcb->waited_tid == tid) {
			return b_tcb->waiting_tcb;
		}
		b_tcb = b_tcb->next;
	}

	return NULL;	// caso não encontrar
}

TCB_t* find_blocked_thread(int tid) {
	BLOCKED_TCB_t* b_tcb = _blocked_head;

	while (b_tcb) {
		if (b_tcb->waiting_tcb->tid == tid) {
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
