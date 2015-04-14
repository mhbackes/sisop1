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
	int yielded = 0;
	int ret = getcontext(&(_run_head->context));
	
	if (yielded == 0) { 
		yielded = 1;	// evita que a thread execute esse código quando voltar
		int prio = _run_head->prio;
		TCB_t *yielded_thread = dequeue(&_run_head, &_run_tail);
		yielded_thread->state = READY;
		enqueue(&_ready_head[prio], &_ready_tail[prio], yielded_thread);
		setcontext(&_sched_context); // chaveia para o escalonador
	}
	return ret; // caso ocorra algum erro em getcontext, retorna -1, senão, retorna 0
}

int mmutex_init(mmutex_t *mtx) {
	mtx = (mmutex_t*) malloc(sizeof(mmutex_t));
	if (!mtx) {
		return -1;
	} else {
		mtx->flag = 0;
		mtx->first = NULL;
		mtx->last = NULL;
	}
	return 0;
}

int mlock(mmutex_t *mtx) {
	if (mtx) {
		if (mtx->flag) {
			_run_head->state = BLOCKED;
			TCB_t* tcb = dequeue(&_run_head, &_run_tail);
			enqueue(&(mtx->first), &(mtx->last), tcb);
			swapcontext(&(tcb->context), &_sched_context);
		} else {
			mtx->flag = 1;
		}
		return 0;
	} else {
		return -1;
	}
}
