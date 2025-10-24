#ifndef STRUCTS_H
#define STRUCTS_H

/*
 * Separei todas as estruturas de dados aqui para ter um mapa mental claro.
 * Cada comentário explica por que eu escolhi determinados campos e tamanhos.
 */

#define MAX_NOME        100
#define MAX_TURMA_NOME   50
#define MAX_CONTEUDO    500
#define MAX_PATH        200
#define MAX_LOGIN        50
#define MAX_SENHA        50

/*
 * Estrutura do aluno:
 *  - RA funciona como identificador único.
 *  - Nome e email usam os tamanhos definidos acima.
 *  - O campo ativo permite fazer soft delete sem perder histórico.
 */
typedef struct {
    int  ra;
    char nome[MAX_NOME];
    char email[MAX_NOME];
    int  ativo;
} Aluno;

/*
 * Estrutura da turma:
 *  - id é a chave primária.
 *  - Guardo o nome do curso/turma, o professor responsável e o período letivo.
 */
typedef struct {
    int  id;
    char nome[MAX_TURMA_NOME];
    char professor[MAX_NOME];
    int  ano;
    int  semestre;
} Turma;

/*
 * Estrutura de aula (registro do diário eletrônico):
 *  - id identifica cada aula.
 *  - id_turma liga a turma correspondente.
 *  - data fica em formato DD/MM/AAAA.
 *  - conteudo armazena o resumo do que foi ministrado.
 */
typedef struct {
    int  id;
    int  id_turma;
    char data[11];
    char conteudo[MAX_CONTEUDO];
} Aula;

/*
 * Associação aluno-turma:
 *  - Represento o relacionamento N:N com um par (ra, id_turma).
 */
typedef struct {
    int ra;
    int id_turma;
} AlunoTurma;

/*
 * Estrutura da atividade:
 *  - id único e id_turma como chave estrangeira.
 *  - Guardo título, descrição e caminho opcional para arquivo.
 */
typedef struct {
    int  id;
    int  id_turma;
    char titulo[MAX_NOME];
    char descricao[MAX_CONTEUDO];
    char path_arquivo[MAX_PATH];
} Atividade;

/*
 * Estrutura do usuário do sistema:
 *  - id como chave.
 *  - login único e senha em texto simples (versão simplificada do PIM).
 *  - tipo define o perfil e ativo permite bloqueio sem excluir.
 */
typedef struct {
    int  id;
    char login[MAX_LOGIN];
    char senha[MAX_SENHA];
    char tipo[20];
    int  ativo;
} Usuario;

#endif /* STRUCTS_H */

