#ifndef ATIVIDADE_MANAGER_H
#define ATIVIDADE_MANAGER_H

#include "structs.h"

/*
 * Este cabeçalho descreve tudo que o módulo de atividades expõe.
 * Eu anotei cada definição para lembrar o motivo de existir.
 */

/* Máximo de atividades que decidi suportar simultaneamente. */
#define MAX_ATIVIDADES 2000

/* Arquivo CSV onde salvo e de onde carrego as atividades. */
#define ARQUIVO_ATIVIDADES "data/atividades.csv"

/*
 * Assinaturas das funções públicas com comentários em primeira pessoa.
 * Assim consigo explicar rapidamente o objetivo de cada uma durante a apresentação.
 */

/* Aqui eu insiro uma nova atividade no sistema. */
int cadastrarAtividade(Atividade *atividade);

/* Uso esta função quando preciso localizar uma atividade específica. */
Atividade* buscarAtividadePorID(int id);

/* Para montar listas completas de atividades, chamo esta função. */
int listarAtividades(Atividade *destino, int max);

/* Para filtrar pelas atividades de uma turma específica, uso esta. */
int listarAtividadesDaTurma(int id_turma, Atividade *destino, int max);

/* Este protótipo sinaliza como aplicar alterações em uma atividade existente. */
int atualizarAtividade(Atividade *atividade);

/* Quando quero remover definitivamente uma atividade, aciono esta função. */
int excluirAtividade(int id);

/* E para gerar novos IDs sequenciais, exponho este utilitário. */
int gerarProximoIDAtividade(void);

#endif /* ATIVIDADE_MANAGER_H */

