#include "../include/mthread.h"
#include "../include/mdata.h"
int mcreate(int prio, void *(*start)(void*), void *arg) {
	if (_next_tid_ == 0)
		if (init() < 0)
			return -1;
	TCB_t *n_tcb = tcb_init(_next_tid_++, prio, start, arg);
	if (!n_tcb)
		return -1;
	enqueue_ready(n_tcb);
	return n_tcb->tid;
}

int myield() {
	if (!_running_head_)
		return -1;
	TCB_t *y_tcb = dequeue_running();
	enqueue_ready(y_tcb);
	return swapcontext(&(y_tcb->context), &_scheduler_context_);
}

int mwait(int tid) {
	if (!_running_head_)
		return -1;
	if (!tcb_exists(tid) || find_blocked_waiting_tcb(tid))
		return -1;
	TCB_t* tcb = dequeue(&_running_head_, &_running_tail_);
	insert_blocked_waiting(tid, tcb);
	return swapcontext(&(tcb->context), &_scheduler_context_);
}

int mmutex_init(mmutex_t *mtx) {
	mtx->flag = 0;
	mtx->first = NULL;
	mtx->last = NULL;
	return 0;
}

int mlock(mmutex_t *mtx) {
	if (!mtx)
		return -1;
	if (!_running_head_)
		return -1;
	if (mtx->flag) {
		TCB_t* tcb = dequeue_running();
		enqueue(&(mtx->first), &(mtx->last), tcb);
		insert_blocked_mutex(tcb->tid);
		swapcontext(&(tcb->context), &_scheduler_context_);
	} else
		mtx->flag = LOCKED;
	return 0;
}

int munlock(mmutex_t *mtx) {
	if (!mtx)
		return -1;
	if (!_running_head_)
		return -1;
	if (mtx->flag == UNLOCKED)
		return -1;
	TCB_t* tcb = dequeue(&(mtx->first), &(mtx->last));
	if (tcb) {
		enqueue_ready(tcb);
		remove_blocked_mutex(tcb->tid);
	} else
		mtx->flag = UNLOCKED;
	return 0;
}
