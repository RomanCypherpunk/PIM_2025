#ifndef ALUNO_MANAGER_H
#define ALUNO_MANAGER_H

#include "structs.h"

/*
 * Eu montei este cabeçalho como um mapa mental do módulo de alunos.
 * Tudo que eu exponho para outros arquivos fica declarado aqui.
 * Os comentários são minhas anotações para lembrar o porquê de cada item.
 */

/* Limite máximo de alunos que decidi suportar em memória. */
#define MAX_ALUNOS 1000

/* Caminho do CSV onde persistirei os dados de alunos. */
#define ARQUIVO_ALUNOS "data/alunos.csv"

/*
 * Aqui declaro as operações disponíveis:
 * cada protótipo vem com uma nota pessoal explicando a intenção do serviço.
 */

/* Quando preciso inserir um novo aluno, chamo esta função. */
int cadastrarAluno(Aluno *aluno);

/* Para localizar rapidamente alguém pelo RA, uso esta função de busca. */
Aluno* buscarAlunoPorRA(int ra);

/* Esta função me ajuda a montar listagens completas de alunos. */
int listarAlunos(Aluno *alunos, int max);

/* Com este protótipo eu informo como atualizar os dados de um aluno existente. */
int atualizarAluno(Aluno *aluno);

/* E, para desativar (soft delete) um aluno, exponho esta função. */
int excluirAluno(int ra);

#endif /* ALUNO_MANAGER_H */

