#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "structs.h"
#include "aluno_manager.h"
#include "turma_manager.h"
#include "aula_manager.h"
#include "atividade_manager.h"
#include "usuario_manager.h"

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

static void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descarta ate o fim da linha */
    }
}

static void aguardarEnter(void) {
    int c;
    printf("\n%sPressione ENTER para continuar...%s", YELLOW, RESET);
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descarta qualquer texto digitado antes do ENTER */
    }
    printf("\n");
}

static void imprimirDivisor(const char *cor) {
    printf("%s==============================================%s\n", cor, RESET);
}

static void imprimirTitulo(const char *titulo, const char *cor) {
    printf("\n");
    imprimirDivisor(cor);
    printf("%s  %s%s\n", cor, titulo, RESET);
    imprimirDivisor(cor);
}

static int gerarRaNovo(void) {
    Aluno buffer[MAX_ALUNOS];
    int total = listarAlunos(buffer, MAX_ALUNOS);
    int maior = 1000;

    for (int i = 0; i < total; i++) {
        if (buffer[i].ra > maior) {
            maior = buffer[i].ra;
        }
    }

    return maior + 1;
}

static void gerarHashSenha(const char *senha, char *destino, size_t tamanho) {
    unsigned long hash = 5381;
    int c;
    const char *ptr = senha;

    while ((c = *ptr++) != 0) {
        hash = ((hash << 5) + hash) + (unsigned long)c;
    }

    snprintf(destino, tamanho, "%lu", hash);
}

static void gerarLoginTeste(char *destino, size_t tamanho) {
    static int sequencia = 1;
    long marcador = (long)time(NULL) % 100000;
    snprintf(destino, tamanho, "user_teste_%ld_%d", marcador, sequencia++);
}

static void exibirMenu(void) {
    puts("");
    imprimirDivisor(CYAN);
    printf("%s  SISTEMA DE TESTES - PIM 2025%s\n", CYAN, RESET);
    imprimirDivisor(CYAN);
    puts("");
    printf("%s[1]%s Testar modulo de Alunos (CRUD)\n", GREEN, RESET);
    printf("%s[2]%s Testar modulo de Turmas (CRUD)\n", GREEN, RESET);
    printf("%s[3]%s Testar associacao Aluno-Turma\n", GREEN, RESET);
    printf("%s[4]%s Testar modulo de Aulas\n", GREEN, RESET);
    printf("%s[5]%s Testar modulo de Atividades\n", GREEN, RESET);
    printf("%s[6]%s Testar modulo de Usuarios\n", GREEN, RESET);
    printf("%s[7]%s Testar geracao de Relatorios\n", GREEN, RESET);
    printf("%s[8]%s Executar todos os testes\n", MAGENTA, RESET);
    printf("%s[0]%s Sair\n", RED, RESET);
    printf("\n%sEscolha uma opcao: %s", YELLOW, RESET);
}

static void testarModuloAlunos(void) {
    imprimirTitulo("TESTE: MODULO DE ALUNOS (CRUD)", BLUE);

    int raBase = gerarRaNovo();

    Aluno aluno1 = {raBase, "Aluno Teste 1", "aluno1@pim.com", 1};
    Aluno aluno2 = {raBase + 1, "Aluno Teste 2", "aluno2@pim.com", 1};
    Aluno aluno3 = {raBase + 2, "Aluno Teste 3", "aluno3@pim.com", 1};

    printf("%sCadastro de tres alunos...\n%s", YELLOW, RESET);
    cadastrarAluno(&aluno1);
    cadastrarAluno(&aluno2);
    cadastrarAluno(&aluno3);

    Aluno lista[16];
    int total = listarAlunos(lista, 16);
    printf("\n%sAlunos cadastrados (ate 16 registros):%s\n", YELLOW, RESET);
    for (int i = 0; i < total; i++) {
        printf("  RA: %d | Nome: %-20s | Status: %s\n",
               lista[i].ra,
               lista[i].nome,
               lista[i].ativo ? "Ativo" : "Inativo");
    }

    printf("\n%sBusca do aluno de RA %d...%s\n", YELLOW, raBase, RESET);
    Aluno *encontrado = buscarAlunoPorRA(raBase);
    if (encontrado) {
        printf("  Encontrado: %s (%s)\n", encontrado->nome, encontrado->email);
    } else {
        printf("  Nao encontrado.\n");
    }

    printf("\n%sAtualizacao do email do RA %d...%s\n", YELLOW, raBase, RESET);
    if (encontrado) {
        Aluno atualizado = *encontrado;
        strcpy(atualizado.email, "aluno1.atualizado@pim.com");
        atualizarAluno(&atualizado);
    }

    printf("\n%sDesativacao do RA %d...%s\n", YELLOW, raBase + 2, RESET);
    excluirAluno(raBase + 2);

    total = listarAlunos(lista, 16);
    printf("\n%sEstado final dos alunos cadastrados:%s\n", YELLOW, RESET);
    for (int i = 0; i < total; i++) {
        printf("  RA: %d | Nome: %-20s | Email: %-28s | Status: %s\n",
               lista[i].ra,
               lista[i].nome,
               lista[i].email,
               lista[i].ativo ? "Ativo" : "Inativo");
    }

    printf("\n%sModulo de alunos testado com sucesso.%s\n", GREEN, RESET);
}

