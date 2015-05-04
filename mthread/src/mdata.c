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

void enqueue_ready(TCB_t* tcb) {
	enqueue(&(_ready_head_[tcb->prio]), &(_ready_tail_[tcb->prio]), tcb);
	tcb->state = READY;
}

void enqueue_running(TCB_t* tcb) {
	enqueue(&_running_head_, &_running_head_, tcb);
	tcb->state = RUNNING;
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

TCB_t* dequeue_ready(int prio) {
	return dequeue(&(_ready_head_[prio]), &(_ready_tail_[prio]));
}

TCB_t* dequeue_running() {
	return dequeue(&_running_head_, &_running_tail_);
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

int tcb_exists(int tid) {
	if (tid >= _next_tid_ || tid < 0)
		return FALSE;
	int i;
	TCB_t* tcb = NULL;
	for (i = 0; i < N_PRIORITIES && !tcb; i++) {
		tcb = find_thread(_ready_head_[i], _ready_tail_[i], tid);
	}
	if (tcb || tcb_is_blocked_waiting(tid) || tcb_is_blocked_waiting(tid))
		return TRUE;
	return FALSE;
}

TCB_t* find_thread(TCB_t* head, TCB_t* tail, int tid) {
	TCB_t* tcb = head;
	while (tcb) {
		if (tcb->tid == tid) {
			return tcb;
		}
		tcb = tcb->next;
	}
	return NULL;
}

int init() {
	if (terminator_init() < 0)
		return -1;
	if (scheduler_init() < 0)
		return -1;
	TCB_t *main_thread = main_thread_init();
	enqueue_ready(main_thread);
	return swapcontext(&(main_thread->context), &_scheduler_context_);
}

TCB_t* main_thread_init() {
	TCB_t* main_thread = (TCB_t*) malloc(sizeof(TCB_t));
	if (!main_thread)
		return NULL;
	main_thread->tid = _next_tid_++;
	main_thread->state = CREATION;
	main_thread->prio = HIGH;
	main_thread->prev = NULL;
	main_thread->next = NULL;
	return main_thread;
}

int terminator_init() {
	if (getcontext(&_terminator_context_) < 0)
		return -1;
	_terminator_context_.uc_link = NULL;
	_terminator_context_.uc_stack.ss_sp = _terminate_stack;
	_terminator_context_.uc_stack.ss_size = sizeof(_terminate_stack);
	makecontext(&_terminator_context_, (void (*)(void)) terminate, 0);
	return 0;
}

int scheduler_init() {
	if (getcontext(&_scheduler_context_))
		return -1;
	_scheduler_context_.uc_link = NULL;
	_scheduler_context_.uc_stack.ss_sp = _sched_stack;
	_scheduler_context_.uc_stack.ss_size = sizeof(_sched_stack);
	makecontext(&_scheduler_context_, (void (*)(void)) schedule, 0);
	return 0;
}

TCB_t* tcb_init(int tid, int prio, void *(*start)(void*), void* arg) {
	TCB_t* n_tcb = (TCB_t*) malloc(sizeof(TCB_t));
	n_tcb->tid = tid;
	n_tcb->state = CREATION;
	n_tcb->prio = prio;
	n_tcb->prev = NULL;
	n_tcb->next = NULL;
	if (getcontext(&(n_tcb->context)) < 0) {
		free(n_tcb);
		return NULL;
	}
	n_tcb->context.uc_link = &_terminator_context_;
	n_tcb->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
	n_tcb->context.uc_stack.ss_size = SIGSTKSZ;
	if (!n_tcb->context.uc_stack.ss_sp) {
		free(n_tcb);
		return NULL;
	}
	makecontext(&(n_tcb->context), (void (*)(void)) start, 1, arg);
	return n_tcb;
}

void terminate() {
	TCB_t* tcb = dequeue_running();
	tcb->state = TERMINATED;
	TCB_t* b_tcb = remove_blocked_waiting(tcb->tid);
	if (b_tcb) {
		enqueue_ready(b_tcb);
	}
	free(tcb->context.uc_stack.ss_sp);
	free(tcb);
	setcontext(&_scheduler_context_);
}

void schedule() {
	int i;
	for (i = 0; i < N_PRIORITIES; i++) {
		if (_ready_head_[i] != NULL) {
			TCB_t* tcb = dequeue_ready(i);
			enqueue_running(tcb);
			setcontext(&(tcb->context));
		}
	}
}

int tcb_is_blocked_waiting(int tid) {
	BW_TCB_t* b_tcb = _blocked_waiting_head_;
	while (b_tcb) {
		if (b_tcb->blocked_tcb->tid == tid) {
			return TRUE;
		}
		b_tcb = b_tcb->next;
	}
	return FALSE;
}

void insert_blocked_waiting(int tid, TCB_t* tcb) {
	BW_TCB_t* b_tcb = (BW_TCB_t*) malloc(sizeof(BW_TCB_t));
	b_tcb->blocked_tcb = tcb;
	tcb->state = BLOCKED;
	b_tcb->waited_tid = tid;
	b_tcb->next = _blocked_waiting_head_;
	_blocked_waiting_head_ = b_tcb;
}

TCB_t* find_blocked_waiting_tcb(int waited_tid) {
	BW_TCB_t* b_tcb = _blocked_waiting_head_;
	while (b_tcb) {
		if (b_tcb->waited_tid == waited_tid) {
			return b_tcb->blocked_tcb;
		}
		b_tcb = b_tcb->next;
	}
	return NULL;
}

TCB_t* remove_blocked_waiting(int tid) {
	BW_TCB_t* b_tcb = _blocked_waiting_head_;
	BW_TCB_t* rem = NULL;
	TCB_t* tcb;
	if (!b_tcb) {
		return NULL;
	}
	if (b_tcb->waited_tid == tid) {
		tcb = b_tcb->blocked_tcb;
		_blocked_waiting_head_ = _blocked_waiting_head_->next;
		free(b_tcb);
		return tcb;
	}
	while (b_tcb->next) {
		if (b_tcb->next->waited_tid == tid) {
			tcb = b_tcb->next->blocked_tcb;
			rem = b_tcb->next;
			b_tcb->next = rem->next;
			free(rem);
			return tcb;
		}
		b_tcb = b_tcb->next;
	}
	return NULL;
}

void insert_blocked_mutex(int tid) {
	BM_TCB_t* m_tcb = (BM_TCB_t*) malloc(sizeof(BM_TCB_t));
	m_tcb->tid = tid;
	m_tcb->next = _blocked_mutex_head_;
	_blocked_mutex_head_ = m_tcb;
}

void remove_blocked_mutex(int tid) {
	BM_TCB_t* b_tcb = _blocked_mutex_head_;
	BM_TCB_t* rem = NULL;
	if (!b_tcb) {
		return;
	}
	if (b_tcb->tid == tid) {
		_blocked_mutex_head_ = _blocked_mutex_head_->next;
		free(b_tcb);
	}
	while (b_tcb->next) {
		if (b_tcb->next->tid == tid) {
			rem = b_tcb->next;
			b_tcb->next = rem->next;
			free(rem);
			return;
		}
		b_tcb = b_tcb->next;
	}
	return;
}

int tcb_is_blocked_mutex(int tid) {
	BM_TCB_t* b_tcb = _blocked_mutex_head_;
	while (b_tcb) {
		if (b_tcb->tid == tid) {
			return TRUE;
		}
		b_tcb = b_tcb->next;
	}
	return FALSE;
}
