#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>
#include "coroutine.h"
#include <math.h>
#define N 2048

int totalTickets;
int T;
int done = 0;

GtkWidget *window;
GtkWidget *grid;

typedef struct ProgressBars{
    GtkWidget *bar;
    GtkWidget *label;
    GtkWidget *overlay;
    GtkWidget *widget;
    GtkWidget *progress;
}ProgressBars;

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

typedef struct Stack{
    int stack[N];
} Stack;

Function *functions;
Stack *stacks;

GtkWidget *createWindow(int width,int height){
    GtkWidget *window;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),"TAYLOR SERIES");
    g_signal_connect(window,"destroy",gtk_main_quit,window);
    gtk_window_set_default_size(GTK_WINDOW(window),width,height);
    gtk_container_set_border_width(GTK_CONTAINER(window),50);
    return window;
}

void load_css(){
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    const gchar *file = "style.css"; 
    GFile *css_fp = g_file_new_for_path(file);
    GError *error = 0;

    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);

    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_file(provider,css_fp,&error);

    g_object_unref(provider);

}

void updateSum(Function *function){
    GtkOverlay *overlay = GTK_OVERLAY(function->bar->overlay);
    GtkWidget *progressBar = gtk_bin_get_child(GTK_BIN(overlay));
    GtkLabel *label = NULL;
    GList *children = gtk_container_get_children(GTK_CONTAINER(overlay));

    while(children){
        if(GTK_IS_LABEL(children->data)){
            label = children->data;
            break;
        }
        children = children->next;
    }

    gdouble fraction = gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(progressBar));
    gchar *buffer = g_strdup_printf("Progress: %.0f%%  Sum: %0.9Le",fraction*100,function->sum);
    gtk_label_set_label(label,buffer);
    g_free(buffer);
}


void set_fract_callback(Function *function){
    //printf("hoooollaaa");
    GtkOverlay *overlay = GTK_OVERLAY(function->bar->overlay);
    GtkWidget *progressBar = gtk_bin_get_child(GTK_BIN(overlay));
    GtkLabel *label = NULL;
    GList *children = gtk_container_get_children(GTK_CONTAINER(overlay));
    gdouble fraction = 0.0;


    while(children){
        if(GTK_IS_LABEL(children->data)){
            label = children->data;
            break;
        }
        children = children->next;
    }

    if(GTK_IS_PROGRESS_BAR(progressBar)){
        fraction = gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(progressBar));
        fraction += 0.01;
        PangoEllipsizeMode ellipSize = gtk_progress_bar_get_ellipsize(GTK_PROGRESS_BAR(progressBar));
        if(ellipSize == PANGO_ELLIPSIZE_NONE){
            gtk_progress_bar_set_ellipsize(GTK_PROGRESS_BAR(progressBar),PANGO_ELLIPSIZE_END);
        }
        gchar *buffer = g_strdup_printf("Progress: %.0f%%  Sum: %0.9Le",fraction*100,function->sum);
        if(fraction< 1.0){
            if(label!=NULL){
                gtk_label_set_label(label,buffer);
            }
            else{
                gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progressBar),TRUE);
                gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressBar),buffer);
            }
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar),fraction);

        }
        else{
            if(label!=NULL){
                gchar *buffer1 = g_strdup_printf("DONE  Sum: %0.9Le",function->sum);
                gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressBar),"100%");
                gtk_label_set_label(label,buffer1);
                g_free(buffer1);
            }
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar),fraction);
        }
        g_free(buffer);
    }   
}


gboolean handle_progress(GtkProgressBar *progressBar){

    gtk_progress_bar_pulse(progressBar);
    gtk_progress_bar_set_pulse_step(progressBar,0.5);
   // gtk_progress_bar_set_show_text(progressBar,TRUE);
    return TRUE;
}

gboolean test(){
    printf("hola\n");
    return TRUE;
}

void readConsole(){
    int L,F,X;
    long int M;
    totalTickets = 0;
    scanf("%d",&T);
    functions =  (Function*) malloc(T * sizeof(Function));
    stacks = (Stack*) malloc(T * sizeof(Stack));
    //int row = 0;
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
        functions[i].n = M * 0.01; //define el 1% de la barra
        functions[i].bar = (ProgressBars*) malloc(sizeof(ProgressBars));
        functions[i].bar->bar = gtk_progress_bar_new();
        functions[i].bar->overlay = gtk_overlay_new();
        functions[i].bar->label = gtk_label_new("progressing...0%");
        functions[i].bar->widget = gtk_button_new_with_mnemonic("Click");
        functions[i].bar->progress = gtk_button_new_with_mnemonic("Click");
        g_object_set(functions[i].bar->bar,"margin-top",15,NULL);
        g_object_set(functions[i].bar->overlay, "margin-top",15,NULL);
        gtk_grid_attach(GTK_GRID(grid),functions[i].bar->overlay,0,i,1,1);
        gtk_container_add(GTK_CONTAINER(functions[i].bar->overlay),functions[i].bar->bar);
        gtk_overlay_add_overlay(GTK_OVERLAY(functions[i].bar->overlay),functions[i].bar->label);
        g_signal_connect_swapped(functions[i].bar->widget,"clicked",G_CALLBACK(set_fract_callback),(gpointer)&functions[i]);
        g_signal_connect_swapped(functions[i].bar->progress,"clicked",G_CALLBACK(updateSum),(gpointer)&functions[i]);

    }        
    
    //for(int i = 0; i < T; i++)
    //    printf("L: %d M: %ld F: %d X: %d\n",functions[i].L,functions[i].M,functions[i].F,functions[i].X);
}

