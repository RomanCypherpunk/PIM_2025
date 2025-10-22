#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atividade_manager.h"
#include "file_manager.h"

// ========== ARMAZENAMENTO EM MEMÓRIA ==========
static Atividade atividades[MAX_ATIVIDADES];
static int total_atividades = 0;

// ========== FUNÇÕES AUXILIARES PRIVADAS ==========

// Carrega atividades do arquivo CSV para a memória
static void carregarAtividadesMemoria(void) {
    total_atividades = carregarDados(ARQUIVO_ATIVIDADES,
                                     atividades,
                                     MAX_ATIVIDADES,
                                     TIPO_ATIVIDADE);
}

// Persiste as atividades do array em memória para o CSV
static void salvarAtividadesArquivo(void) {
    salvarDados(ARQUIVO_ATIVIDADES,
                atividades,
                total_atividades,
                TIPO_ATIVIDADE);
}

// ========== FUNÇÕES PÚBLICAS ==========

int cadastrarAtividade(Atividade *atividade) {
    if (atividade == NULL) {
        return 0;
    }

    carregarAtividadesMemoria();

    // Verifica unicidade do ID
    for (int i = 0; i < total_atividades; i++) {
        if (atividades[i].id == atividade->id) {
            printf("Erro: atividade ID %d já cadastrada.\n", atividade->id);
            return 0;
        }
    }

    if (total_atividades >= MAX_ATIVIDADES) {
        printf("Erro: limite máximo de atividades atingido.\n");
        return 0;
    }

    atividades[total_atividades] = *atividade;
    total_atividades++;
    salvarAtividadesArquivo();

    printf("Atividade '%s' cadastrada com sucesso!\n", atividade->titulo);
    return 1;
}

Atividade* buscarAtividadePorID(int id) {
    carregarAtividadesMemoria();

    for (int i = 0; i < total_atividades; i++) {
        if (atividades[i].id == id) {
            return &atividades[i];
        }
    }

    return NULL;
}

int listarAtividades(Atividade *destino, int max) {
    carregarAtividadesMemoria();

    int count = (total_atividades < max) ? total_atividades : max;

    for (int i = 0; i < count; i++) {
        destino[i] = atividades[i];
    }

    return count;
}

int listarAtividadesDaTurma(int id_turma, Atividade *destino, int max) {
    carregarAtividadesMemoria();

    int count = 0;

    for (int i = 0; i < total_atividades && count < max; i++) {
        if (atividades[i].id_turma == id_turma) {
            destino[count] = atividades[i];
            count++;
        }
    }

    return count;
}

int atualizarAtividade(Atividade *atividade) {
    if (atividade == NULL) {
        return 0;
    }

    carregarAtividadesMemoria();

    for (int i = 0; i < total_atividades; i++) {
        if (atividades[i].id == atividade->id) {
            atividades[i] = *atividade;
            salvarAtividadesArquivo();
            printf("Atividade atualizada com sucesso!\n");
            return 1;
        }
    }

    printf("Erro: atividade ID %d não encontrada.\n", atividade->id);
    return 0;
}

int excluirAtividade(int id) {
    carregarAtividadesMemoria();

    for (int i = 0; i < total_atividades; i++) {
        if (atividades[i].id == id) {
            // Desloca elementos para preencher o espaço
            for (int j = i; j < total_atividades - 1; j++) {
                atividades[j] = atividades[j + 1];
            }
            total_atividades--;
            salvarAtividadesArquivo();
            printf("Atividade ID %d removida com sucesso!\n", id);
            return 1;
        }
    }

    printf("Erro: atividade ID %d não encontrada.\n", id);
    return 0;
}

int gerarProximoIDAtividade(void) {
    carregarAtividadesMemoria();

    int maior_id = 0;

    for (int i = 0; i < total_atividades; i++) {
        if (atividades[i].id > maior_id) {
            maior_id = atividades[i].id;
        }
    }

    return maior_id + 1;
}

