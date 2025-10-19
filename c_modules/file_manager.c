#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_manager.h"

// ========== SALVAR DADOS ==========
int salvarDados(const char *nome_arquivo, void *dados, int num_registros, int tipo) {
    FILE *arquivo;
    int i;
    
    // Estrutura de decisão (requisito obrigatório)
    if (dados == NULL || num_registros <= 0) {
        printf("Erro: dados inválidos para salvar.\n");
        return 0;
    }
    
    arquivo = fopen(nome_arquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo %s para escrita.\n", nome_arquivo);
        return 0;
    }
    
    // Estrutura de decisão switch (requisito obrigatório)
    switch (tipo) {
        case TIPO_ALUNO: {
            Aluno *alunos = (Aluno *)dados;
            fprintf(arquivo, "RA,Nome,Email,Ativo\n"); // Cabeçalho CSV
            
            // Estrutura de repetição (requisito obrigatório)
            for (i = 0; i < num_registros; i++) {
                fprintf(arquivo, "%d,%s,%s,%d\n",
                        alunos[i].ra,
                        alunos[i].nome,
                        alunos[i].email,
                        alunos[i].ativo);
            }
            break;
        }
        
        case TIPO_TURMA: {
            Turma *turmas = (Turma *)dados;
            fprintf(arquivo, "ID,Nome,Professor,Ano,Semestre\n");
            
            for (i = 0; i < num_registros; i++) {
                fprintf(arquivo, "%d,%s,%s,%d,%d\n",
                        turmas[i].id,
                        turmas[i].nome,
                        turmas[i].professor,
                        turmas[i].ano,
                        turmas[i].semestre);
            }
            break;
        }
        
        case TIPO_AULA: {
            Aula *aulas = (Aula *)dados;
            fprintf(arquivo, "ID,ID_Turma,Data,Conteudo\n");
            
            for (i = 0; i < num_registros; i++) {
                fprintf(arquivo, "%d,%d,%s,%s\n",
                        aulas[i].id,
                        aulas[i].id_turma,
                        aulas[i].data,
                        aulas[i].conteudo);
            }
            break;
        }

        case TIPO_ATIVIDADE: {
            Atividade *atividades = (Atividade *)dados;
            fprintf(arquivo, "ID,ID_Turma,Titulo,Descricao,Arquivo\n");

            for (i = 0; i < num_registros; i++) {
                fprintf(arquivo, "%d,%d,%s,%s,%s\n",
                        atividades[i].id,
                        atividades[i].id_turma,
                        atividades[i].titulo,
                        atividades[i].descricao,
                        atividades[i].path_arquivo);
            }
            break;
        }
        
        default:
            printf("Erro: tipo de dado desconhecido.\n");
            fclose(arquivo);
            return 0;
    }
    
    fclose(arquivo);
    printf("Dados salvos com sucesso em %s\n", nome_arquivo);
    return 1;
}

// ========== CARREGAR DADOS ==========
int carregarDados(const char *nome_arquivo, void *destino, int max_registros, int tipo) {
    FILE *arquivo;
    char linha[1024];
    int contador = 0;
    
    arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Aviso: arquivo %s não encontrado. Será criado ao salvar.\n", nome_arquivo);
        return 0;
    }
    
    // Pular cabeçalho
    fgets(linha, sizeof(linha), arquivo);
    
    // Estrutura de repetição while (requisito obrigatório)
    while (fgets(linha, sizeof(linha), arquivo) != NULL && contador < max_registros) {
        // Remove quebra de linha
        linha[strcspn(linha, "\n")] = 0;
        
        switch (tipo) {
            case TIPO_ALUNO: {
                Aluno *alunos = (Aluno *)destino;
                sscanf(linha, "%d,%[^,],%[^,],%d",
                       &alunos[contador].ra,
                       alunos[contador].nome,
                       alunos[contador].email,
                       &alunos[contador].ativo);
                break;
            }
            
            case TIPO_TURMA: {
                Turma *turmas = (Turma *)destino;
                sscanf(linha, "%d,%[^,],%[^,],%d,%d",
                       &turmas[contador].id,
                       turmas[contador].nome,
                       turmas[contador].professor,
                       &turmas[contador].ano,
                       &turmas[contador].semestre);
                break;
            }
            
            case TIPO_AULA: {
                Aula *aulas = (Aula *)destino;
                sscanf(linha, "%d,%d,%[^,],%[^\n]",
                       &aulas[contador].id,
                       &aulas[contador].id_turma,
                       aulas[contador].data,
                       aulas[contador].conteudo);
                break;
            }

            case TIPO_ATIVIDADE: {
                Atividade *atividades = (Atividade *)destino;
                sscanf(linha, "%d,%d,%[^,],%[^,],%[^\n]",
                       &atividades[contador].id,
                       &atividades[contador].id_turma,
                       atividades[contador].titulo,
                       atividades[contador].descricao,
                       atividades[contador].path_arquivo);
                break;
            }
        }
        
        contador++;
    }
    
    fclose(arquivo);
    printf("%d registros carregados de %s\n", contador, nome_arquivo);
    return contador;
}
