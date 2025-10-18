#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "structs.h"

// Função para salvar dados em arquivo CSV
// Retorna: 1 se sucesso, 0 se erro
int salvarDados(const char *nome_arquivo, void *dados, int num_registros, int tipo);

// Função para carregar dados de arquivo CSV
// Retorna: número de registros lidos, -1 se erro
int carregarDados(const char *nome_arquivo, void *destino, int max_registros, int tipo);

// Enumeração para identificar o tipo de dado
enum TipoDado {
    TIPO_ALUNO = 1,
    TIPO_TURMA = 2,
    TIPO_AULA = 3,
    TIPO_ALUNO_TURMA = 4,
    TIPO_ATIVIDADE = 5
};

#endif