static void testarModuloTurmas(void) {
    imprimirTitulo("TESTE: MODULO DE TURMAS (CRUD)", BLUE);

    int idBase = gerarProximoIDTurma();

    Turma turma1 = {idBase, "ADS Teste A", "Professora Ana", 2025, 1};
    Turma turma2 = {idBase + 1, "ADS Teste B", "Professor Bruno", 2025, 2};

    printf("%sCadastro de duas turmas...\n%s", YELLOW, RESET);
    cadastrarTurma(&turma1);
    cadastrarTurma(&turma2);

    Turma lista[16];
    int total = listarTurmas(lista, 16);
    printf("\n%sTurmas cadastradas (ate 16 registros):%s\n", YELLOW, RESET);
    for (int i = 0; i < total; i++) {
        printf("  ID: %d | Nome: %-15s | Professor: %-18s | Ano/Sem: %d/%d\n",
               lista[i].id,
               lista[i].nome,
               lista[i].professor,
               lista[i].ano,
               lista[i].semestre);
    }

    printf("\n%sBusca pela turma ID %d...%s\n", YELLOW, idBase, RESET);
    Turma *turmaEncontrada = buscarTurmaPorID(idBase);
    if (turmaEncontrada) {
        printf("  Encontrada: %s (Professor %s)\n",
               turmaEncontrada->nome,
               turmaEncontrada->professor);
    }

    printf("\n%sAtualizacao do professor da turma %d...%s\n", YELLOW, idBase, RESET);
    if (turmaEncontrada) {
        Turma alterada = *turmaEncontrada;
        strcpy(alterada.professor, "Professor Carlos");
        atualizarTurma(&alterada);
    }

    printf("\n%sExclusao da turma ID %d...%s\n", YELLOW, idBase + 1, RESET);
    excluirTurma(idBase + 1);

    total = listarTurmas(lista, 16);
    printf("\n%sEstado final das turmas:%s\n", YELLOW, RESET);
    for (int i = 0; i < total; i++) {
        printf("  ID: %d | Nome: %-15s | Professor: %-18s | Ano/Sem: %d/%d\n",
               lista[i].id,
               lista[i].nome,
               lista[i].professor,
               lista[i].ano,
               lista[i].semestre);
    }

    printf("\n%sModulo de turmas testado com sucesso.%s\n", GREEN, RESET);
}

static void testarAssociacaoAlunoTurma(void) {
    imprimirTitulo("TESTE: ASSOCIACAO ALUNO-TURMA", BLUE);

    int ra = gerarRaNovo();
    int turmaId = gerarProximoIDTurma();

    Aluno aluno = {ra, "Aluno Vinculo", "aluno.vinculo@pim.com", 1};
    Turma turma = {turmaId, "ADS Vinculo", "Professor Diego", 2025, 1};

    cadastrarAluno(&aluno);
    cadastrarTurma(&turma);

    printf("\n%sMatriculando aluno RA %d na turma %d...%s\n", YELLOW, ra, turmaId, RESET);
    associarAlunoTurma(ra, turmaId);

    int ras[32];
    int totalAlunos = listarAlunosDaTurma(turmaId, ras, 32);
    printf("\n%sAlunos da turma %d:%s\n", YELLOW, turmaId, RESET);
    for (int i = 0; i < totalAlunos; i++) {
        printf("  RA: %d\n", ras[i]);
    }

    int turmas[32];
    int totalTurmas = listarTurmasDoAluno(ra, turmas, 32);
    printf("\n%sTurmas do aluno RA %d:%s\n", YELLOW, ra, RESET);
    for (int i = 0; i < totalTurmas; i++) {
        printf("  Turma ID: %d\n", turmas[i]);
    }

    printf("\n%sRemovendo aluno da turma...%s\n", YELLOW, RESET);
    removerAlunoTurma(ra, turmaId);

    printf("\n%sAssociacao aluno-turma testada com sucesso.%s\n", GREEN, RESET);
}

