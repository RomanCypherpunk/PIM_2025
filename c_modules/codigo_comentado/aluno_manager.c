#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aluno_manager.h"
#include "file_manager.h"

/*
 * Estou usando este arquivo como material de estudo.
 * Aqui eu centralizo todas as operações relacionadas ao módulo de alunos
 * (CRUD completo) e registro nos comentários a minha linha de raciocínio.
 * Assim, quando eu estiver apresentando o projeto, consigo explicar cada
 * decisão de forma didática.
 */

/*
 * Eu mantenho um array global com os alunos carregados.
 * Isso evita leituras repetidas no disco e me permite trabalhar
 * com os dados na RAM até que eu decida persistir as mudanças.
 */
static Aluno alunos[MAX_ALUNOS];
static int total_alunos = 0;

/*
 * Sempre que preciso consultar ou alterar algum aluno
 * eu começo trazendo tudo do arquivo CSV para a memória.
 * Dessa maneira garanto que estou mexendo na versão mais recente.
 */
static void carregarAlunosMemoria() {
    total_alunos = carregarDados(ARQUIVO_ALUNOS, alunos, MAX_ALUNOS, TIPO_ALUNO);
}

/*
 * Depois de qualquer alteração em memória eu salvo o array novamente no CSV.
 * Eu faço isso para que outras partes do sistema enxerguem o mesmo estado dos dados.
 */
static void salvarAlunosArquivo() {
    salvarDados(ARQUIVO_ALUNOS, alunos, total_alunos, TIPO_ALUNO);
}

// ========== CADASTRAR ALUNO ==========
/*
 * Nesta função eu cadastro um aluno novo.
 * A sequência mental que sigo é:
 *   1. Verifico se recebi um ponteiro válido.
 *   2. Carrego os dados existentes para não sobrescrever nada antigo.
 *   3. Confiro se o RA já existe (mantendo a integridade).
 *   4. Insiro o aluno e salvo no arquivo.
 */
int cadastrarAluno(Aluno *aluno) {
    if (aluno == NULL) {
        /* Aqui eu trato o caso em que alguém me passa um ponteiro nulo. */
        return 0;
    }

    carregarAlunosMemoria(); /* Trago os dados mais recentes antes de alterar. */

    for (int i = 0; i < total_alunos; i++) {
        /*
         * Analiso cada posição para garantir que o RA informado
         * ainda não está cadastrado. Assim evito duplicidades.
         */
        if (alunos[i].ra == aluno->ra) {
            printf("Erro: RA %d já cadastrado.\n", aluno->ra);
            return 0;
        }
    }

    if (total_alunos < MAX_ALUNOS) {
        /*
         * Caso haja espaço, copio a estrutura recebida
         * para a próxima posição livre do array.
         */
        alunos[total_alunos] = *aluno;
        total_alunos++;

        /*
         * Após inserir, salvo tudo em arquivo para consolidar a alteração.
         * Isso evita que um crash deixe os dados inconsistentes.
         */
        salvarAlunosArquivo();
        printf("Aluno cadastrado com sucesso!\n");
        return 1;
    }

    printf("Erro: limite de alunos atingido.\n");
    return 0;
}

// ========== BUSCAR ALUNO POR RA ==========
/*
 * Sempre que quero localizar um aluno específico, uso o RA.
 * Retorno um ponteiro para permitir que outras funções atualizem a estrutura.
 */
Aluno* buscarAlunoPorRA(int ra) {
    carregarAlunosMemoria(); /* Recarrego para ter certeza de que os dados estão atuais. */

    for (int i = 0; i < total_alunos; i++) {
        if (alunos[i].ra == ra) {
            /*
             * Retorno o endereço da estrutura em memória.
             * Fazer isso me permite tanto ler quanto editar o registro.
             */
            return &alunos[i];
        }
    }

    /* Se ninguém foi encontrado devolvo NULL para sinalizar ausência. */
    return NULL;
}

// ========== LISTAR TODOS OS ALUNOS ==========
/*
 * Esta função me ajuda a montar relatórios ou preencher tabelas.
 * Eu copio uma quantidade limitada de alunos para um buffer externo.
 */
int listarAlunos(Aluno *destino, int max) {
    carregarAlunosMemoria();

    /* Defino quantos registros posso copiar sem estourar o array de destino. */
    int count = (total_alunos < max) ? total_alunos : max;

    for (int i = 0; i < count; i++) {
        /*
         * Faço uma cópia por valor de cada aluno.
         * Dessa maneira quem chamou a função pode trabalhar livremente
         * sem modificar meu array global por engano.
         */
        destino[i] = alunos[i];
    }

    return count;
}

// ========== ATUALIZAR ALUNO ==========
/*
 * Para atualizar um aluno eu recebo a estrutura preenchida já com o RA.
 * Quando encontro o RA correspondente eu substituo a versão antiga pela nova
 * e salvo no CSV.
 */
int atualizarAluno(Aluno *aluno) {
    carregarAlunosMemoria();

    for (int i = 0; i < total_alunos; i++) {
        if (alunos[i].ra == aluno->ra) {
            /*
             * Aqui eu sobrescrevo a estrutura inteira.
             * Isso garante que todos os campos ficam sincronizados.
             */
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
/*
 * Ao invés de apagar fisicamente o aluno, faço um soft delete.
 * Marco o campo `ativo` como 0. Assim mantenho histórico para relatórios.
 */
int excluirAluno(int ra) {
    carregarAlunosMemoria();

    for (int i = 0; i < total_alunos; i++) {
        if (alunos[i].ra == ra) {
            /*
             * Eu apenas actualizo a flag `ativo`. O resto dos dados permanece,
             * permitindo que eu saiba quem já esteve matriculado.
             */
            alunos[i].ativo = 0;
            salvarAlunosArquivo();
            printf("Aluno desativado com sucesso!\n");
            return 1;
        }
    }

    printf("Erro: aluno não encontrado.\n");
    return 0;
}

