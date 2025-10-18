#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aluno_manager.h"
#include "file_manager.h"

// Array global de alunos (em memória)
static Aluno alunos[MAX_ALUNOS];
static int total_alunos = 0;

// Função auxiliar para carregar alunos em memória
static void carregarAlunosMemoria() {
    total_alunos = carregarDados(ARQUIVO_ALUNOS, alunos, MAX_ALUNOS, TIPO_ALUNO);
}

// Função auxiliar para salvar alunos no arquivo
static void salvarAlunosArquivo() {
    salvarDados(ARQUIVO_ALUNOS, alunos, total_alunos, TIPO_ALUNO);
}

// ========== CADASTRAR ALUNO ==========
int cadastrarAluno(Aluno *aluno) {
    if (aluno == NULL) {
        return 0;
    }
    
    carregarAlunosMemoria();
    
    // Verificar se RA já existe (estrutura de repetição)
    for (int i = 0; i < total_alunos; i++) {
        if (alunos[i].ra == aluno->ra) {
            printf("Erro: RA %d já cadastrado.\n", aluno->ra);
            return 0;
        }
    }
    
    // Adicionar novo aluno
    if (total_alunos < MAX_ALUNOS) {
        alunos[total_alunos] = *aluno;
        total_alunos++;
        salvarAlunosArquivo();
        printf("Aluno cadastrado com sucesso!\n");
        return 1;
    }
    
    printf("Erro: limite de alunos atingido.\n");
    return 0;
}

// ========== BUSCAR ALUNO POR RA ==========
Aluno* buscarAlunoPorRA(int ra) {
    carregarAlunosMemoria();
    
    // Uso de ponteiro (requisito desejável)
    for (int i = 0; i < total_alunos; i++) {
        if (alunos[i].ra == ra) {
            return &alunos[i]; // Retorna ponteiro para o aluno
        }
    }
    
    return NULL; // Não encontrado
}

// ========== LISTAR TODOS OS ALUNOS ==========
int listarAlunos(Aluno *destino, int max) {
    carregarAlunosMemoria();
    
    int count = (total_alunos < max) ? total_alunos : max;
    
    // Copia alunos para o array de destino
    for (int i = 0; i < count; i++) {
        destino[i] = alunos[i];
    }
    
    return count;
}

// ========== ATUALIZAR ALUNO ==========
int atualizarAluno(Aluno *aluno) {
    carregarAlunosMemoria();
    
    for (int i = 0; i < total_alunos; i++) {
        if (alunos[i].ra == aluno->ra) {
            alunos[i] = *aluno;
            salvarAlunosArquivo();
            printf("Aluno atualizado com sucesso!\n");
            return 1;
        }
    }
    
    printf("Erro: aluno não encontrado.\n");
    return 0;
}

// ========== EXCLUIR ALUNO (soft delete) ==========
int excluirAluno(int ra) {
    carregarAlunosMemoria();
    
    for (int i = 0; i < total_alunos; i++) {
        if (alunos[i].ra == ra) {
            alunos[i].ativo = 0; // Desativa ao invés de remover
            salvarAlunosArquivo();
            printf("Aluno desativado com sucesso!\n");
            return 1;
        }
    }
    
    printf("Erro: aluno não encontrado.\n");
    return 0;
}