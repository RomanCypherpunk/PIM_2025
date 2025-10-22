#ifndef ATIVIDADE_MANAGER_H
#define ATIVIDADE_MANAGER_H

#include "structs.h"

#define MAX_ATIVIDADES 2000
#define ARQUIVO_ATIVIDADES "data/atividades.csv"

// ========== FUNÇÕES DE GERENCIAMENTO DE ATIVIDADES ==========

// Cadastrar uma nova atividade
// Retorna 1 em caso de sucesso, 0 caso contrário
int cadastrarAtividade(Atividade *atividade);

// Buscar atividade por ID
// Retorna ponteiro para a atividade ou NULL se não encontrada
Atividade* buscarAtividadePorID(int id);

// Listar todas as atividades
// Copia até max atividades para o destino e retorna a quantidade copiada
int listarAtividades(Atividade *destino, int max);

// Listar atividades por turma
// Retorna a quantidade copiada para o destino
int listarAtividadesDaTurma(int id_turma, Atividade *destino, int max);

// Atualizar uma atividade existente
int atualizarAtividade(Atividade *atividade);

// Excluir uma atividade definitivamente (remove do arquivo)
int excluirAtividade(int id);

// Gerar próximo ID sequencial disponível
int gerarProximoIDAtividade(void);

#endif