void pi(void* p){
    Function* f = (Function*)p;
    long int i = 1;
    long double n;
    for(n = 1; n <= f -> M; n++,i++){
        if (n == f->M){
            done++;
            totalTickets -= f -> L;
            f -> L = 0;
        }
        f->sum += 1/(n*n);

        if(i==f->n){
            gtk_button_clicked(GTK_BUTTON(f->bar->widget));
            i=0;
        }
        else    gtk_button_clicked(GTK_BUTTON(f->bar->progress));
        
        yield(f->c);
    }
}

void napierian(void* p){
    Function* f = (Function*)p;
    long int i = 1;
    long double n;
    int signo = 1;
    long upper = 1;
    for(n = 1; n <= f -> M; n++,i++){
        if (n == f->M){
            done++;
            totalTickets -= f -> L;
            f -> L = 0;
        }
        upper *= f->X;
        f->sum += (upper / n) * signo;

        signo *= -1;
        if(i==f->n){
            gtk_button_clicked(GTK_BUTTON(f->bar->widget));
            i=0;
        }

        else    gtk_button_clicked(GTK_BUTTON(f->bar->progress));
        //gtk_button_clicked(GTK_BUTTON(f->bar->button));
        
        yield(f->c);

    }
}

void euler(void* p){ // EL factorial no debe de calcularse a mas de 1754
    Function* f = (Function*)p;
    long int i = 1;
    long double m,n;
    long double d,div;
    for (n = 0; n <= f -> M; n++,i++) {
        if (n == f->M) {
            done++;
            totalTickets -= f -> L;
            f -> L = 0;
        }
        d = 1;
        for (m = 0; m < n; m++) {
            d = d * f->X;
        }
        div = 1;
        long double newN = n;
        if (newN > (long double)1753) {
            newN = (long double)1754;
        }
        for (m = 1; m <= newN; m++) {
            div = div * m;
        }
        f->sum += (d / div);
        if(i == f->n){
            gtk_button_clicked(GTK_BUTTON(f->bar->widget));
            i = 0;
        }
        //gtk_button_clicked(GTK_BUTTON(f->bar->button));
        else    gtk_button_clicked(GTK_BUTTON(f->bar->progress));
        yield(f->c);
    }
}

void _sin(void* p){
    Function* f = (Function*)p;
    long int i = 1;
    long double m,n;
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
        long double newN = 2*n+1; // EL factorial no debe de calcularse a mas de 1754
        if (newN > (long double)1753){
            newN = (long double)1754;
        }
        for(m = 1; m <= newN; m++){
            div = div * m;
        }
        signo *= -1;
        f->sum+= (d / div) * signo;
        if(i==f->n){
            gtk_button_clicked(GTK_BUTTON(f->bar->widget));
            i=0;
        }
        else gtk_button_clicked(GTK_BUTTON(f->bar->progress));
        
        yield(f->c);
    }
}

int chooseFunction(){
    int ticketSelected = rand() % ((totalTickets - 1) + 1) + 1;
    int sum = 0;
    for (int i = 0; i < T; i++){
        sum += functions[i].L;
        if(sum >= ticketSelected){
            return i;
        }
    }
    return 0;
}

void gui(){  // Preguntar cuanto deberia ser el maximo de numeros que se muestran (porque se pueden mil)
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


}
gboolean lotteryLoop(){
    if(done>=T){
        printf("END\n");
        return FALSE;
    }
    int choosedFunc = 0;
    choosedFunc = chooseFunction();
    next(functions[choosedFunc].c);

    // gui();
    //printf("done: %d\n",done);
    
    return TRUE;
 
}

int main(int argc, char* argv[])
{
	gtk_init(&argc, &argv);
   
    load_css();

	window = createWindow(1900,1000);
    grid = gtk_grid_new();

    gtk_grid_set_column_homogeneous(GTK_GRID(grid),TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid),FALSE);
    gtk_container_add(GTK_CONTAINER(window),grid);

    srand(time(NULL));   
    readConsole();
    startLottery();

    g_timeout_add(1,(GSourceFunc)lotteryLoop,NULL);

    gtk_widget_show_all(window);
    gtk_main();

    free(functions);
    free(stacks);
	return 0;
}

