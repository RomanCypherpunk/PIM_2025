#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "structs.h"

/*
 * Este cabeçalho descreve minhas funções utilitárias para lidar com arquivos CSV.
 * Deixo aqui anotações para lembrar exatamente o que cada função faz.
 */

/* Eu salvo um vetor de estruturas em disco no formato CSV. */
int salvarDados(const char *nome_arquivo, void *dados, int num_registros, int tipo);

/* Eu carrego dados de um CSV para um vetor previamente alocado. */
int carregarDados(const char *nome_arquivo, void *destino, int max_registros, int tipo);

/* Enum que uso para dizer ao file_manager qual estrutura estou tratando. */
enum TipoDado {
    TIPO_ALUNO = 1,
    TIPO_TURMA = 2,
    TIPO_AULA = 3,
    TIPO_ALUNO_TURMA = 4,
    TIPO_ATIVIDADE = 5
};

#endif /* FILE_MANAGER_H */

