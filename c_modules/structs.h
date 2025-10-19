#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_NOME 100
#define MAX_TURMA_NOME 50
#define MAX_CONTEUDO 500
#define MAX_PATH 200
#define MAX_LOGIN 50
#define MAX_SENHA 50

// Estrutura para representar um Aluno
typedef struct {
    int ra;                    // Registro Acadêmico (identificador único)
    char nome[MAX_NOME];       // Nome completo
    char email[MAX_NOME];      // Email institucional
    int ativo;                 // 1 = ativo, 0 = inativo
} Aluno;

// Estrutura para representar uma Turma
typedef struct {
    int id;                    // ID único da turma
    char nome[MAX_TURMA_NOME]; // Nome da turma (ex: "ADS-2A")
    char professor[MAX_NOME];  // Nome do professor
    int ano;                   // Ano letivo
    int semestre;              // Semestre (1 ou 2)
} Turma;

// Estrutura para representar uma Aula (diário eletrônico)
typedef struct {
    int id;                    // ID único da aula
    int id_turma;              // FK: ID da turma
    char data[11];             // Data da aula (formato: DD/MM/AAAA)
    char conteudo[MAX_CONTEUDO]; // Conteúdo ministrado
} Aula;

// Estrutura para associar Aluno a Turma (relacionamento N:N)
typedef struct {
    int ra;                    // FK: RA do aluno
    int id_turma;              // FK: ID da turma
} AlunoTurma;

// Estrutura para representar uma Atividade
typedef struct {
    int id;                    // ID único da atividade
    int id_turma;              // FK: ID da turma
    char titulo[MAX_NOME];     // Título da atividade
    char descricao[MAX_CONTEUDO]; // Descrição
    char path_arquivo[MAX_PATH]; // Caminho do arquivo (se houver)
} Atividade;

// Estrutura para representar um Usuário
typedef struct {
    int id;                    // ID único do usuário
    char login[MAX_LOGIN];     // Nome de login (único)
    char senha_hash[65];       // Hash da senha (para segurança)
    char tipo[20];             // Tipo: "ADMIN", "PROFESSOR", "ALUNO"
    int ativo;                 // 1 = ativo, 0 = inativo
} Usuario;

#endif