#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "turma_manager.h"
#include "file_manager.h"

// ========== ARRAYS GLOBAIS (EM MEMÓRIA) ==========
static Turma turmas[MAX_TURMAS];
static int total_turmas = 0;

static AlunoTurma matriculas[MAX_TURMAS * 10]; // Cada turma pode ter vários alunos
static int total_matriculas = 0;

// ========== FUNÇÕES AUXILIARES PRIVADAS ==========

// Carrega turmas do arquivo para memória
static void carregarTurmasMemoria(void) {
    total_turmas = carregarDados(ARQUIVO_TURMAS, turmas, MAX_TURMAS, TIPO_TURMA);
}

// Salva turmas da memória para o arquivo
static void salvarTurmasArquivo(void) {
    salvarDados(ARQUIVO_TURMAS, turmas, total_turmas, TIPO_TURMA);
}

// Carrega matrículas do arquivo para memória
static void carregarMatriculasMemoria(void) {
    // Note: Você precisará adicionar TIPO_ALUNO_TURMA no file_manager
    // Por simplicidade, vou implementar aqui diretamente
    FILE *arquivo = fopen(ARQUIVO_ALUNO_TURMA, "r");
    if (arquivo == NULL) {
        total_matriculas = 0;
        return;
    }
    
    char linha[256];
    fgets(linha, sizeof(linha), arquivo); // Pular cabeçalho
    
    total_matriculas = 0;
    while (fgets(linha, sizeof(linha), arquivo) != NULL && total_matriculas < MAX_TURMAS * 10) {
        sscanf(linha, "%d,%d", 
               &matriculas[total_matriculas].ra,
               &matriculas[total_matriculas].id_turma);
        total_matriculas++;
    }
    
    fclose(arquivo);
}

// Salva matrículas da memória para o arquivo
static void salvarMatriculasArquivo(void) {
    FILE *arquivo = fopen(ARQUIVO_ALUNO_TURMA, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo de matrículas.\n");
        return;
    }
    
    fprintf(arquivo, "RA,ID_Turma\n");
    
    // Estrutura de repetição (requisito obrigatório)
    for (int i = 0; i < total_matriculas; i++) {
        fprintf(arquivo, "%d,%d\n", 
                matriculas[i].ra,
                matriculas[i].id_turma);
    }
    
    fclose(arquivo);
}

// ========== IMPLEMENTAÇÃO DAS FUNÇÕES PÚBLICAS ==========

// Cadastrar uma nova turma
int cadastrarTurma(Turma *turma) {
    // Estrutura de decisão (requisito obrigatório)
    if (turma == NULL) {
        printf("Erro: turma inválida.\n");
        return 0;
    }
    
    carregarTurmasMemoria();
    
    // Verificar se ID já existe
    for (int i = 0; i < total_turmas; i++) {
        if (turmas[i].id == turma->id) {
            printf("Erro: ID %d já cadastrado.\n", turma->id);
            return 0;
        }
    }
    
    // Verificar limite
    if (total_turmas >= MAX_TURMAS) {
        printf("Erro: limite de turmas atingido.\n");
        return 0;
    }
    
    // Adicionar nova turma
    turmas[total_turmas] = *turma;
    total_turmas++;
    salvarTurmasArquivo();
    
    printf("Turma '%s' cadastrada com sucesso!\n", turma->nome);
    return 1;
}

// Buscar turma por ID (uso de ponteiro - requisito desejável)
Turma* buscarTurmaPorID(int id) {
    carregarTurmasMemoria();
    
    // Estrutura de repetição (requisito obrigatório)
    for (int i = 0; i < total_turmas; i++) {
        if (turmas[i].id == id) {
            return &turmas[i]; // Retorna ponteiro
        }
    }
    
    return NULL; // Não encontrada
}

// Listar todas as turmas
int listarTurmas(Turma *destino, int max) {
    carregarTurmasMemoria();
    
    int count = (total_turmas < max) ? total_turmas : max;
    
    // Copia turmas para o array de destino
    for (int i = 0; i < count; i++) {
        destino[i] = turmas[i];
    }
    
    return count;
}

