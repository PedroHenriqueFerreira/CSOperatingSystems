#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
[pronto]
1.0 - Crie um programa em C que implemente os algoritmos de escalonamento [pronto]
    1.1 - RM (RM) [pronto]
    1.2 - EDF (EDF) [pronto]
2.0 - O programa também deve fazer os testes de escalonabilidade [pronto]
3.0 - mostrar uma escala de tempo de execução [pronto]
    3.1 - (Semelhantes aos Slides de Aula) grafico [pronto].
4.0 - O programa deve ler o sitema de um arquivo .txt [pronto]
    4.1 - períodos [pronto],
    2.2 - tempos de execução no pior caso [pronto]
    2.3 - deadlines[pronto].
*/

// Enumeração de booleanos
typedef enum { 
    FALSE = 0,
    TRUE = 1,
} bool;

// Estrutura de tarefas
typedef struct {
    int id; // Identificador da tarefa
    int P; // Período de execução
    int next_P; // Próximo período de execução
    int C; // Tempo de execução
    int remaining_C; // Tempo de execução restante
    int D; // Deadline
    int remaining_D; // Deadline restante
} T;

// Reseta as tarefas
void reset_tasks(T* tasks, int tasks_number) {
    for (int i = 0; i < tasks_number; i++) {
        tasks[i].remaining_C = tasks[i].C;
        tasks[i].remaining_D = tasks[i].D;
        tasks[i].next_P = 0;
    }
}

// Calcula o MDC de dois números
int mdc(int a, int b) {
    if (b == 0) return a;
    return mdc(b, a % b);
}

// Calcula o MMC de dois números
int mmc(int a, int b) {
    return a * b / mdc(a, b);
}

// Calcula o tempo total
int compute_total_time(T* tasks, int tasks_number) {
    int total_time = tasks[0].P;

    for (int i = 1; i < tasks_number; i++) {
        total_time = mmc(total_time, tasks[i].P);
    }

    return total_time;
}

// Calcula a soma de Ci/Pi
float compute_sum(T* tasks, int tasks_number) {
    float sum = 0.0;

    for(int i = 0; i < tasks_number; i++){
        sum += (float) tasks[i].C / tasks[i].P;
    }

    return sum;
}

// Checa a escalonabilidade do RM
bool check_RM_scalability(T* tasks, int tasks_number) {
    float limit = tasks_number * (pow(2, 1.0 / tasks_number) - 1);

    return compute_sum(tasks, tasks_number) <= limit;
}

// Checa a escalonabilidade do EDF
bool check_EDF_scalability(T* tasks, int tasks_number) {
    return compute_sum(tasks, tasks_number) <= 1;
}

// Atualiza o deadline de cada tarefa
void update_remaining_D(T* tasks, int tasks_number) {
    // Reduz o deadline de cada tarefa
    for (int i = 0; i < tasks_number; i++){
        if (tasks[i].remaining_D == 0) tasks[i].remaining_D = tasks[i].D;
        else tasks[i].remaining_D--;
    }
}

// Atualiza o tempo de execução da tarefa
void update_remaining_C(T *task) {
    // Reduz o tempo de execução da tarefa
    task->remaining_C--;

    // Se o tempo de execução da tarefa acabou
    if (task->remaining_C == 0) {
        // Atualiza o próximo período de execução da tarefa
        task->next_P += task->P;
        // Atualiza o tempo de execução da tarefa
        task->remaining_C = task->C;
    }
}

// RM
void RM(T* tasks, int tasks_number, int total_time, int* tasks_schedule_RM) {
    // Para cada unidade de tempo
    for (int time = 0; time < total_time; time++) {
        // Extrai a tarefa com menor período de execução que ainda não foi executada
        T *task_now = NULL;

        for (int i = 0; i < tasks_number; i++) {
            // Se a tarefa está no período de execução e ainda tem tempo de execução
            if (time >= tasks[i].next_P && tasks[i].remaining_C > 0) {
                // Se a tarefa atual é nula ou a tarefa atual tem um período de execução menor
                if (task_now == NULL || tasks[i].P < task_now->P) {
                    task_now = &tasks[i];
                }
            }
        }

        // Se a tarefa atual não é nula
        if (task_now != NULL) {
            // Salva na lista de tarefas escalonadas a tarefa atual
            tasks_schedule_RM[time] = task_now->id;
            // Atualiza o tempo de execução da tarefa
            update_remaining_C(task_now);
        } else {
            // Salva na lista de tarefas escalonadas -1
            tasks_schedule_RM[time] = -1;
        }
    }
}

