#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aula_manager.h"
#include "file_manager.h"

// ========== ARRAYS GLOBAIS (EM MEMÓRIA) ==========
static Aula aulas[MAX_AULAS];
static int total_aulas = 0;

// ========== FUNÇÕES AUXILIARES PRIVADAS ==========

// Carrega aulas do arquivo para memória
static void carregarAulasMemoria(void) {
    total_aulas = carregarDados(ARQUIVO_AULAS, aulas, MAX_AULAS, TIPO_AULA);
}

// Salva aulas da memória para o arquivo
static void salvarAulasArquivo(void) {
    salvarDados(ARQUIVO_AULAS, aulas, total_aulas, TIPO_AULA);
}

// ========== IMPLEMENTAÇÃO DAS FUNÇÕES PÚBLICAS ==========

// Registrar uma nova aula no diário eletrônico
int registrarAula(Aula *aula) {
    // Estrutura de decisão (requisito obrigatório)
    if (aula == NULL) {
        printf("Erro: dados da aula inválidos.\n");
        return 0;
    }
    
    // Validar data
    if (!validarData(aula->data)) {
        printf("Erro: data inválida. Use formato DD/MM/AAAA.\n");
        return 0;
    }
    
    carregarAulasMemoria();
    
    // Verificar se ID já existe
    for (int i = 0; i < total_aulas; i++) {
        if (aulas[i].id == aula->id) {
            printf("Erro: ID %d já cadastrado.\n", aula->id);
            return 0;
        }
    }
    
    // Verificar limite
    if (total_aulas >= MAX_AULAS) {
        printf("Erro: limite de aulas atingido.\n");
        return 0;
    }
    
    // Adicionar nova aula
    aulas[total_aulas] = *aula;
    total_aulas++;
    salvarAulasArquivo();
    
    printf("Aula registrada com sucesso no diário eletrônico!\n");
    return 1;
}

// Buscar aula por ID (uso de ponteiro - requisito desejável)
Aula* buscarAulaPorID(int id) {
    carregarAulasMemoria();
    
    // Estrutura de repetição (requisito obrigatório)
    for (int i = 0; i < total_aulas; i++) {
        if (aulas[i].id == id) {
            return &aulas[i]; // Retorna ponteiro
        }
    }
    
    return NULL; // Não encontrada
}

// Listar todas as aulas de uma turma específica
int listarAulasDaTurma(int id_turma, Aula *destino, int max) {
    carregarAulasMemoria();
    
    int count = 0;
    
    // Estrutura de repetição com decisão (requisito obrigatório)
    for (int i = 0; i < total_aulas && count < max; i++) {
        if (aulas[i].id_turma == id_turma) {
            destino[count] = aulas[i];
            count++;
        }
    }
    
    return count;
}

// Listar todas as aulas
int listarTodasAulas(Aula *destino, int max) {
    carregarAulasMemoria();
    
    int count = (total_aulas < max) ? total_aulas : max;
    
    for (int i = 0; i < count; i++) {
        destino[i] = aulas[i];
    }
    
    return count;
}

// Atualizar dados de uma aula
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

// Excluir uma aula do diário
int excluirAula(int id) {
    carregarAulasMemoria();
    
    for (int i = 0; i < total_aulas; i++) {
        if (aulas[i].id == id) {
            // Remove a aula deslocando os elementos
            for (int j = i; j < total_aulas - 1; j++) {
                aulas[j] = aulas[j + 1];
            }
            total_aulas--;
            salvarAulasArquivo();
            
            printf("Aula excluída com sucesso!\n");
            return 1;
        }
    }
    
    printf("Erro: aula não encontrada.\n");
    return 0;
}

// ========== FUNÇÕES DE CONSULTA E RELATÓRIOS ==========

// Buscar aulas por data específica
int buscarAulasPorData(const char *data, Aula *destino, int max) {
    carregarAulasMemoria();
    
    int count = 0;
    
    for (int i = 0; i < total_aulas && count < max; i++) {
        // Comparação de strings (requisito de estrutura de decisão)
        if (strcmp(aulas[i].data, data) == 0) {
            destino[count] = aulas[i];
            count++;
        }
    }
    
    return count;
}

