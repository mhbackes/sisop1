/*
 * mdata.h: arquivo de inclusão de uso apenas na geração da libmthread
 *
 * Esse arquivo pode ser modificado. ENTRETANTO, deve ser utilizada a TCB fornecida.
 *
 */
#ifndef __mdata__
#define __mdata__

#include <stdlib.h>
#include <ucontext.h>

/* NÃO ALTERAR ESSA struct */
typedef struct TCB { 
	int  tid;		// identificador da thread 
	int  state;	// estado em que a thread se encontra 
					// 0: Criação; 1: Apto; 2: Execução; 3: Bloqueado e 4: Término 
	int  prio;		// prioridade da thread (0:alta; 1: média, 2:baixa) 
	ucontext_t   context;	// contexto de execução da thread (SP, PC, GPRs e recursos) 
	struct TCB   *prev;		// ponteiro para o TCB anterior da lista 
	struct TCB   *next;		// ponteiro para o próximo TCB da lista 
} TCB_t;

enum state { CREATION, READY, EXECUTING, BLOCKED, TERMINATED };

TCB_t* _ready_head[3] = { NULL, NULL, NULL };
TCB_t* _ready_tail[3] = { NULL, NULL, NULL };
TCB_t* _blocked_head = NULL;
TCB_t* _blocked_tail = NULL;
TCB_t* _exe_head = NULL;
TCB_t* _exe_tail = NULL;

ucontext_t _sched_context;

int _last_tid = 0;

// funções:

void enqueue(TCB_t** head, TCB_t** tail, TCB_t* tcb);
TCB_t* dequeue(TCB_t** head, TCB_t** tail);

TCB_t* thread_init(int, int, int, ucontext_t, TCB_t*, TCB_t*);

void schedule();
#endif
