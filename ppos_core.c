#include "queue.h"
#include "ppos.h"
#include <stdio.h>
#include <stdlib.h>

task_t* Main, *Atual;
task_t Dispatcher;
queue_t* Fila;
static int id_global = -1;
static int userTasks = -1;

int new_id(){
    //printf("new id\n");
    id_global++;
    return id_global;
}

// retorna a proxima tarefa a ser executada
task_t* scheduler(){
    task_t* prev = (task_t*)(Fila);
    Fila = (queue_t*)(Fila->next);
    return prev;
}

// controla a ordem de execução das tarefas
void dispatcher(void *arg){
    while(userTasks > 0){
        task_t* next = scheduler();
        if(next != NULL){
            task_switch(next);
            if(next->status == FINALIZADA){
                // Tira item da fila, libera stack, libera struct
                queue_remove ((queue_t**)&Fila, (queue_t*) next); 
                free(next->context.uc_stack.ss_sp);
                free(next);
                userTasks--;
            }
            // voltando ao dispatcher, trata a tarefa de acordo com seu estado
            //caso o estado da tarefa "próxima" seja:
            // PRONTA    : ...
            // TERMINADA : ...
            // SUSPENSA  : ...
            // (etc)
        }
    }
    task_exit(0);
}

// Inicializa o sistema operacional; deve ser chamada no inicio do main()
void ppos_init (){
    //printf("ppos_init\n");
    /* desativa o buffer da saida padrao (stdout), usado pela função printf */
    setvbuf (stdout, 0, _IONBF, 0) ;
    
    Main = (task_t*)malloc(sizeof(task_t));
    Main->id = new_id();
    getcontext(&(Main->context));
    Atual = Main;

    Fila = NULL;
    task_create(&Dispatcher, dispatcher, NULL);
}

// Cria uma nova tarefa. Retorna um ID> 0 ou erro.
int task_create (task_t *task,			// descritor da nova tarefa
                 void (*start_func)(void *),	// funcao corpo da tarefa
                 void *arg) 			// argumentos para a tarefa
    {
        //printf("task_create\n");
        char* stack;
        task_t* new_task = (task_t*)malloc(sizeof(task_t));
        if(new_task == NULL) return -1;
        new_task->context = Main->context;

        stack = malloc(STACKSIZE);
        
        if(stack){
            new_task->context.uc_stack.ss_sp = stack ;
            new_task->context.uc_stack.ss_size = STACKSIZE ;
            new_task->context.uc_stack.ss_flags = 0 ;
            new_task->context.uc_link = 0 ;
        } else {
            perror ("Erro na criação da pilha: ") ;
            return -1;
        }

        makecontext (&(new_task->context), (void*)start_func, 1, (void*)arg);
        new_task->id = new_id();
        
        new_task->next = NULL;
        new_task->prev = NULL;

        new_task->status = PRONTA;

        *task = *new_task;

        userTasks++;
        if(task->id > 0){
            queue_append((queue_t**)&Fila, (queue_t*)new_task);
        }
        return task->id;
    }


// alterna a execução para a tarefa indicada
int task_switch (task_t *task){
    //printf("task switch\n");
    task_t* prev = Atual;
    Atual = task;
    return swapcontext(&(prev->context), &(Atual->context));    
}

// Termina a tarefa corrente, indicando um valor de status encerramento
void task_exit (int exit_code){
    //printf("task_exit\n");
    //Liberar tarefa atual só depois
    Atual->status = FINALIZADA;
    // Alternativamente, if(Atual == &Dispatcher) task_switch(Main);
    if(userTasks > 0) task_switch(&Dispatcher);
    else task_switch(Main);
}

// retorna o identificador da tarefa corrente (main deve ser 0)
int task_id (){
    //printf("task_id\n");
    return Atual->id;
}

// libera o processador para a próxima tarefa, retornando à fila de tarefas
// prontas ("ready queue")
void task_yield (){
    task_switch(&Dispatcher);
}

