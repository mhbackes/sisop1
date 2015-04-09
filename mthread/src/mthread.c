#include "mthread.h"
#include "mdata.h"

int mcreate (int prio, void *(*start)(void*), void *arg){

	
	if (_last_tid == 0) {
		ucontext_t main_context;
		getcontext(&main_context);
		if (_last_id == 0) {
			TCB_t *main_thread = thread_init(_last_tid++, 1, prio, main_context, NULL, NULL);
			enqueue(&_ready_head, &_ready_tail, main_thread);
			makecontext(&_sched_context, (void (*)(void))schedule, 0);
			setcontext(&_sched_context);
		}
	}

	ucontext_t = new_thread_context;
	TCB_t *new_thread = (TCB_t*) malloc(sizeof(TCB_t));
	
	makecontext(&new_thread_context, (void (*)(void))start, 1, arg); 
	new_thread = thread_init(_last_tid++, 1, prio, new_thread_context, NULL, NULL);
	enqueue(&_ready_head, &_ready_tail, &new_thread);
}
