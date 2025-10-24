#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aula_manager.h"
#include "file_manager.h"

/*
 * Este arquivo é o meu roteiro de estudos para o módulo de aulas (diário eletrônico).
 * Eu mantive toda a lógica original, mas inseri comentários em primeira pessoa
 * para explicar o que estou fazendo e por que cada etapa é importante.
 */

/* Eu deixo todas as aulas carregadas em memória enquanto o programa roda. */
static Aula aulas[MAX_AULAS];
static int total_aulas = 0;

/*
 * Sempre começo sincronizando a memória com o arquivo CSV.
 * Assim tenho certeza de que estou trabalhando com a versão mais atualizada das aulas.
 */
static void carregarAulasMemoria(void) {
    total_aulas = carregarDados(ARQUIVO_AULAS, aulas, MAX_AULAS, TIPO_AULA);
}

/*
 * Depois que termino alguma alteração, salvo tudo de volta no CSV.
 * Dessa forma, se o programa fechar eu não perco o que acabei de registrar.
 */
static void salvarAulasArquivo(void) {
    salvarDados(ARQUIVO_AULAS, aulas, total_aulas, TIPO_AULA);
}

/*
 * Aqui eu recebo os dados de uma aula e valido passo a passo antes de salvar.
 * Eu checo ponteiro, formato de data, duplicidade de ID e limite do array.
 */
int registrarAula(Aula *aula) {
    if (aula == NULL) {
        printf("Erro: dados da aula inválidos.\n");
        return 0;
    }

    if (!validarData(aula->data)) {
        printf("Erro: data inválida. Use formato DD/MM/AAAA.\n");
        return 0;
    }

    carregarAulasMemoria();

    for (int i = 0; i < total_aulas; i++) {
        /* Garanto que não existe outra aula com o mesmo ID. */
        if (aulas[i].id == aula->id) {
            printf("Erro: ID %d já cadastrado.\n", aula->id);
            return 0;
        }
    }

    if (total_aulas >= MAX_AULAS) {
        printf("Erro: limite de aulas atingido.\n");
        return 0;
    }

    aulas[total_aulas] = *aula;
    total_aulas++;
    salvarAulasArquivo();

    printf("Aula registrada com sucesso no diário eletrônico!\n");
    return 1;
}

/*
 * Para localizar uma aula específica, percorro as aulas carregadas e retorno o ponteiro.
 * Usar ponteiro facilita atualizações posteriores sem cópias desnecessárias.
 */
Aula* buscarAulaPorID(int id) {
    carregarAulasMemoria();

    for (int i = 0; i < total_aulas; i++) {
        if (aulas[i].id == id) {
            return &aulas[i];
        }
    }

    return NULL;
}

/*
 * Quando preciso listar as aulas de uma turma, filtro pelo id_turma.
 * Vou copiando para o array destino até atingir o limite informado.
 */
int listarAulasDaTurma(int id_turma, Aula *destino, int max) {
    carregarAulasMemoria();

    int count = 0;

    for (int i = 0; i < total_aulas && count < max; i++) {
        if (aulas[i].id_turma == id_turma) {
            destino[count] = aulas[i];
            count++;
        }
    }

    return count;
}

/*
 * Para montar relatórios completos, copio todas as aulas disponíveis.
 * O retorno indica quantos itens foram realmente copiados.
 */
int listarTodasAulas(Aula *destino, int max) {
    carregarAulasMemoria();

    int count = (total_aulas < max) ? total_aulas : max;

    for (int i = 0; i < count; i++) {
        destino[i] = aulas[i];
    }

    return count;
}

/*
 * Aqui eu atualizo uma aula existente.
 * Reaproveito a lógica de validação da data e substituo a estrutura inteira ao encontrar o ID.
 */
int atualizarAula(Aula *aula) {
    if (aula == NULL) {
        return 0;
    }

    if (!validarData(aula->data)) {
        printf("Erro: data inválida.\n");
        return 0;
    }

    carregarAulasMemoria();

    for (int i = 0; i < total_aulas; i++) {
        if (aulas[i].id == aula->id) {
            aulas[i] = *aula;
            salvarAulasArquivo();
            printf("Aula atualizada com sucesso!\n");
            return 1;
        }
    }

    printf("Erro: aula não encontrada.\n");
    return 0;
}

/*
 * Para excluir eu removo a posição do array, deslocando as aulas seguintes.
 * Assim mantenho a estrutura compacta.
 */
int excluirAula(int id) {
    carregarAulasMemoria();

    for (int i = 0; i < total_aulas; i++) {
        if (aulas[i].id == id) {
            for (int j = i; j < total_aulas - 1; j++) {
                aulas[j] = aulas[j + 1];
            }
            total_aulas--;
            salvarAulasArquivo();
            printf("Aula ID %d removida com sucesso!\n", id);
            return 1;
        }
    }

    printf("Erro: aula não encontrada.\n");
    return 0;
}

