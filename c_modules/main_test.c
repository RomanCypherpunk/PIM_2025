#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "aluno_manager.h"
#include "turma_manager.h"
#include "aula_manager.h"

// Cores para o terminal (opcional)
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// Função auxiliar para pausar entre testes
void pausar() {
    printf("\n%sPressione ENTER para continuar...%s", YELLOW, RESET);
    getchar();
    printf("\n");
}

// Função auxiliar para limpar o buffer do teclado
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Exibe menu de testes
void exibirMenu() {
    printf("\n");
    printf("%s╔════════════════════════════════════════════╗%s\n", CYAN, RESET);
    printf("%s║  SISTEMA DE TESTES - PIM II (C Modules)   ║%s\n", CYAN, RESET);
    printf("%s╚════════════════════════════════════════════╝%s\n", CYAN, RESET);
    printf("\n%s[1]%s Testar Módulo de Alunos (CRUD)\n", GREEN, RESET);
    printf("%s[2]%s Testar Módulo de Turmas (CRUD)\n", GREEN, RESET);
    printf("%s[3]%s Testar Associação Aluno-Turma\n", GREEN, RESET);
    printf("%s[4]%s Testar Módulo de Aulas (Diário Eletrônico)\n", GREEN, RESET);
    printf("%s[5]%s Testar Geração de Relatórios\n", GREEN, RESET);
    printf("%s[6]%s Executar TODOS os Testes Automaticamente\n", MAGENTA, RESET);
    printf("%s[0]%s Sair\n", RED, RESET);
    printf("\n%sEscolha uma opção: %s", YELLOW, RESET);
}

// ========== TESTE 1: MÓDULO DE ALUNOS ==========
void testarModuloAlunos() {
    printf("\n%s═══════════════════════════════════════════%s\n", BLUE, RESET);
    printf("%s  TESTE 1: MÓDULO DE ALUNOS (CRUD)%s\n", BLUE, RESET);
    printf("%s═══════════════════════════════════════════%s\n", BLUE, RESET);
    
    // Criar alunos de teste
    Aluno a1 = {12345, "João Silva Santos", "joao.silva@unip.br", 1};
    Aluno a2 = {12346, "Maria Oliveira Costa", "maria.oliveira@unip.br", 1};
    Aluno a3 = {12347, "Pedro Henrique Souza", "pedro.souza@unip.br", 1};
    
    printf("\n%s[TESTE]%s Cadastrando 3 alunos...\n", YELLOW, RESET);
    cadastrarAluno(&a1);
    cadastrarAluno(&a2);
    cadastrarAluno(&a3);
    
    printf("\n%s[TESTE]%s Listando todos os alunos cadastrados:\n", YELLOW, RESET);
    Aluno lista[100];
    int total = listarAlunos(lista, 100);
    printf("\n%sTotal de alunos: %d%s\n", GREEN, total, RESET);
    for (int i = 0; i < total; i++) {
        printf("  RA: %d | Nome: %-30s | Email: %-25s | Status: %s\n", 
               lista[i].ra, 
               lista[i].nome, 
               lista[i].email,
               lista[i].ativo ? "Ativo" : "Inativo");
    }
    
    printf("\n%s[TESTE]%s Buscando aluno por RA (12345)...\n", YELLOW, RESET);
    Aluno *encontrado = buscarAlunoPorRA(12345);
    if (encontrado != NULL) {
        printf("  %s✓ Aluno encontrado:%s %s\n", GREEN, RESET, encontrado->nome);
    } else {
        printf("  %s✗ Aluno não encontrado!%s\n", RED, RESET);
    }
    
    printf("\n%s[TESTE]%s Atualizando email do aluno RA 12346...\n", YELLOW, RESET);
    a2.ra = 12346;
    strcpy(a2.email, "maria.novo@unip.br");
    atualizarAluno(&a2);
    
    printf("\n%s[TESTE]%s Desativando aluno RA 12347...\n", YELLOW, RESET);
    excluirAluno(12347);
    
    printf("\n%s[TESTE]%s Listagem após modificações:\n", YELLOW, RESET);
    total = listarAlunos(lista, 100);
    for (int i = 0; i < total; i++) {
        printf("  RA: %d | Nome: %-30s | Status: %s\n", 
               lista[i].ra, 
               lista[i].nome,
               lista[i].ativo ? GREEN "Ativo" RESET : RED "Inativo" RESET);
    }
    
    printf("\n%s✅ TESTE 1 CONCLUÍDO!%s\n", GREEN, RESET);
}