// Buscar aulas de uma turma em um período
int buscarAulasPorPeriodo(int id_turma, const char *data_inicio, 
                          const char *data_fim, Aula *destino, int max) {
    carregarAulasMemoria();
    
    int count = 0;
    
    // Estrutura de repetição com múltiplas condições
    for (int i = 0; i < total_aulas && count < max; i++) {
        if (aulas[i].id_turma == id_turma) {
            // Comparação de datas (simplificada - considera formato DD/MM/AAAA)
            if (strcmp(aulas[i].data, data_inicio) >= 0 && 
                strcmp(aulas[i].data, data_fim) <= 0) {
                destino[count] = aulas[i];
                count++;
            }
        }
    }
    
    return count;
}

// Contar total de aulas de uma turma
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

// Gerar relatório do diário de classe (Requisito de Sustentabilidade)
int gerarRelatorioTurma(int id_turma, const char *arquivo_destino) {
    carregarAulasMemoria();
    
    FILE *relatorio = fopen(arquivo_destino, "w");
    if (relatorio == NULL) {
        printf("Erro ao criar arquivo de relatório.\n");
        return 0;
    }
    
    // Cabeçalho do relatório
    fprintf(relatorio, "========================================\n");
    fprintf(relatorio, "  DIÁRIO DE CLASSE - TURMA ID %d\n", id_turma);
    fprintf(relatorio, "========================================\n\n");
    
    int aulas_encontradas = 0;
    
    // Estrutura de repetição para gerar relatório
    for (int i = 0; i < total_aulas; i++) {
        if (aulas[i].id_turma == id_turma) {
            fprintf(relatorio, "Data: %s\n", aulas[i].data);
            fprintf(relatorio, "Conteúdo: %s\n", aulas[i].conteudo);
            fprintf(relatorio, "----------------------------------------\n\n");
            aulas_encontradas++;
        }
    }
    
    // Rodapé do relatório
    fprintf(relatorio, "\n========================================\n");
    fprintf(relatorio, "Total de aulas ministradas: %d\n", aulas_encontradas);
    fprintf(relatorio, "========================================\n");
    
    fclose(relatorio);
    
    printf("Relatório gerado com sucesso: %s\n", arquivo_destino);
    printf("Total de aulas: %d\n", aulas_encontradas);
    
    return 1;
}

// ========== FUNÇÕES AUXILIARES ==========

// Gerar próximo ID disponível de aula
int gerarProximoIDAula(void) {
    carregarAulasMemoria();
    
    int maior_id = 0;
    
    // Encontra o maior ID existente
    for (int i = 0; i < total_aulas; i++) {
        if (aulas[i].id > maior_id) {
            maior_id = aulas[i].id;
        }
    }
    
    return maior_id + 1;
}

// Validar formato de data (DD/MM/AAAA)
int validarData(const char *data) {
    // Estrutura de decisão (requisito obrigatório)
    if (data == NULL) {
        return 0;
    }
    
    // Verifica tamanho (deve ter exatamente 10 caracteres)
    if (strlen(data) != 10) {
        return 0;
    }
    
    // Verifica se as barras estão nas posições corretas
    if (data[2] != '/' || data[5] != '/') {
        return 0;
    }
    
    // Verifica se os demais caracteres são dígitos
    for (int i = 0; i < 10; i++) {
        if (i != 2 && i != 5) { // Pula as barras
            if (data[i] < '0' || data[i] > '9') {
                return 0;
            }
        }
    }
    
    // Extrai dia, mês e ano
    int dia = (data[0] - '0') * 10 + (data[1] - '0');
    int mes = (data[3] - '0') * 10 + (data[4] - '0');
    int ano = (data[6] - '0') * 1000 + (data[7] - '0') * 100 + 
              (data[8] - '0') * 10 + (data[9] - '0');
    
    // Validações básicas
    if (dia < 1 || dia > 31) {
        return 0;
    }
    
    if (mes < 1 || mes > 12) {
        return 0;
    }
    
    if (ano < 1900 || ano > 2100) {
        return 0;
    }
    
    return 1; // Data válida
}