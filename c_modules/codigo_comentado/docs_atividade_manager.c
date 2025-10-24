#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atividade_manager.h"
#include "file_manager.h"

/*
 * Nesta versão comentada eu explico como administro as atividades das turmas.
 * A ideia é transformar o código em uma aula: cada decisão é detalhada para
 * que eu consiga apresentar o fluxo sem ter que improvisar.
 */

/* Aqui eu guardo as atividades carregadas do CSV enquanto o programa roda. */
static Atividade atividades[MAX_ATIVIDADES];
static int total_atividades = 0;

/*
 * Toda vez que preciso trabalhar com atividades, começo trazendo os dados
 * do arquivo CSV para este array global. Assim mantenho a memória sincronizada
 * com o que está salvo em disco.
 */
static void carregarAtividadesMemoria(void) {
    total_atividades = carregarDados(ARQUIVO_ATIVIDADES,
                                     atividades,
                                     MAX_ATIVIDADES,
                                     TIPO_ATIVIDADE);
}

/*
 * Depois de qualquer alteração, salvo a lista novamente no CSV para não perder
 * o estado atual das atividades.
 */
static void salvarAtividadesArquivo(void) {
    salvarDados(ARQUIVO_ATIVIDADES,
                atividades,
                total_atividades,
                TIPO_ATIVIDADE);
}

// ========== FUNÇÕES PÚBLICAS ==========

/*
 * Cadastro de atividades:
 *  1. Confirmo que recebi um ponteiro válido.
 *  2. Carrego a memória com o conteúdo mais recente.
 *  3. Garante que o ID ainda não foi usado.
 *  4. Copio a atividade para o array e salvo.
 */
int cadastrarAtividade(Atividade *atividade) {
    if (atividade == NULL) {
        return 0;
    }

    carregarAtividadesMemoria();

    for (int i = 0; i < total_atividades; i++) {
        /*
         * Eu uso o ID como chave primária. Se encontrar o mesmo ID,
         * devolvo erro para que o chamador trate a duplicidade.
         */
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

/*
 * Eu busco uma atividade pelo ID e retorno um ponteiro para ela.
 * Isso me permite editar a estrutura diretamente em outras partes do sistema.
 */
Atividade* buscarAtividadePorID(int id) {
    carregarAtividadesMemoria();

    for (int i = 0; i < total_atividades; i++) {
        if (atividades[i].id == id) {
            return &atividades[i];
        }
    }

    return NULL;
}

/*
 * Para montar um relatório completo de atividades, copio o conteúdo para um
 * buffer fornecido externamente. Retorno quantos registros consegui copiar.
 */
int listarAtividades(Atividade *destino, int max) {
    carregarAtividadesMemoria();

    int count = (total_atividades < max) ? total_atividades : max;

    for (int i = 0; i < count; i++) {
        destino[i] = atividades[i];
    }

    return count;
}

/*
 * Quando desejo listar somente as atividades de uma turma específica,
 * filtro pelo campo id_turma. Vou copiando até preencher o espaço disponível.
 */
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

/*
 * Para atualizar uma atividade eu procuro o ID e substituo a estrutura toda.
 * Depois gravo a lista de volta no CSV para manter persistência.
 */
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

/*
 * Para excluir eu removo a posição do array.
 * Faço isso deslocando os elementos seguintes uma casa para trás,
 * mantendo o array compacto.
 */
int excluirAtividade(int id) {
    carregarAtividadesMemoria();

    for (int i = 0; i < total_atividades; i++) {
        if (atividades[i].id == id) {
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

/*
 * Quando preciso de um novo identificador sequencial,
 * percorro a lista para descobrir o maior ID e retorno o próximo.
 */
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