static void testarModuloAulas(void) {
    imprimirTitulo("TESTE: MODULO DE AULAS", BLUE);

    int turmaId = gerarProximoIDTurma();
    Turma turma = {turmaId, "ADS Diario", "Professora Elisa", 2025, 2};
    cadastrarTurma(&turma);

    int aulaIdBase = gerarProximoIDAula();

    Aula aula1 = {aulaIdBase, turmaId, "10/03/2025", "Introducao ao projeto integrador"};
    Aula aula2 = {aulaIdBase + 1, turmaId, "17/03/2025", "Modelagem de dados"};

    printf("%sRegistrando aulas...\n%s", YELLOW, RESET);
    registrarAula(&aula1);
    registrarAula(&aula2);

    Aula lista[16];
    int total = listarAulasDaTurma(turmaId, lista, 16);
    printf("\n%sAulas da turma %d:%s\n", YELLOW, turmaId, RESET);
    for (int i = 0; i < total; i++) {
        printf("  Aula ID: %d | Data: %s | Conteudo: %s\n",
               lista[i].id,
               lista[i].data,
               lista[i].conteudo);
    }

    printf("\n%sBuscando aula ID %d...%s\n", YELLOW, aulaIdBase, RESET);
    Aula *encontrada = buscarAulaPorID(aulaIdBase);
    if (encontrada) {
        printf("  Aula encontrada em %s com conteudo: %s\n",
               encontrada->data,
               encontrada->conteudo);
    }

    printf("\n%sAtualizando conteudo da aula %d...%s\n", YELLOW, aulaIdBase, RESET);
    if (encontrada) {
        Aula alterada = *encontrada;
        strcpy(alterada.conteudo, "Introducao ao projeto integrador - revisao");
        atualizarAula(&alterada);
    }

    int totalAulas = contarAulasDaTurma(turmaId);
    printf("\n%sTotal de aulas registradas para a turma: %d%s\n", YELLOW, totalAulas, RESET);

    printf("\n%sExcluindo a aula %d...%s\n", YELLOW, aulaIdBase + 1, RESET);
    excluirAula(aulaIdBase + 1);

    printf("\n%sModulo de aulas testado com sucesso.%s\n", GREEN, RESET);
}

static void testarModuloAtividades(void) {
    imprimirTitulo("TESTE: MODULO DE ATIVIDADES", BLUE);

    int turmaId = gerarProximoIDTurma();
    Turma turma = {turmaId, "ADS Atividades", "Professor Fabio", 2025, 1};
    cadastrarTurma(&turma);

    int atividadeId = gerarProximoIDAtividade();

    Atividade atividade = {
        atividadeId,
        turmaId,
        "Atividade Avaliativa 1",
        "Desenvolver um CRUD completo.",
        "documents/modelo_crud.pdf"
    };

    printf("%sCadastrando atividade...\n%s", YELLOW, RESET);
    cadastrarAtividade(&atividade);

    Atividade lista[16];
    int total = listarAtividades(lista, 16);
    printf("\n%sAtividades cadastradas (ate 16 registros):%s\n", YELLOW, RESET);
    for (int i = 0; i < total; i++) {
        printf("  ID: %d | Turma: %d | Titulo: %s\n",
               lista[i].id,
               lista[i].id_turma,
               lista[i].titulo);
    }

    printf("\n%sAtualizando descricao da atividade ID %d...%s\n", YELLOW, atividadeId, RESET);
    Atividade *encontrada = buscarAtividadePorID(atividadeId);
    if (encontrada) {
        Atividade alterada = *encontrada;
        strcpy(alterada.descricao, "Desenvolver e documentar um CRUD completo.");
        atualizarAtividade(&alterada);
    }

    Atividade porTurma[16];
    int totalTurma = listarAtividadesDaTurma(turmaId, porTurma, 16);
    printf("\n%sAtividades da turma %d:%s\n", YELLOW, turmaId, RESET);
    for (int i = 0; i < totalTurma; i++) {
        printf("  ID: %d | Titulo: %s\n",
               porTurma[i].id,
               porTurma[i].titulo);
    }

    printf("\n%sExcluindo atividade ID %d...%s\n", YELLOW, atividadeId, RESET);
    excluirAtividade(atividadeId);

    printf("\n%sModulo de atividades testado com sucesso.%s\n", GREEN, RESET);
}

