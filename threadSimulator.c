#include <stdio.h>
#include <stdlib.h>
typedef struct Function{
   int L;
   int M;
   int F;
   int X;
}Function;

Function *functions;

int main(){

    int T,L,M,F,X;    
    scanf("%d",&T);
    functions =  (Function*) malloc(T*sizeof(Function));

    for(int i = 0; i < T; i++)
    {
        X=0;
        scanf("%d %d %d",&L,&M,&F);
        printf("%d %d %d \n",L,M,F);
        if(F!=1)scanf("%d",&X);
        functions[i].L=L;
        functions[i].M=M;
        functions[i].F=F;
        functions[i].X=X;
    }

    for(int i = 0; i < T; i++)
        printf("L: %d M: %d F: %d X: %d \n",functions[i].L,functions[i].M,functions[i].F,functions[i].X);

    return 0;
    
}