// ========== TESTE 2: MÓDULO DE TURMAS ==========
void testarModuloTurmas() {
    printf("\n%s═══════════════════════════════════════════%s\n", BLUE, RESET);
    printf("%s  TESTE 2: MÓDULO DE TURMAS (CRUD)%s\n", BLUE, RESET);
    printf("%s═══════════════════════════════════════════%s\n", BLUE, RESET);
    
    // Criar turmas de teste
    Turma t1 = {gerarProximoIDTurma(), "ADS-2A", "Prof. Carlos Silva", 2025, 1};
    Turma t2 = {gerarProximoIDTurma(), "ADS-2B", "Prof. Ana Paula", 2025, 1};
    Turma t3 = {gerarProximoIDTurma(), "SI-3A", "Prof. Roberto Lima", 2025, 1};
    
    printf("\n%s[TESTE]%s Cadastrando 3 turmas...\n", YELLOW, RESET);
    cadastrarTurma(&t1);
    cadastrarTurma(&t2);
    cadastrarTurma(&t3);
    
    printf("\n%s[TESTE]%s Listando todas as turmas cadastradas:\n", YELLOW, RESET);
    Turma lista[100];
    int total = listarTurmas(lista, 100);
    printf("\n%sTotal de turmas: %d%s\n", GREEN, total, RESET);
    for (int i = 0; i < total; i++) {
        printf("  ID: %d | Turma: %-10s | Professor: %-20s | %d/%d\n", 
               lista[i].id, 
               lista[i].nome, 
               lista[i].professor,
               lista[i].ano,
               lista[i].semestre);
    }
    
    printf("\n%s[TESTE]%s Buscando turma por ID (%d)...\n", YELLOW, t1.id, RESET);
    Turma *encontrada = buscarTurmaPorID(t1.id);
    if (encontrada != NULL) {
        printf("  %s✓ Turma encontrada:%s %s (Prof. %s)\n", 
               GREEN, RESET, encontrada->nome, encontrada->professor);
    } else {
        printf("  %s✗ Turma não encontrada!%s\n", RED, RESET);
    }
    
    printf("\n%s[TESTE]%s Atualizando professor da turma %s...\n", YELLOW, RESET, t2.nome);
    t2.id = lista[1].id; // Usar o ID da segunda turma
    strcpy(t2.professor, "Prof. Mariana Costa");
    atualizarTurma(&t2);
    
    printf("\n%s✅ TESTE 2 CONCLUÍDO!%s\n", GREEN, RESET);
}

