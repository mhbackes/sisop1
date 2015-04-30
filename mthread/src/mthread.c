#include "../include/mthread.h"
#include "../include/mdata.h"

int mcreate(int prio, void *(*start)(void*), void *arg) {
	if (_next_tid == 0) {
		init();
	}
	TCB_t *new_thread = thread_init(_next_tid++, CREATION, prio, start, arg);
	enqueue(&_ready_head[prio], &_ready_tail[prio], new_thread);
	new_thread->state = READY;
	return new_thread->tid;
}

int myield() {
	int prio = _run_head->prio;
	TCB_t *yielded = dequeue(&_run_head, &_run_tail);
	yielded->state = READY;
	enqueue(&_ready_head[prio], &_ready_tail[prio], yielded);
	int ret = swapcontext(&(yielded->context), &_sched_context);
	return ret;
}

int mmutex_init(mmutex_t *mtx) {
	mtx = (mmutex_t*) malloc(sizeof(mmutex_t));
	if (!mtx)
		return -1;
	mtx->flag = 0;
	mtx->first = NULL;
	mtx->last = NULL;

	return 0;
}

int mwait(int tid) {
	TCB_t* tcb = dequeue(&_run_head, &_run_tail);
	if (find_blocked_thread(tid))
		return -1;
	insert_blocked_thread(tid, tcb);
	tcb->state = BLOCKED;
	return swapcontext(&(tcb->context), &_sched_context);
}

int mlock(mmutex_t *mtx) {
	if (!mtx)
		return -1;
	if (mtx->flag) {
		_run_head->state = BLOCKED;
		TCB_t* tcb = dequeue(&_run_head, &_run_tail);
		enqueue(&(mtx->first), &(mtx->last), tcb);
		swapcontext(&(tcb->context), &_sched_context);
	} else {
		mtx->flag = LOCKED;
	}
	return 0;
}

int munlock(mmutex_t *mtx) {
	if (!mtx)
		return -1;
	if (mtx->flag == UNLOCKED)
		return -1;
	TCB_t* tcb = dequeue(&(mtx->first), &(mtx->last));
	if (tcb) {
		enqueue(&_ready_head[tcb->prio], &_ready_tail[tcb->prio], tcb);
	} else {
		mtx->flag = UNLOCKED;
	}
	return 0;
}
