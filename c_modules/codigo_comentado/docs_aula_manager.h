#ifndef AULA_MANAGER_H
#define AULA_MANAGER_H

#include "structs.h"

/*
 * Este cabeçalho é o meu guia para tudo que o gerenciamento de aulas exporta.
 * Cada definição ganhou um comentário para que eu explique facilmente em sala.
 */

/* Quantidade máxima de aulas que planejo suportar ao mesmo tempo. */
#define MAX_AULAS 5000

/* Local onde persistimos o diário eletrônico em CSV. */
#define ARQUIVO_AULAS "data/aulas.csv"

/* ===== Operações principais do diário eletrônico ===== */

/* Aqui registro uma aula nova e valido data, limites e duplicidade. */
int registrarAula(Aula *aula);

/* Função que uso para recuperar uma aula existente pelo ID. */
Aula* buscarAulaPorID(int id);

/* Lista somente as aulas de uma turma específica. */
int listarAulasDaTurma(int id_turma, Aula *destino, int max);

/* Lista todas as aulas cadastradas, respeitando o tamanho do buffer. */
int listarTodasAulas(Aula *destino, int max);

/* Atualiza as informações de uma aula já registrada. */
int atualizarAula(Aula *aula);

/* Remove definitivamente uma aula do diário. */
int excluirAula(int id);

/* ===== Consultas e relatórios ===== */

/* Busca todas as aulas que aconteceram em uma data específica. */
int buscarAulasPorData(const char *data, Aula *destino, int max);

/* Faz um filtro por período, útil para relatórios. */
int buscarAulasPorPeriodo(int id_turma,
                          const char *data_inicio,
                          const char *data_fim,
                          Aula *destino,
                          int max);

/* Conta quantas aulas uma turma já teve. */
int contarAulasDaTurma(int id_turma);

/* Gera um relatório textual com todas as aulas de uma turma. */
int gerarRelatorioTurma(int id_turma, const char *arquivo_destino);

/* ===== Funções auxiliares ===== */

/* Fornece o próximo ID disponível para registrar uma aula nova. */
int gerarProximoIDAula(void);

/* Verifica se a string está no formato DD/MM/AAAA e dentro das faixas esperadas. */
int validarData(const char *data);

#endif /* AULA_MANAGER_H */

