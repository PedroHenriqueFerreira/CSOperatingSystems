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

typedef enum { 
    FALSE = 0,
    TRUE = 1,
} bool;

// Tempo de simulação
int simulation_time = 0;

// Estrutura de tarefas
typedef struct {
    int id;
    int P;
    int C;
    int D;
    int remaining_c_in_p;
    int next_p;
} T;

// Vetor de tarefas
T tasks[100];

// Vetor de tarefas original
T tasks_original[100];

// Número de tarefas
int tasks_number = 0;

// Vetor de ID's de tarefas escalonadas pelo RM (Usado para gerar o gráfico)
int* tasks_scheduled_RM;

// Vetor de ID's de tarefas escalonadas pelo EDF (Usado para gerar o gráfico)
int* tasks_scheduled_EDF;

// Reseta as tarefas para o próximo algoritmo
void reset_tasks() {
    for(int i = 0; i < tasks_number; i++){
        tasks[i] = tasks_original[i]; 
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

// Calcula o tempo de simulação
void compute_simulation_time() {
    simulation_time = tasks[0].P;

    for(int i = 1; i < tasks_number; i++){
        simulation_time = mmc(simulation_time, tasks[i].P);
    }
}

// Calcula a soma de Ci/Pi
float get_sum() {
    float sum = 0.0;

    for(int i = 0; i < tasks_number; i++){
        sum += (float) tasks[i].C / tasks[i].P;
    }

    return sum;
}

// Checa a escalonabilidade do RM
bool check_RM_scalability() {
    float sum = get_sum();

    float test = tasks_number * (pow(2, 1.0 / tasks_number) - 1);

    if(sum <= test) return TRUE;
    
    return FALSE;
}

// Checa a escalonabilidade do EDF
bool check_EDF_scalability() {
    float sum = get_sum();

    if (sum <= 1) return TRUE;
    
    return FALSE;
}

// RM
void RM() {
    // Aloca memória para as tarefas escalonadas
    tasks_scheduled_RM = malloc(simulation_time * sizeof(int));

    // Para cada unidade de tempo
    for (int time = 0; time < simulation_time; time++) {
        // Extrai a tarefa com menor período de execução que ainda não foi executada
        T *task_now = NULL;

        for (int i = 0; i < tasks_number; i++) {
            // Se a tarefa está no período de execução e ainda tem tempo de execução
            if (time >= tasks[i].next_p && tasks[i].remaining_c_in_p > 0) {
                // Se a tarefa atual é nula ou a tarefa atual tem um período de execução menor
                if (task_now == NULL || tasks[i].P < task_now->P) {
                    task_now = &tasks[i];
                }
            }
        }

        // Se a tarefa atual não é nula
        if (task_now != NULL) {
            // Salva na lista de tarefas escalonadas a tarefa atual
            tasks_scheduled_RM[time] = task_now->id;

            // Reduz o tempo de execução da tarefa
            task_now->remaining_c_in_p--;

            // Se o tempo de execução da tarefa acabou
            if (task_now->remaining_c_in_p == 0) {
                // Atualiza o próximo período de execução da tarefa
                task_now->next_p += task_now->P;
                // Atualiza o tempo de execução da tarefa
                task_now->remaining_c_in_p = task_now->C;
            }
        } else {
            // Salva na lista de tarefas escalonadas -1
            tasks_scheduled_RM[time] = -1;
        }
    }
}

// EDF
void EDF() {
    // Aloca memória para as tarefas escalonadas
    tasks_scheduled_EDF = malloc(simulation_time * sizeof(int));

    // Para cada unidade de tempo
    for (int time = 0; time < simulation_time; time++) {
        // Extrai a tarefa com menor deadline que ainda não foi executada
        T *task_now = NULL;

        for (int i = 0; i < tasks_number; i++){
            // Se a tarefa está no período de execução e ainda tem tempo de execução
            if (time >= tasks[i].next_p && tasks[i].remaining_c_in_p > 0) {
                // Se a tarefa atual é nula ou a tarefa atual tem um deadline menor
                if (task_now == NULL || tasks[i].D < task_now->D) {
                    task_now = &tasks[i];
                }
            }

            // Reduz o deadline de cada tarefa
            if (tasks[i].D == 0) tasks[i].D = tasks_original[i].D;
            else tasks[i].D--;
        }

        // Se a tarefa atual não é nula
        if(task_now != NULL) {
            // Salva na lista de tarefas escalonadas a tarefa atual
            tasks_scheduled_EDF[time] = task_now->id;

            // Reduz o tempo de execução da tarefa
            task_now->remaining_c_in_p--;

            // Se o tempo de execução da tarefa acabou
            if (task_now->remaining_c_in_p == 0) {
                // Atualiza o próximo período de execução da tarefa
                task_now->next_p += task_now->P;
                // Atualiza o tempo de execução da tarefa
                task_now->remaining_c_in_p = task_now->C;
            }
        } else {
            // Salva na lista de tarefas escalonadas -1
            tasks_scheduled_EDF[time] = -1;
        }
    }
}

// Mostra o gráfico de tarefas escalonadas
void graphic(int* tasks_scheduled) {
    for (int task = 0; task < tasks_number; task++) {
        printf("t%d ", tasks[task].id);

        for (int time = 0; time < simulation_time; time++) {
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
    FILE *input_file;
    input_file = fopen(argv[1], "r");

    // Verifica se o arquivo de entrada é nulo e exibe uma mensagem de erro
    if (input_file == NULL) {
        perror("diretorio para o arquivo de entrada errado");
        return EXIT_FAILURE;
    }

    // Pula a primeira linha do arquivo de entrada
    fscanf(input_file, "%*[^\n]\n");

    // Variáveis para armazenar os valores de P, C e D
    int P, C, D;

    // Variável para armazenar o índice da tarefa
    int i = 0;

    // Lê o arquivo de entrada e armazena os valores de P, C e D
    while (fscanf(input_file, "%d %d %d", &P, &C, &D) != EOF) {
        tasks[i].id = i + 1;

        tasks[i].P = P;
        tasks[i].C = C;
        tasks[i].D = D;
        tasks[i].remaining_c_in_p = C;
        tasks[i].next_p = 0;
        tasks_original[i] = tasks[i];
        i++;
    }

    // Fecha o arquivo de entrada
    fclose(input_file);
    
    // Armazena o número de tarefas
    tasks_number = i;

    // Calcula o tempo de simulação
    compute_simulation_time();

    // Verifica se o sistema é escalonável pelo RM
    if (check_RM_scalability() == FALSE) printf("RM falhou no teste de escalonabilidade\n\n");
    else printf("RM passou no teste de escalonabilidade\n\n");
    // Executa o RM
    RM();
    // Mostra o gráfico de tarefas escalonadas
    printf("Escalonamento RM: \n");
    graphic(tasks_scheduled_RM);

    // Reseta as tarefas para o próximo algoritmo
    reset_tasks();

    // Verifica se o sistema é escalonável pelo EDF
    if (check_EDF_scalability() == FALSE) printf("EDF falhou no teste de escalonabilidade\n\n");
    else printf("EDF passou no teste de escalonabilidade\n\n");
    // Executa o EDF
    EDF();
    // Mostra o gráfico de tarefas escalonadas
    printf("Escalonamento EDF: \n");
    graphic(tasks_scheduled_EDF);

    return 0;
}