/*
 * Aqui eu procuro todas as aulas que aconteceram em uma data específica.
 * Copio para o destino apenas as que batem com a data informada.
 */
int buscarAulasPorData(const char *data, Aula *destino, int max) {
    if (data == NULL || !validarData(data)) {
        return 0;
    }

    carregarAulasMemoria();

    int count = 0;

    for (int i = 0; i < total_aulas && count < max; i++) {
        if (strcmp(aulas[i].data, data) == 0) {
            destino[count] = aulas[i];
            count++;
        }
    }

    return count;
}

/*
 * Quando preciso fazer uma busca por período eu comparo strings de datas,
 * assumindo o formato DD/MM/AAAA. Faço a filtragem respeitando os limites.
 */
int buscarAulasPorPeriodo(int id_turma,
                          const char *data_inicio,
                          const char *data_fim,
                          Aula *destino,
                          int max) {
    if (!validarData(data_inicio) || !validarData(data_fim)) {
        return 0;
    }

    carregarAulasMemoria();

    int count = 0;

    for (int i = 0; i < total_aulas && count < max; i++) {
        if (aulas[i].id_turma != id_turma) {
            continue;
        }

        if (strcmp(aulas[i].data, data_inicio) >= 0 &&
            strcmp(aulas[i].data, data_fim) <= 0) {
            destino[count] = aulas[i];
            count++;
        }
    }

    return count;
}

/*
 * Para saber quantas aulas uma turma teve, simplesmente conto os registros
 * cujo id_turma bate com o solicitado.
 */
int contarAulasDaTurma(int id_turma) {
    carregarAulasMemoria();

    int count = 0;

    for (int i = 0; i < total_aulas; i++) {
        if (aulas[i].id_turma == id_turma) {
            count++;
        }
    }

    return count;
}

/*
 * Eu gero um relatório em texto puro para entregar no PIM.
 * Abro o arquivo, escrevo cabeçalho, varro as aulas da turma e monto o rodapé.
 */
int gerarRelatorioTurma(int id_turma, const char *arquivo_destino) {
    carregarAulasMemoria();

    FILE *relatorio = fopen(arquivo_destino, "w");
    if (relatorio == NULL) {
        printf("Erro ao criar arquivo de relatório.\n");
        return 0;
    }

    fprintf(relatorio, "========================================\n");
    fprintf(relatorio, "  DIÁRIO DE CLASSE - TURMA ID %d\n", id_turma);
    fprintf(relatorio, "========================================\n\n");

    int aulas_encontradas = 0;

    for (int i = 0; i < total_aulas; i++) {
        if (aulas[i].id_turma == id_turma) {
            fprintf(relatorio, "Data: %s\n", aulas[i].data);
            fprintf(relatorio, "Conteúdo: %s\n", aulas[i].conteudo);
            fprintf(relatorio, "----------------------------------------\n\n");
            aulas_encontradas++;
        }
    }

    fprintf(relatorio, "\n========================================\n");
    fprintf(relatorio, "Total de aulas ministradas: %d\n", aulas_encontradas);
    fprintf(relatorio, "========================================\n");

    fclose(relatorio);

    printf("Relatório gerado com sucesso: %s\n", arquivo_destino);
    printf("Total de aulas: %d\n", aulas_encontradas);

    return 1;
}

/*
 * Este utilitário vasculha os IDs existentes e devolve o próximo número livre.
 * Uso sempre que quero registrar uma nova aula sem colisões.
 */
int gerarProximoIDAula(void) {
    carregarAulasMemoria();

    int maior_id = 0;

    for (int i = 0; i < total_aulas; i++) {
        if (aulas[i].id > maior_id) {
            maior_id = aulas[i].id;
        }
    }

    return maior_id + 1;
}

/*
 * Eu verifico se a data está em formato DD/MM/AAAA com caracteres válidos.
 * Faço checagens simples de tamanho, posição das barras e faixa de valores.
 */
int validarData(const char *data) {
    if (data == NULL) {
        return 0;
    }

    if (strlen(data) != 10) {
        return 0;
    }

    if (data[2] != '/' || data[5] != '/') {
        return 0;
    }

    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) {
            continue;
        }

        if (data[i] < '0' || data[i] > '9') {
            return 0;
        }
    }

    int dia = (data[0] - '0') * 10 + (data[1] - '0');
    int mes = (data[3] - '0') * 10 + (data[4] - '0');
    int ano = (data[6] - '0') * 1000 +
              (data[7] - '0') * 100 +
              (data[8] - '0') * 10 +
              (data[9] - '0');

    if (dia < 1 || dia > 31) {
        return 0;
    }

    if (mes < 1 || mes > 12) {
        return 0;
    }

    if (ano < 1900 || ano > 2100) {
        return 0;
    }

    return 1;
}

