#ifndef HEADER
#define HEADER

#include <math.h>
#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>

#define N 2048

// Struct para el widget de las barras de progreso
typedef struct ProgressBars{
    GtkWidget *bar;
    GtkWidget *label;
    GtkWidget *overlay;
    GtkWidget *widget;
    GtkWidget *progress;
} ProgressBars;

// Struct para cada serie de taylor
typedef struct Function{
    coroutine* c;
    int L;
    long int M;
    int F;
    int X;
    long double sum;
    ProgressBars *bar;
    long int n;
} Function;

// Struct para representar pilas para cada una de las series de taylor
typedef struct Stack{
    int stack[N];
} Stack;

int totalTickets;
int T;
int done;
Function *functions;
Stack *stacks;
GtkWidget *window;
GtkWidget *grid;

GtkWidget *createWindow(int width,int height);
void load_css();
void updateSum(Function *function);
void set_fract_callback(Function *function);
gboolean handle_progress(GtkProgressBar *progressBar);
void readConsole();
void pi(void* p);
void napierian(void* p);
void euler(void* p);
void _sin(void* p);
int chooseFunction();
void gui();
void startLottery();
gboolean lotteryLoop();
int main();

#endif