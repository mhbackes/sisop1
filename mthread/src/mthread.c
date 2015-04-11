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