static void testarModuloUsuarios(void) {
    imprimirTitulo("TESTE: MODULO DE USUARIOS", BLUE);

    Usuario usuario;
    usuario.id = gerarProximoIDUsuario();
    gerarLoginTeste(usuario.login, sizeof(usuario.login));
    gerarHashSenha("Senha@123", usuario.senha_hash, sizeof(usuario.senha_hash));
    strcpy(usuario.tipo, "PROFESSOR");
    usuario.ativo = 1;

    printf("%sCadastrando usuario %s...\n%s", YELLOW, usuario.login, RESET);
    cadastrarUsuario(&usuario);

    Usuario lista[16];
    int total = listarUsuarios(lista, 16);
    printf("\n%sUsuarios cadastrados (ate 16 registros):%s\n", YELLOW, RESET);
    for (int i = 0; i < total; i++) {
        printf("  ID: %d | Login: %-16s | Tipo: %-10s | Status: %s\n",
               lista[i].id,
               lista[i].login,
               lista[i].tipo,
               lista[i].ativo ? "Ativo" : "Inativo");
    }

    printf("\n%sAtualizando tipo do usuario ID %d...%s\n", YELLOW, usuario.id, RESET);
    Usuario *encontrado = buscarUsuarioPorID(usuario.id);
    if (encontrado) {
        Usuario alterado = *encontrado;
        strcpy(alterado.tipo, "ADMIN");
        atualizarUsuario(&alterado);
    }

    printf("\n%sDesativando usuario ID %d...%s\n", YELLOW, usuario.id, RESET);
    excluirUsuario(usuario.id);

    printf("\n%sModulo de usuarios testado com sucesso.%s\n", GREEN, RESET);
}

static void testarRelatorios(void) {
    imprimirTitulo("TESTE: GERACAO DE RELATORIOS", BLUE);

    int turmaId = gerarProximoIDTurma();
    Turma turma = {turmaId, "ADS Relatorios", "Professora Gabi", 2025, 1};
    cadastrarTurma(&turma);

    int aulaId = gerarProximoIDAula();

    Aula aula1 = {aulaId, turmaId, "05/04/2025", "Apresentacao do plano de ensino"};
    Aula aula2 = {aulaId + 1, turmaId, "12/04/2025", "Oficina de requisitos"};

    registrarAula(&aula1);
    registrarAula(&aula2);

    char arquivo[128];
    snprintf(arquivo, sizeof(arquivo), "data/relatorio_turma_%d.txt", turmaId);

    printf("\n%sGerando relatorio da turma %s (ID %d)...%s\n",
           YELLOW, turma.nome, turmaId, RESET);

    if (gerarRelatorioTurma(turmaId, arquivo)) {
        printf("  Relatorio salvo em: %s\n", arquivo);
    } else {
        printf("  Falha ao gerar relatorio.\n");
    }

    printf("\n%sGeracao de relatorio testada com sucesso.%s\n", GREEN, RESET);
}

static void executarTodosTestes(void) {
    imprimirTitulo("EXECUTANDO TODOS OS TESTES", MAGENTA);

    testarModuloAlunos();
    aguardarEnter();

    testarModuloTurmas();
    aguardarEnter();

    testarAssociacaoAlunoTurma();
    aguardarEnter();

    testarModuloAulas();
    aguardarEnter();

    testarModuloAtividades();
    aguardarEnter();

    testarModuloUsuarios();
    aguardarEnter();

    testarRelatorios();

    imprimirTitulo("TODOS OS TESTES FORAM EXECUTADOS", GREEN);
}

int main(void) {
    int opcao;

    do {
        exibirMenu();
        if (scanf("%d", &opcao) != 1) {
            printf("%s\nOpcao invalida! Digite um numero.%s\n", RED, RESET);
            limparBuffer();
            continue;
        }
        limparBuffer();

        switch (opcao) {
            case 1:
                testarModuloAlunos();
                aguardarEnter();
                break;
            case 2:
                testarModuloTurmas();
                aguardarEnter();
                break;
            case 3:
                testarAssociacaoAlunoTurma();
                aguardarEnter();
                break;
            case 4:
                testarModuloAulas();
                aguardarEnter();
                break;
            case 5:
                testarModuloAtividades();
                aguardarEnter();
                break;
            case 6:
                testarModuloUsuarios();
                aguardarEnter();
                break;
            case 7:
                testarRelatorios();
                aguardarEnter();
                break;
            case 8:
                executarTodosTestes();
                aguardarEnter();
                break;
            case 0:
                printf("\n%sEncerrando sistema de testes...%s\n", CYAN, RESET);
                break;
            default:
                printf("%s\nOpcao invalida! Escolha entre 0 e 8.%s\n", RED, RESET);
        }
    } while (opcao != 0);

    return 0;
}
