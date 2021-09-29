#include "coroutine.h"
#include "threadSimulator.h"

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion que crea la ventana de interfaz para las series de taylor
GtkWidget *createWindow(int width,int height){
    GtkWidget *window;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL); // Define la ventana
    gtk_window_set_title(GTK_WINDOW(window),"TAYLOR SERIES"); // Nombre de la ventana
    g_signal_connect(window,"destroy",gtk_main_quit,window);
    gtk_window_set_default_size(GTK_WINDOW(window),width,height); // Define las dimensiones de la ventana
    gtk_container_set_border_width(GTK_CONTAINER(window),50);
    return window;
}

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion que le aplica el css a la ventana de GTK
void load_css(){
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    const gchar *file = "style.css"; // Carga archivo con estilos
    GFile *css_fp = g_file_new_for_path(file);
    GError *error = 0;

    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);

    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_file(provider,css_fp,&error); // Aplica los estilos

    g_object_unref(provider);
}

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion que actualiza la barra de progreso y la suma en la interfaz
void updateSum(Function *function){
    GtkOverlay *overlay = GTK_OVERLAY(function->bar->overlay);
    GtkWidget *progressBar = gtk_bin_get_child(GTK_BIN(overlay));
    GtkLabel *label = NULL;
    GList *children = gtk_container_get_children(GTK_CONTAINER(overlay));

    while(children){
        if(GTK_IS_LABEL(children->data)){
            label = children->data; //Obtiene el widget donde poner la nueva data
            break;
        }
        children = children->next;
    }

    gdouble fraction = gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(progressBar));
    gchar *buffer = g_strdup_printf("Progress: %.0f%%  Sum: %0.9Le",fraction*100,function->sum); // El porcentaje por el que va y la suma hasta el momento
    gtk_label_set_label(label,buffer);
    g_free(buffer);
}

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion que 
void set_fract_callback(Function *function){
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
                gchar *buffer1 = g_strdup_printf("DONE - Sum: %0.9Le",function->sum);
                gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressBar),"100%");
                gtk_label_set_label(label,buffer1);
                g_free(buffer1);
            }
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar),fraction);
        }
        g_free(buffer);
    }   
}

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion que maneja los widgets de progeso para cada serie de taylor
gboolean handle_progress(GtkProgressBar *progressBar){
    gtk_progress_bar_pulse(progressBar);
    gtk_progress_bar_set_pulse_step(progressBar,0.5);
    return TRUE;
}

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion que lee los parametros de la consola
void readConsole(){
    int L,F,X;
    long int M;
    totalTickets = 0;
    scanf("%d",&T); // Lee la T (cantidad de funciones siguientes que se deben de leer)
    functions =  (Function*) malloc(T * sizeof(Function));
    stacks = (Stack*) malloc(T * sizeof(Stack));
    //int row = 0;
    for(int i = 0; i < T; i++)
    {
        X = 0;
        scanf("%d %ld %d", &L, &M, &F);

        if(F != 1) // Si la funcion no es Pi se debe de leer el X
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
}

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion para representar la serie de taylor para Pi
void pi(void* p){
    Function* f = (Function*)p; // Guarda el puntero de la corutina
    long int i = 1;
    long double n;
    for(n = 1; n <= f -> M; n++,i++){
        if (n == f->M){ // Cuando sea la ultima iteracion remueva los tickets del total y incremente el contador de series completadas
            done++;
            totalTickets -= f -> L;
            f -> L = 0;
        }
        f->sum += 1/(n*n);

        if(i==f->n){
            gtk_button_clicked(GTK_BUTTON(f->bar->widget)); // boton invisible para indicar que termino el calculo de la serie
            i=0;
        }
        else    gtk_button_clicked(GTK_BUTTON(f->bar->progress)); // boton invisible para refrescar la interfaz
        
        yield(f->c); // "Retorna" la corutina (vease uso de comillas)
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion para representar la serie de taylor para logaritmo natural
void napierian(void* p){
    Function* f = (Function*)p; // Guarda el puntero de la corutina
    long int i = 1;
    long double n;
    int signo = 1;
    long upper = 1;
    for(n = 1; n <= f -> M; n++,i++){
        if (n == f->M){ // Cuando sea la ultima iteracion remueva los tickets del total y incremente el contador de series completadas
            done++;
            totalTickets -= f -> L;
            f -> L = 0;
        }
        upper *= f->X;
        f->sum += (upper / n) * signo;

        signo *= -1;
        if(i==f->n){
            gtk_button_clicked(GTK_BUTTON(f->bar->widget)); // boton invisible para indicar que termino el calculo de la serie
            i=0;
        }

        else    gtk_button_clicked(GTK_BUTTON(f->bar->progress));// boton invisible para refrescar la interfaz
        yield(f->c); // "Retorna" la corutina (vease uso de comillas)

    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion para representar la serie de taylor para euler elevado a la X
void euler(void* p){
    Function* f = (Function*)p; // Guarda el puntero de la corutina
    long int i = 1;
    long double m,n;
    long double d,div;
    for (n = 0; n <= f -> M; n++,i++) {
        if (n == f->M) { // Cuando sea la ultima iteracion remueva los tickets del total y incremente el contador de series completadas
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
        if (newN > (long double)1753) { // El calculo maximo para un factorial que cabe en un long double es de 1754
            newN = (long double)1754;
        }
        for (m = 1; m <= newN; m++) {
            div = div * m;
        }
        f->sum += (d / div);
        if(i == f->n){
            gtk_button_clicked(GTK_BUTTON(f->bar->widget)); // boton invisible para indicar que termino el calculo de la serie
            i = 0;
        }
        else    gtk_button_clicked(GTK_BUTTON(f->bar->progress)); // boton invisible para refrescar la interfaz
        yield(f->c); // "Retorna" la corutina (vease uso de comillas)
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion para representar la serie de taylor de seno
void _sin(void* p){
    Function* f = (Function*)p; // Guarda el puntero de la corutina
    long int i = 1;
    long double m,n;
    long double d,div;
    int signo = -1;
    for(n=0; n <= f->M ; n++){
        if (n == f->M){ // Cuando sea la ultima iteracion remueva los tickets del total y incremente el contador de series completadas
            done++;
            totalTickets -= f -> L;
            f -> L = 0;
        }
        d = 1;
        for(m = 0; m < 2*n+1 ; m++){
            d = d * f->X;
        }
        div = 1;
        long double newN = 2*n+1;
        if (newN > (long double)1753){ // El calculo maximo para un factorial que cabe en un long double es de 1754
            newN = (long double)1754;
        }
        for(m = 1; m <= newN; m++){
            div = div * m;
        }
        signo *= -1;
        f->sum+= (d / div) * signo;
        if(i==f->n){
            gtk_button_clicked(GTK_BUTTON(f->bar->widget)); // boton invisible para indicar que termino el calculo de la serie
            i=0;
        }
        else gtk_button_clicked(GTK_BUTTON(f->bar->progress)); // boton invisible para refrescar la interfaz
        
        yield(f->c); // "Retorna" la corutina (vease uso de comillas)
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion para escoger la funcion que gano la loteria
int chooseFunction(){
    int ticketSelected = rand() % ((totalTickets - 1) + 1) + 1; //Random de 1 a l total de tiquetes
    int sum = 0;
    for (int i = 0; i < T; i++){
        sum += functions[i].L;
        if(sum >= ticketSelected){ //Suma una acumulada en base a los tiquetes y en donde entre es porque esa funcion gano la loteria
            return i;
        }
    }
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion para que muestra en consola los resultados de los calculos de las series de taylor
void gui(){  // Mostrar solo 20
    for(int i = 0; i < T; i++){
        printf("%Lf \n", functions[i].sum);
    }
    printf("------------------------------------\n");
    printf("------------------------------------\n");
}


//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion que comienza las corutinas de cada una de las series de taylor
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

//---------------------------------------------------------------------------------------------------------------------------------------
//Funcion que lleva el control de todo el programa (sacar un tiquete, ejecutar la corutina, terminar la loteria)
gboolean lotteryLoop(){
    if(done >= T){ // Si ya todas las series terminaron se termina el programa
        printf("END\n");
        return FALSE;
    }
    int choosedFunc = 0;
    choosedFunc = chooseFunction();
    next(functions[choosedFunc].c); //Obtiene el siguente valor al hacer la corutina, si devuelve falso es porque ya termino

    // gui();
    return TRUE;
}

//---------------------------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    srand(time(NULL)); // Seed para el random
	gtk_init(&argc, &argv);
    load_css();

    done = 0;
	window = createWindow(1900,1000); // Crea ventana
    grid = gtk_grid_new();

    gtk_grid_set_column_homogeneous(GTK_GRID(grid),TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid),FALSE);
    gtk_container_add(GTK_CONTAINER(window),grid);

    readConsole(); //Lee por consola
    startLottery(); //Inicializa las corutinas

    g_timeout_add(1,(GSourceFunc)lotteryLoop,NULL); // Llama el control del juego cada 1 milisegundo

    gtk_widget_show_all(window); //Muestra la ventana
    gtk_main();

    free(functions);
    free(stacks);
	return 0;
}