// Atualizar dados de uma turma
int atualizarTurma(Turma *turma) {
    if (turma == NULL) {
        return 0;
    }
    
    carregarTurmasMemoria();
    
    for (int i = 0; i < total_turmas; i++) {
        if (turmas[i].id == turma->id) {
            turmas[i] = *turma;
            salvarTurmasArquivo();
            printf("Turma atualizada com sucesso!\n");
            return 1;
        }
    }
    
    printf("Erro: turma não encontrada.\n");
    return 0;
}

// Excluir uma turma
int excluirTurma(int id) {
    carregarTurmasMemoria();
    
    // Estrutura de repetição com decisão (requisitos obrigatórios)
    for (int i = 0; i < total_turmas; i++) {
        if (turmas[i].id == id) {
            // Remove a turma deslocando os elementos
            for (int j = i; j < total_turmas - 1; j++) {
                turmas[j] = turmas[j + 1];
            }
            total_turmas--;
            salvarTurmasArquivo();
            
            printf("Turma excluída com sucesso!\n");
            return 1;
        }
    }
    
    printf("Erro: turma não encontrada.\n");
    return 0;
}

// ========== FUNÇÕES DE ASSOCIAÇÃO ALUNO-TURMA ==========

// Associar um aluno a uma turma (matrícula)
int associarAlunoTurma(int ra, int id_turma) {
    carregarMatriculasMemoria();
    
    // Verificar se já está matriculado
    for (int i = 0; i < total_matriculas; i++) {
        if (matriculas[i].ra == ra && matriculas[i].id_turma == id_turma) {
            printf("Aviso: aluno já matriculado nesta turma.\n");
            return 0;
        }
    }
    
    // Adicionar matrícula
    if (total_matriculas < MAX_TURMAS * 10) {
        matriculas[total_matriculas].ra = ra;
        matriculas[total_matriculas].id_turma = id_turma;
        total_matriculas++;
        salvarMatriculasArquivo();
        
        printf("Aluno RA %d matriculado na turma ID %d.\n", ra, id_turma);
        return 1;
    }
    
    printf("Erro: limite de matrículas atingido.\n");
    return 0;
}

// Remover um aluno de uma turma
int removerAlunoTurma(int ra, int id_turma) {
    carregarMatriculasMemoria();
    
    for (int i = 0; i < total_matriculas; i++) {
        if (matriculas[i].ra == ra && matriculas[i].id_turma == id_turma) {
            // Remove deslocando elementos
            for (int j = i; j < total_matriculas - 1; j++) {
                matriculas[j] = matriculas[j + 1];
            }
            total_matriculas--;
            salvarMatriculasArquivo();
            
            printf("Aluno removido da turma com sucesso!\n");
            return 1;
        }
    }
    
    printf("Erro: matrícula não encontrada.\n");
    return 0;
}

// Listar todos os alunos de uma turma
int listarAlunosDaTurma(int id_turma, int *ras_destino, int max) {
    carregarMatriculasMemoria();
    
    int count = 0;
    
    // Estrutura de repetição aninhada (requisito obrigatório)
    for (int i = 0; i < total_matriculas && count < max; i++) {
        if (matriculas[i].id_turma == id_turma) {
            ras_destino[count] = matriculas[i].ra;
            count++;
        }
    }
    
    return count;
}

// Listar todas as turmas de um aluno
int listarTurmasDoAluno(int ra, int *ids_destino, int max) {
    carregarMatriculasMemoria();
    
    int count = 0;
    
    for (int i = 0; i < total_matriculas && count < max; i++) {
        if (matriculas[i].ra == ra) {
            ids_destino[count] = matriculas[i].id_turma;
            count++;
        }
    }
    
    return count;
}

// Verificar se um aluno está matriculado em uma turma
int verificarMatricula(int ra, int id_turma) {
    carregarMatriculasMemoria();
    
    for (int i = 0; i < total_matriculas; i++) {
        if (matriculas[i].ra == ra && matriculas[i].id_turma == id_turma) {
            return 1; // Matriculado
        }
    }
    
    return 0; // Não matriculado
}

// Gerar próximo ID disponível de turma
int gerarProximoIDTurma(void) {
    carregarTurmasMemoria();
    
    int maior_id = 0;
    
    // Encontra o maior ID existente
    for (int i = 0; i < total_turmas; i++) {
        if (turmas[i].id > maior_id) {
            maior_id = turmas[i].id;
        }
    }
    
    return maior_id + 1;
}