// ========== TESTE 3: ASSOCIAÇÃO ALUNO-TURMA ==========
void testarAssociacaoAlunoTurma() {
    printf("\n%s═══════════════════════════════════════════%s\n", BLUE, RESET);
    printf("%s  TESTE 3: ASSOCIAÇÃO ALUNO-TURMA%s\n", BLUE, RESET);
    printf("%s═══════════════════════════════════════════%s\n", BLUE, RESET);
    
    // Buscar IDs das turmas existentes
    Turma turmas[10];
    int total_turmas = listarTurmas(turmas, 10);
    
    if (total_turmas == 0) {
        printf("%s⚠ Nenhuma turma encontrada. Execute o Teste 2 primeiro!%s\n", YELLOW, RESET);
        return;
    }
    
    printf("\n%s[TESTE]%s Matriculando alunos nas turmas...\n", YELLOW, RESET);
    associarAlunoTurma(12345, turmas[0].id); // João na primeira turma
    associarAlunoTurma(12346, turmas[0].id); // Maria na primeira turma
    associarAlunoTurma(12345, turmas[1].id); // João também na segunda turma
    
    printf("\n%s[TESTE]%s Listando alunos da turma '%s' (ID: %d):\n", 
           YELLOW, RESET, turmas[0].nome, turmas[0].id);
    int ras[100];
    int total_alunos = listarAlunosDaTurma(turmas[0].id, ras, 100);
    printf("  %sTotal de alunos matriculados: %d%s\n", GREEN, total_alunos, RESET);
    for (int i = 0; i < total_alunos; i++) {
        Aluno *aluno = buscarAlunoPorRA(ras[i]);
        if (aluno != NULL) {
            printf("    - RA %d: %s\n", ras[i], aluno->nome);
        }
    }
    
    printf("\n%s[TESTE]%s Listando turmas do aluno RA 12345:\n", YELLOW, RESET);
    int ids_turmas[100];
    int total_turmas_aluno = listarTurmasDoAluno(12345, ids_turmas, 100);
    printf("  %sTotal de turmas: %d%s\n", GREEN, total_turmas_aluno, RESET);
    for (int i = 0; i < total_turmas_aluno; i++) {
        Turma *turma = buscarTurmaPorID(ids_turmas[i]);
        if (turma != NULL) {
            printf("    - %s (Prof. %s)\n", turma->nome, turma->professor);
        }
    }
    
    printf("\n%s[TESTE]%s Verificando matrícula do aluno 12345 na turma %d...\n", 
           YELLOW, RESET, turmas[0].id);
    if (verificarMatricula(12345, turmas[0].id)) {
        printf("  %s✓ Aluno está matriculado!%s\n", GREEN, RESET);
    } else {
        printf("  %s✗ Aluno NÃO está matriculado!%s\n", RED, RESET);
    }
    
    printf("\n%s✅ TESTE 3 CONCLUÍDO!%s\n", GREEN, RESET);
}

// ========== TESTE 4: MÓDULO DE AULAS ==========
void testarModuloAulas() {
    printf("\n%s═══════════════════════════════════════════%s\n", BLUE, RESET);
    printf("%s  TESTE 4: DIÁRIO ELETRÔNICO (AULAS)%s\n", BLUE, RESET);
    printf("%s═══════════════════════════════════════════%s\n", BLUE, RESET);
    
    // Buscar turma existente
    Turma turmas[10];
    int total_turmas = listarTurmas(turmas, 10);
    
    if (total_turmas == 0) {
        printf("%s⚠ Nenhuma turma encontrada. Execute o Teste 2 primeiro!%s\n", YELLOW, RESET);
        return;
    }
    
    int id_turma = turmas[0].id;
    
    // Criar aulas de teste
    Aula aula1 = {gerarProximoIDAula(), id_turma, "15/10/2025", 
                  "Introdução à Programação em C - Estruturas de decisão"};
    Aula aula2 = {gerarProximoIDAula(), id_turma, "17/10/2025", 
                  "Estruturas de repetição: for, while e do-while"};
    Aula aula3 = {gerarProximoIDAula(), id_turma, "22/10/2025", 
                  "Ponteiros e alocação dinâmica de memória"};
    
    printf("\n%s[TESTE]%s Registrando 3 aulas no diário da turma '%s'...\n", 
           YELLOW, RESET, turmas[0].nome);
    registrarAula(&aula1);
    registrarAula(&aula2);
    registrarAula(&aula3);
    
    printf("\n%s[TESTE]%s Listando aulas da turma '%s':\n", 
           YELLOW, RESET, turmas[0].nome);
    Aula lista_aulas[100];
    int total_aulas = listarAulasDaTurma(id_turma, lista_aulas, 100);
    printf("  %sTotal de aulas: %d%s\n", GREEN, total_aulas, RESET);
    for (int i = 0; i < total_aulas; i++) {
        printf("\n  Aula #%d:\n", lista_aulas[i].id);
        printf("    Data: %s\n", lista_aulas[i].data);
        printf("    Conteúdo: %s\n", lista_aulas[i].conteudo);
    }
    
    printf("\n%s[TESTE]%s Contando aulas da turma...\n", YELLOW, RESET);
    int count = contarAulasDaTurma(id_turma);
    printf("  %sTotal: %d aulas ministradas%s\n", GREEN, count, RESET);
    
    printf("\n%s✅ TESTE 4 CONCLUÍDO!%s\n", GREEN, RESET);
}

