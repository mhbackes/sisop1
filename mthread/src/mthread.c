#include "mthread.h"
#include "mdata.h"

int mcreate (int prio, void *(*start)(void*), void *arg){
	ucontext_t = new_thread_context;
	TCB_t new_thread;head
	
	makecontext(&new_thread_context, (void (*)(void))start, 1, arg); 
	new_thread = { .tid = last_tid, .state = 1, .prio = prio, .context = new_thread_context, .prev = NULL, .next = NULL };
}
