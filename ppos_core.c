#include "queue.h"
#include "ppos.h"
#include <stdio.h>
#include <stdlib.h>

task_t* Main, *Atual;
static int id_global = -1;

int new_id(){
    //printf("new id\n");
    id_global++;
    return id_global;
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

        *task = *new_task;

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
    task_switch(Main);
}

// retorna o identificador da tarefa corrente (main deve ser 0)
int task_id (){
    //printf("task_id\n");
    return Atual->id;
}