// ========== TESTE 5: GERAÇÃO DE RELATÓRIOS ==========
void testarRelatorios() {
    printf("\n%s═══════════════════════════════════════════%s\n", BLUE, RESET);
    printf("%s  TESTE 5: GERAÇÃO DE RELATÓRIOS%s\n", BLUE, RESET);
    printf("%s═══════════════════════════════════════════%s\n", BLUE, RESET);
    
    // Buscar turma existente
    Turma turmas[10];
    int total_turmas = listarTurmas(turmas, 10);
    
    if (total_turmas == 0) {
        printf("%s⚠ Nenhuma turma encontrada!%s\n", YELLOW, RESET);
        return;
    }
    
    int id_turma = turmas[0].id;
    char arquivo_relatorio[100];
    sprintf(arquivo_relatorio, "data/relatorio_turma_%d.txt", id_turma);
    
    printf("\n%s[TESTE]%s Gerando relatório digital da turma '%s'...\n", 
           YELLOW, RESET, turmas[0].nome);
    printf("  Arquivo: %s\n", arquivo_relatorio);
    
    if (gerarRelatorioTurma(id_turma, arquivo_relatorio)) {
        printf("\n  %s✓ Relatório gerado com sucesso!%s\n", GREEN, RESET);
        printf("  %s📄 Veja o arquivo '%s'%s\n", CYAN, arquivo_relatorio, RESET);
    } else {
        printf("  %s✗ Erro ao gerar relatório!%s\n", RED, RESET);
    }
    
    printf("\n%s✅ TESTE 5 CONCLUÍDO!%s\n", GREEN, RESET);
}

// ========== EXECUTAR TODOS OS TESTES ==========
void executarTodosTestes() {
    printf("\n%s╔═══════════════════════════════════════════╗%s\n", MAGENTA, RESET);
    printf("%s║   EXECUTANDO TODOS OS TESTES...          ║%s\n", MAGENTA, RESET);
    printf("%s╚═══════════════════════════════════════════╝%s\n", MAGENTA, RESET);
    
    testarModuloAlunos();
    pausar();
    
    testarModuloTurmas();
    pausar();
    
    testarAssociacaoAlunoTurma();
    pausar();
    
    testarModuloAulas();
    pausar();
    
    testarRelatorios();
    
    printf("\n%s╔═══════════════════════════════════════════╗%s\n", GREEN, RESET);
    printf("%s║  ✅ TODOS OS TESTES FORAM CONCLUÍDOS! ✅  ║%s\n", GREEN, RESET);
    printf("%s╚═══════════════════════════════════════════╝%s\n", GREEN, RESET);
}

// ========== MAIN ==========
int main() {
    int opcao;
    
    do {
        exibirMenu();
        
        if (scanf("%d", &opcao) != 1) {
            limparBuffer();
            printf("%s\n✗ Opção inválida! Digite um número.%s\n", RED, RESET);
            continue;
        }
        limparBuffer();
        
        switch (opcao) {
            case 1:
                testarModuloAlunos();
                pausar();
                break;
            case 2:
                testarModuloTurmas();
                pausar();
                break;
            case 3:
                testarAssociacaoAlunoTurma();
                pausar();
                break;
            case 4:
                testarModuloAulas();
                pausar();
                break;
            case 5:
                testarRelatorios();
                pausar();
                break;
            case 6:
                executarTodosTestes();
                pausar();
                break;
            case 0:
                printf("\n%s👋 Encerrando sistema de testes...%s\n\n", CYAN, RESET);
                break;
            default:
                printf("%s\n✗ Opção inválida! Escolha entre 0 e 6.%s\n", RED, RESET);
        }
        
    } while (opcao != 0);
    
    return 0;
}