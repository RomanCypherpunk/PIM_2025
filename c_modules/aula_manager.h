#ifndef AULA_MANAGER_H
#define AULA_MANAGER_H

#include "structs.h"

#define MAX_AULAS 5000
#define ARQUIVO_AULAS "data/aulas.csv"

// ========== FUNÇÕES DE GERENCIAMENTO DE AULAS (DIÁRIO ELETRÔNICO) ==========

// Função para registrar uma nova aula no diário
// Retorna: 1 se sucesso, 0 se erro
int registrarAula(Aula *aula);

// Função para buscar aula por ID
// Retorna: ponteiro para a aula ou NULL se não encontrada
Aula* buscarAulaPorID(int id);

// Função para listar todas as aulas de uma turma
// Retorna: número de aulas listadas
int listarAulasDaTurma(int id_turma, Aula *destino, int max);

// Função para listar todas as aulas
// Retorna: número total de aulas
int listarTodasAulas(Aula *destino, int max);

// Função para atualizar dados de uma aula
// Retorna: 1 se sucesso, 0 se erro
int atualizarAula(Aula *aula);

// Função para excluir uma aula do diário
// Retorna: 1 se sucesso, 0 se erro
int excluirAula(int id);

// ========== FUNÇÕES DE CONSULTA E RELATÓRIOS ==========

// Função para buscar aulas por data
// Retorna: número de aulas encontradas naquela data
int buscarAulasPorData(const char *data, Aula *destino, int max);

// Função para buscar aulas de uma turma em um período
// Retorna: número de aulas no período
int buscarAulasPorPeriodo(int id_turma, const char *data_inicio, 
                          const char *data_fim, Aula *destino, int max);

// Função para contar total de aulas de uma turma
// Retorna: número de aulas ministradas
int contarAulasDaTurma(int id_turma);

// Função para gerar relatório do diário de classe (texto)
// Retorna: 1 se sucesso, 0 se erro
int gerarRelatorioTurma(int id_turma, const char *arquivo_destino);

// ========== FUNÇÕES AUXILIARES ==========

// Função para gerar próximo ID disponível de aula
int gerarProximoIDAula(void);

// Função para validar formato de data (DD/MM/AAAA)
// Retorna: 1 se válida, 0 se inválida
int validarData(const char *data);

#endif