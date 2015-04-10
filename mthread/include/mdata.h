/*
 * mdata.h: arquivo de inclus�o de uso apenas na gera��o da libmthread
 *
 * Esse arquivo pode ser modificado. ENTRETANTO, deve ser utilizada a TCB fornecida.
 *
 */
#ifndef __mdata__
#define __mdata__

#include <stdlib.h>
#include <ucontext.h>

/* N�O ALTERAR ESSA struct */
typedef struct TCB { 
	int  tid;		// identificador da thread 
	int  state;	// estado em que a thread se encontra 
					// 0: Cria��o; 1: Apto; 2: Execu��o; 3: Bloqueado e 4: T�rmino 
	int  prio;		// prioridade da thread (0:alta; 1: m�dia, 2:baixa) 
	ucontext_t   context;	// contexto de execu��o da thread (SP, PC, GPRs e recursos) 
	struct TCB   *prev;		// ponteiro para o TCB anterior da lista 
	struct TCB   *next;		// ponteiro para o pr�ximo TCB da lista 
} TCB_t;

enum state { CREATION, READY, EXECUTING, BLOCKED, TERMINATED };

TCB_t* _ready_head[3];
TCB_t* _ready_tail[3];
TCB_t* _blocked_head;
TCB_t* _blocked_tail;
TCB_t* _exe_head;
TCB_t* _exe_tail;

ucontext_t _sched_context;
char _sched_stack[SIGSTKSZ];

int _next_tid;

// fun��es:

void enqueue(TCB_t** head, TCB_t** tail, TCB_t* tcb);
TCB_t* dequeue(TCB_t** head, TCB_t** tail);

TCB_t* thread_init(int, int, int, TCB_t*, TCB_t*);

void schedule();
#endif
