#ifndef TURMA_MANAGER_H
#define TURMA_MANAGER_H

#include "structs.h"

#define MAX_TURMAS 500
#define ARQUIVO_TURMAS "data/turmas.csv"
#define ARQUIVO_ALUNO_TURMA "data/aluno_turma.csv"

// ========== FUNÇÕES DE GERENCIAMENTO DE TURMAS ==========

// Função para cadastrar uma nova turma
// Retorna: 1 se sucesso, 0 se erro
int cadastrarTurma(Turma *turma);

// Função para buscar turma por ID
// Retorna: ponteiro para a turma ou NULL se não encontrada
Turma* buscarTurmaPorID(int id);

// Função para listar todas as turmas
// Retorna: número de turmas listadas
int listarTurmas(Turma *destino, int max);

// Função para atualizar dados de uma turma
// Retorna: 1 se sucesso, 0 se erro
int atualizarTurma(Turma *turma);

// Função para excluir uma turma
// Retorna: 1 se sucesso, 0 se erro
int excluirTurma(int id);

// ========== FUNÇÕES DE ASSOCIAÇÃO ALUNO-TURMA ==========

// Função para associar um aluno a uma turma (matrícula)
// Retorna: 1 se sucesso, 0 se erro
int associarAlunoTurma(int ra, int id_turma);

// Função para remover um aluno de uma turma
// Retorna: 1 se sucesso, 0 se erro
int removerAlunoTurma(int ra, int id_turma);

// Função para listar todos os alunos de uma turma
// Retorna: número de alunos na turma
int listarAlunosDaTurma(int id_turma, int *ras_destino, int max);

// Função para listar todas as turmas de um aluno
// Retorna: número de turmas do aluno
int listarTurmasDoAluno(int ra, int *ids_destino, int max);

// Função para verificar se um aluno está matriculado em uma turma
// Retorna: 1 se matriculado, 0 se não
int verificarMatricula(int ra, int id_turma);

// ========== FUNÇÕES AUXILIARES ==========

// Função para gerar próximo ID disponível de turma
int gerarProximoIDTurma(void);

#endif