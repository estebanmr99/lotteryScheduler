#include "coroutine.h"
#include <math.h>
#include <unistd.h>
#define N 2048

int totalTickets;
int T;
int done = 0;

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

void pi(void* p){
  
    Function* f = (Function*)p;
    long double n;
    for(n = 1; n <= f -> M; n++){
        if (n == f->M){
            done++;
            totalTickets -= f -> L;
            f -> L = 0;
        }
        f->sum += 1/(n*n);
        yield(f->c);
    }
}

void napierian(void* p){
    Function* f = (Function*)p;
    long double n;
    int signo = 1;
    long upper = 1;
    for(n = 1; n <= f -> M; n++){
        if (n == f->M){
            done++;
            totalTickets -= f -> L;
            f -> L = 0;
        }
        upper *= f->X;
        f->sum += (upper / n) * signo;

        signo *= -1;
        yield(f->c);
        
    }
}

void euler(void* p){ // Que vergas hacer con el factorial a partir de 50
    done++;
}

void _sin(void* p){ // Que vergas hacer con el factorial a partir de 50
    Function* f = (Function*)p;
    long double n,m;
    long double d,div;
    int signo = -1;
    for(n=0; n <= f->M ; n++){
        if (n == f->M){
            done++;
            totalTickets -= f -> L;
            f -> L = 0;
        }
        d = 1;
        for(m = 0; m < 2*n+1 ; m++){
            d = d * f->X;
        }
        div = 1;
        for(m = 1; m <= 2*n+1 ; m++){
            div = d * m;
        }
        signo *= -1;
        f->sum+= (d / div) * signo;
        yield(f->c);     
    }
}

int chooseFunction(){ // Preguntarle a eddy si se puede repetir un ticket
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

void gui(){
    for(int i = 0; i < T; i++){
        printf("%Lf \n", functions[i].sum);
    }
    printf("------------------------------------\n");
    printf("------------------------------------\n");
}

void startLottery(){

    for(int i = 0; i < T; i++){
        int func = functions[i].F;

        if(func == 1){
            start(functions[i].c, &pi, &functions[i], stacks[i].stack+N);
        } else if(func == 2){
            start(functions[i].c, &napierian, &functions[i], stacks[i].stack+N);
        } else if (func == 3){
            start(functions[i].c, &euler, &functions[i], stacks[i].stack+N);
        } else if(func == 4){
            start(functions[i].c, &_sin, &functions[i], stacks[i].stack+N);
        }
    }

    int choosedFunc = 0;

    while (done < T)
    {
        choosedFunc = chooseFunction();
        next(functions[choosedFunc].c);
        gui();
    }
}

int main(){
    srand(time(NULL));   // Seed para el random
    readConsole();

    startLottery();

    free(functions);
    free(stacks);
    return 0;
}