#include "../include/mthread.h"
#include "../include/mdata.h"

int mcreate (int prio, void *(*start)(void*), void *arg){

	
	if (_next_tid == 0) {
		TCB_t *main_thread = thread_init(_next_tid, 1, prio, NULL, NULL);
		getcontext(&(main_thread->context));
		if (_next_tid == 0) {
			_next_tid++;
			enqueue(&_ready_head, &_ready_tail, main_thread);
			getcontext(&_sched_context);
			_sched_context.uc_link = NULL;
			_sched_context.uc_stack.ss_sp = _sched_stack;
			_sched_context.uc_stack.ss_size = sizeof(_sched_stack);
			makecontext(&_sched_context, (void (*)(void)) schedule, 0);
			setcontext(&_sched_context);
		}
	}
	TCB_t *new_thread = (TCB_t*) malloc(sizeof(TCB_t));
	new_thread = thread_init(_next_tid++, 1, prio, NULL, NULL);
	
	getcontext(&(new_thread->context));
	new_thread->context.uc_link = NULL;
	new_thread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
	new_thread->context.uc_stack.ss_size = SIGSTKSZ;
	makecontext(&(new_thread->context), (void (*)(void))start, 1, arg);

	enqueue(&_ready_head, &_ready_tail, &new_thread);

	return new_thread->tid;
}
