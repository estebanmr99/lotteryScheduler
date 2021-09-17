#include "coroutine.h"

int totalTickets;
int T;

typedef struct Function{
    coroutine* c;
    int L;
    long int M;
    int F;
    int X;
    long double sum;
} Function;

typedef struct {
    coroutine* c;
    int max_x, max_y;
    int x, y;
} iter;

Function *functions;

void readConsole(){
    int L,F,X;
    long int M;
    totalTickets = 0;

    scanf("%d",&T);
    functions =  (Function*) malloc(T * sizeof(Function)); // Tiene que haber un puntero de stacks

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

#define N 1024

int main(){
    srand(time(NULL));   // Seed para el random
    readConsole();

    // coroutine c;
    // int stack[N]; // Tiene que haber un puntero de stacks
    // iter it = {&c, 2, 2};
    // start(&c, &iterate, &it, stack+N);
    // while(next(&c)) {
    //     printf("%d %d\n", it.x, it.y);
    // }

    free(functions);
    return 0;
}