// EDF
void EDF(T* tasks, int tasks_number, int total_time, int* tasks_schedule_EDF) {
    // Para cada unidade de tempo
    for (int time = 0; time < total_time; time++) {
        // Extrai a tarefa com menor deadline que ainda não foi executada
        T *task_now = NULL;

        for (int i = 0; i < tasks_number; i++){
            // Se a tarefa está no período de execução e ainda tem tempo de execução
            if (time >= tasks[i].next_P && tasks[i].remaining_C > 0) {
                // Se a tarefa atual é nula ou a tarefa atual tem um deadline menor
                if (task_now == NULL || tasks[i].remaining_D < task_now->remaining_D) {
                    task_now = &tasks[i];
                }
            }
        }

        // Reduz o deadline de cada tarefa
        update_remaining_D(tasks, tasks_number);

        // Se a tarefa atual não é nula
        if(task_now != NULL) {
            // Salva na lista de tarefas escalonadas a tarefa atual
            tasks_schedule_EDF[time] = task_now->id;
            // Atualiza o tempo de execução da tarefa
            update_remaining_C(task_now);
        } else {
            // Salva na lista de tarefas escalonadas -1
            tasks_schedule_EDF[time] = -1;
        }
    }
}

// Mostra o gráfico de tarefas escalonadas
void plot(T* tasks, int tasks_number, int total_time, int* tasks_scheduled) {
    for (int task = 0; task < tasks_number; task++) {
        printf("t%d ", tasks[task].id);

        for (int time = 0; time < total_time; time++) {
            printf(tasks_scheduled[time] == tasks[task].id ? "#" : "-");
        }

        printf("\n");
    }

    printf("\n");
}

int main(int argc, char *argv[]){
    // Verifica se o número de argumentos é menor que 2 e exibe uma mensagem de erro
    if (argc < 2) {
        fprintf(stderr, "Comando previsto: %s SistemaTestes/sistemaX.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Abre o arquivo de entrada
    FILE *file = fopen(argv[1], "r");

    // Verifica se o arquivo de entrada é nulo e exibe uma mensagem de erro
    if (file == NULL) {
        perror("diretorio para o arquivo de entrada errado");
        return EXIT_FAILURE;
    }

    // Vetor de tarefas
    T tasks[100];

    // Variáveis para armazenar os valores de P, C e D
    int P, C, D;

    // Variável para armazenar a quantidade de tarefas
    int tasks_number = 0;

    // Pula a primeira linha do arquivo de entrada
    fscanf(file, "%*[^\n]\n");

    // Lê o arquivo de entrada e armazena os valores de P, C e D
    while (fscanf(file, "%d %d %d", &P, &C, &D) != EOF) {
        tasks[tasks_number].id = tasks_number + 1;

        tasks[tasks_number].P = P;
        tasks[tasks_number].C = C;
        tasks[tasks_number].D = D;

        tasks[tasks_number].remaining_C = C;
        tasks[tasks_number].remaining_D = D;
        tasks[tasks_number].next_P = 0;

        tasks_number++;
    }

    // Fecha o arquivo de entrada
    fclose(file);

    // Calcula o tempo total de simulação
    int total_time = compute_total_time(tasks, tasks_number);

    // Verifica se o sistema é escalonável pelo RM
    switch (check_RM_scalability(tasks, tasks_number)) {
        case TRUE:
            printf("RM passou no teste de escalonabilidade\n\n");
            break;
        case FALSE:
            printf("RM falhou no teste de escalonabilidade\n\n");
            break;
    }

    // Vetor com as tarefas escalonadas pelo RM (Usado para gerar o gráfico)
    int* tasks_schedule_RM = malloc(total_time * sizeof(int));

    // Executa o RM
    RM(tasks, tasks_number, total_time, tasks_schedule_RM);
    // Mostra o gráfico de tarefas escalonadas
    printf("Escalonamento RM: \n");
    plot(tasks, tasks_number, total_time, tasks_schedule_RM);

    // Reseta as tarefas para o próximo algoritmo
    reset_tasks(tasks, tasks_number);

    // Verifica se o sistema é escalonável pelo EDF
    switch (check_EDF_scalability(tasks, tasks_number)) {
        case TRUE:
            printf("EDF passou no teste de escalonabilidade\n\n");
            break;
        case FALSE:
            printf("EDF falhou no teste de escalonabilidade\n\n");
            break;
    }

    // Vetor com as tarefas escalonadas pelo EDF (Usado para gerar o gráfico)
    int* tasks_schedule_EDF = malloc(total_time * sizeof(int));

    // Executa o EDF
    EDF(tasks, tasks_number, total_time, tasks_schedule_EDF);
    // Mostra o gráfico de tarefas escalonadas
    printf("Escalonamento EDF: \n");
    plot(tasks, tasks_number, total_time, tasks_schedule_EDF);

    return 0;
}