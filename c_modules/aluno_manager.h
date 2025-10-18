#ifndef ALUNO_MANAGER_H
#define ALUNO_MANAGER_H

#include "structs.h"

#define MAX_ALUNOS 1000
#define ARQUIVO_ALUNOS "data/alunos.csv"

// Função para cadastrar um novo aluno
int cadastrarAluno(Aluno *aluno);

// Função para buscar aluno por RA
Aluno* buscarAlunoPorRA(int ra);

// Função para listar todos os alunos
int listarAlunos(Aluno *alunos, int max);

// Função para atualizar dados de um aluno
int atualizarAluno(Aluno *aluno);

// Função para excluir aluno (desativar)
int excluirAluno(int ra);

#endif