#include "coroutine.h"
#include <math.h>
#define N 1024

int totalTickets;
int T;
int done=0;


typedef struct Function{
    coroutine* c;
    int L;
    long int M;
    int F;
    int X;
    long double sum;
} Function;

typedef struct Stack{
    int stack[N];
} Stack;

typedef struct {
    coroutine* c;
    int max_x, max_y;
    int x, y;
} iter;

Function *functions;
Stack *stacks;

void readConsole(){
    int L,F,X;
    long int M;
    totalTickets = 0;
    scanf("%d",&T);
    functions =  (Function*) malloc(T * sizeof(Function)); 
    stacks = (Stack*) malloc(T * sizeof(Stack)); 


    for(int i = 0; i < T; i++)
    {
        X = 0;
        scanf("%d %ld %d", &L, &M, &F);

        if(F != 1)
            scanf("%d", &X);
        
        totalTickets += L;
        functions[i].c = (coroutine*) malloc(sizeof(coroutine));
        functions[i].L = L;
        functions[i].M = M;
        functions[i].F = F;
        functions[i].X = X;
        functions[i].sum = 0;
    }

    for(int i = 0; i < T; i++)
        printf("L: %d M: %ld F: %d X: %d\n",functions[i].L,functions[i].M,functions[i].F,functions[i].X);
}

void iterate(void* p) {
  iter* it = (iter*)p;
  int x,y;
  for(x=0; x<it->max_x; x++) {
    for(y=0; y < it->max_y; y++) {
      it->x = x;
      it->y = y;
      yield(it->c);
    }
  }
}
void napierian(void* p){
    Function* f = (Function*)p;
    long double n;
    for(n = 1; n <= f -> M; n++){
        f->sum+=(pow(-1,n+1)*pow(f->X,n))/n; //de interney
        //f->sum+=pow(-1,n+1*(pow(f->X,n))/n); //de eddy
        yield(f->c);
    }
    //agregar al struct un done
    done++;
}

void pi(void* p){
  
    Function* f = (Function*)p;
    long double n;
    for(n = 1; n <= f -> M; n++){
        f->sum += 1/(n*n);
        yield(f->c);
    }
    done++;
}

int chooseRandom(){ // Preguntarle a eddy si se puede repetir un ticket
    int ticketSelected = rand() % ((totalTickets - 1) + 1) + 1;
    
    // int flag = 1;
    // int ticketSelected = 0;
    // while(flag){
    //     ticketSelected = rand() % ((totalTickets - 1) + 1) + 1;
    //     flag = tickets[ticketSelected];
    // }
    // tickets[ticketSelected] = 1;

    int sum = 0;
    for (int i = 0; i < T; i++){
        sum += functions[i].L;
        if(sum >= ticketSelected){
            return i;
        }
    }
    return 0;
}


int main(){
    srand(time(NULL));   // Seed para el random
    readConsole();


    start(functions[0].c, &pi, &functions[0], stacks[0].stack+N);
    while(next(functions[0].c)) {
        printf("%Lf \n", functions[0].sum);
    }
    //printf("------------------------------------\n");
    start(functions[1].c, &napierian, &functions[1], stacks[1].stack+N);
    while(next(functions[1].c)) {
        printf("%Lf \n", functions[1].sum);
    }


    free(functions);
    free(stacks);
    return 0;
}
