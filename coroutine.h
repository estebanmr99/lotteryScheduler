#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Struct para guardar los contextos de la funcion que llama y la que fue llamada
typedef struct {
  jmp_buf callee_context;
  jmp_buf caller_context;
} coroutine;

typedef void (*func)(void*);

void start(coroutine* c, func f, void* arg, void* sp);
void yield(coroutine* c);
int next(coroutine* c); // 

enum { WORKING = 1, DONE };

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion que se llama para saltar de vuelta a donde fue llamada la corutina, es un par de setjmp/longjmp
void yield(coroutine* c) {
  if(!setjmp(c->callee_context)) { // Si el setjmp no retorna 0 puede salvar el contexto
    longjmp(c->caller_context, WORKING); // Retorna al contexto previo que se guardo
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion que obtiene el siguente valor al hacer la corutina, si devuelve falso es porque ya termino, es un par de setjmp/longjmp
int next(coroutine* c) {
  int ret = setjmp(c->caller_context);
  if(!ret) { 
    longjmp(c->callee_context, 1);
  }
  else {
    return ret == WORKING; // Reporta de vuelta si ya se completo la corutina o si todavia hay partes pendientes
  }
}

//Struct para la copiar los valores de la corutina del lado del productor
typedef struct {
  coroutine* c;
  func f;
  void* arg;
  void* old_sp;
  void* old_fp;
} start_params;


// Codigo ensamblador para mover los registros de las pilas
#define get_sp(p) \
  asm volatile("movq %%rsp, %0" : "=r"(p))
#define get_fp(p) \
  asm volatile("movq %%rbp, %0" : "=r"(p))
#define set_sp(p) \
  asm volatile("movq %0, %%rsp" : : "r"(p))
#define set_fp(p) \
  asm volatile("movq %0, %%rbp" : : "r"(p))

enum { FRAME_SZ=5 }; //fairly arbitrary

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion que inicializa la corutina
void start(coroutine* c, func f, void* arg, void* sp)
{
  // Guarda espacio para una pila por separado
  start_params* p = ((start_params*)sp)-1; // Se ocupa una pila separada para el productor

  // Guarda los parametros antes de cambiar la pila 
  p->c = c;
  p->f = f;
  p->arg = arg;
  get_sp(p->old_sp);
  get_fp(p->old_fp);

  set_sp(p - FRAME_SZ);
  set_fp(p); 
  get_fp(p); // se lee P de vuelta de $fp

  // Se leen los parametros desde la copia de la corutina
  if(!setjmp(p->c->callee_context)) {
    set_sp(p->old_sp);
    set_fp(p->old_fp);
    return; // Retorna al consumidor
  }
  (*p->f)(p->arg); // Cambio de pila
  longjmp(p->c->caller_context, DONE);
}