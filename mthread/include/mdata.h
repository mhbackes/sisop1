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

#define N_PRIORITIES 3

#define TRUE 1
#define FALSE 0
#define ERROR -1

/* N�O ALTERAR ESSA struct */
typedef struct TCB {
	int tid;		// identificador da thread
	int state;	// estado em que a thread se encontra
				// 0: Cria��o; 1: Apto; 2: Execu��o; 3: Bloqueado e 4: T�rmino
	int prio;		// prioridade da thread (0:alta; 1: m�dia, 2:baixa)
	ucontext_t context;	// contexto de execu��o da thread (SP, PC, GPRs e recursos)
	struct TCB *prev;		// ponteiro para o TCB anterior da lista
	struct TCB *next;		// ponteiro para o pr�ximo TCB da lista
} TCB_t;

typedef struct BLOCKED_WAITING_TCB {
	int waited_tid;
	TCB_t* blocked_tcb;
	struct BLOCKED_WAITING_TCB *next;
} BW_TCB_t;

typedef struct BLOCKED_MUTEX_TCB {
	int tid;
	struct BLOCKED_MUTEX_TCB *next;
} BM_TCB_t;

enum tcb_state { CREATION = 0, READY, RUNNING, BLOCKED, TERMINATED };

enum tcb_priority {	HIGH = 0, MEDIUM, LOW };

enum mutex_state { UNLOCKED = 0, LOCKED };

TCB_t* _ready_head_[N_PRIORITIES];
TCB_t* _ready_tail_[N_PRIORITIES];
BW_TCB_t* _blocked_waiting_head_;
BM_TCB_t* _blocked_mutex_head_;
TCB_t* _running_head_;
TCB_t* _running_tail_;

ucontext_t _scheduler_context_;
char _sched_stack[SIGSTKSZ];

ucontext_t _terminator_context_;
char _terminate_stack[SIGSTKSZ];

int _next_tid_;

// fun��es:

TCB_t* find_thread(TCB_t* head, TCB_t* tail, int tid);
int tcb_exists(int tid);

int tcb_is_blocked_waiting(int tid);
int tcb_is_blocked_mutex(int tid);

void insert_blocked_waiting(int tid, TCB_t* tcb);
TCB_t* find_blocked_waiting_tcb(int tid);
TCB_t* remove_blocked_waiting(int tid);

void insert_blocked_mutex(int tid);
void remove_blocked_mutex(int tid);



TCB_t* find_thread(TCB_t* head, TCB_t* tail, int tid);

void enqueue_ready(TCB_t* tcb);
void enqueue_running(TCB_t* tcb);
void enqueue(TCB_t** head, TCB_t** tail, TCB_t* tcb);

TCB_t* dequeue_ready(int prio);
TCB_t* dequeue_running();
TCB_t* dequeue(TCB_t** head, TCB_t** tail);

TCB_t* tcb_init(int tid, int prio, void *(*start)(void*), void* arg);
void terminate();
TCB_t* main_thread_init();

int init();
int terminator_init();
int scheduler_init();
void schedule();

#endif
