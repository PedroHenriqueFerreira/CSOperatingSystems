#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
[pronto]
1.0 - Crie um programa em C que implemente os algoritmos de escalonamento [pronto]
    1.1 - RM (Rate Monotonic) [pronto]
    1.2 - EDF (Earliest Deadline First) [pronto]
2.0 - O programa também deve fazer os testes de escalonabilidade [pronto]
3.0 - mostrar uma escala de tempo de execução [pronto]
    3.1 - (Semelhantes aos Slides de Aula) grafico em python [opcional][pronto].
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
void reset_tasks(){
    for(int i = 0; i < tasks_number; i++){
        tasks[i] = tasks_original[i]; 
    }
}

// Calcula a soma de Ci/Pi
float get_sum(){
    float sum = 0.0;

    for(int i = 0; i < tasks_number; i++){
        sum += (float) tasks[i].C / tasks[i].P;
    }

    return sum;
}

// Checa a escalonabilidade do Rate Monotonic
int check_RM_scalability(){
    float sum = get_sum();

    float test = tasks_number * (pow(2, 1.0 / tasks_number) - 1);

    if(sum <= test) return TRUE;
    
    return FALSE;
}

// Checa a escalonabilidade do Earliest Deadline First
int check_EDF_scalability(){
    float sum = get_sum();

    if (sum <= 1) return TRUE;
    
    return FALSE;
}

// Rate Monotonic
void RM(int scalability) {
    if(scalability == FALSE) return;

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
            // Salva no arquivo de saída o algoritmo, o tempo e a tarefa
            // fprintf(output_file, "RM\t%d\t%d\n", time + 1, task_now->id);

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
            // Se a tarefa atual é nula, salva no arquivo de saída o algoritmo, o tempo e -1
            // fprintf(output_file, "RM\t%d\t-1\n", time+1);

            // Salva na lista de tarefas escalonadas -1
            tasks_scheduled_RM[time] = -1;
        }
    }
}

// Earliest Deadline First
void EDF(int scalability) {
    if(scalability == FALSE) return;

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
            // Salva no arquivo de saída o algoritmo, o tempo e a tarefa
            // fprintf(output_file, "EDF\t%d\t%d\n", time + 1, task_now->id);

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
            // Se a tarefa atual é nula, salva no arquivo de saída o algoritmo, o tempo e -1
            // fprintf(output_file, "EDF\t%d\t-1\n", time + 1);

            // Salva na lista de tarefas escalonadas -1
            tasks_scheduled_EDF[time] = -1;
        }
    }
}

// Mostra o gráfico de tarefas escalonadas
void print_graphic(int* tasks_scheduled) {
    for (int task = 0; task < tasks_number; task++) {
        printf("t%d ", tasks[task].id);

        for (int time = 0; time < simulation_time; time++) {
            if (tasks_scheduled[time] == tasks[task].id) {
                printf("#");
            } else {
                printf("-");
            }
        }

        printf("\n");
    }
}

int main(int argc, char *argv[]){
    // Verifica se o número de argumentos é menor que 3 e exibe uma mensagem de erro
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

    // Busca o maior período de execução para definir o tempo de simulação
    for (int i = 0; i < tasks_number; i++) {
        if(tasks[i].P >= simulation_time){
            simulation_time = tasks[i].P;
        }
    }

    // Abre o arquivo de saída
    // FILE *output_file;
    // output_file = fopen(argv[2], "w");

    // Primeira linha do arquivo de saída
    // fprintf(output_file, "algoritimo\ttempo\ttarefa\n");

    bool scalability_RM = check_RM_scalability();

    // Executa o Rate Monotonic
    RM(scalability_RM);

    // Mostra o gráfico de tarefas escalonadas
    if (scalability_RM == TRUE) {
        printf("Escalonamento RM: \n");
        print_graphic(tasks_scheduled_RM);
    } else {
        printf("Impossivel escalonar pelo Rate Monotonic\n");
    }

    // Reseta as tarefas para o próximo algoritmo
    reset_tasks();

    bool scalability_EDF = check_EDF_scalability();

    // Executa o Earliest Deadline First
    EDF(scalability_EDF);

    // Mostra o gráfico de tarefas escalonadas
    if (scalability_EDF == TRUE) {
        printf("Escalonamento EDF: \n");
        print_graphic(tasks_scheduled_EDF);
    } else {
        printf("Impossivel escalonar pelo Earliest Deadline First\n");
    }

    // Fecha o arquivo de saída
    // fclose(output_file);

    return 0;
}
