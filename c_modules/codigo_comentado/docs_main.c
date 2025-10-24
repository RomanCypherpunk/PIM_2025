#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "aluno_manager.h"
#include "turma_manager.h"
#include "aula_manager.h"
#include "atividade_manager.h"
#include "usuario_manager.h"

static int lerLinha(char *destino, size_t tamanho) {
    if (fgets(destino, (int)tamanho, stdin) == NULL) {
        return 0;
    }
    destino[strcspn(destino, "\n")] = '\0';
    return 1;
}

static void lerStringObrigatoria(const char *prompt, char *destino, size_t tamanho) {
    do {
        printf("%s", prompt);
        if (!lerLinha(destino, tamanho)) {
            destino[0] = '\0';
            continue;
        }
        if (destino[0] == '\0') {
            printf("Valor nao pode ser vazio. Tente novamente.\n");
        }
    } while (destino[0] == '\0');
}

static int lerInteiroObrigatorio(const char *prompt) {
    char buffer[64];
    int valor;

    while (1) {
        printf("%s", prompt);
        if (!lerLinha(buffer, sizeof(buffer))) {
            printf("Erro de leitura. Tente novamente.\n");
            continue;
        }
        if (sscanf(buffer, "%d", &valor) == 1) {
            return valor;
        }
        printf("Entrada invalida. Digite um numero inteiro.\n");
    }
}

static int lerInteiroOpcional(const char *prompt, int atual) {
    char buffer[64];
    int valor;

    while (1) {
        printf("%s", prompt);
        if (!lerLinha(buffer, sizeof(buffer))) {
            printf("Erro de leitura. Tente novamente.\n");
            continue;
        }
        if (buffer[0] == '\0') {
            return atual;
        }
        if (sscanf(buffer, "%d", &valor) == 1) {
            return valor;
        }
        printf("Entrada invalida. Digite um numero inteiro ou pressione ENTER para manter.\n");
    }
}

static void aguardarEnter(void) {
    char buffer[4];
    printf("\nPressione ENTER para continuar...");
    lerLinha(buffer, sizeof(buffer));
    printf("\n");
}

static void tornarMaiusculo(char *texto) {
    for (size_t i = 0; texto[i] != '\0'; i++) {
        texto[i] = (char)toupper((unsigned char)texto[i]);
    }
}

static int gerarRaDisponivel(void) {
    Aluno alunos[MAX_ALUNOS];
    int total = listarAlunos(alunos, MAX_ALUNOS);
    int maior = 1000;

    for (int i = 0; i < total; i++) {
        if (alunos[i].ra > maior) {
            maior = alunos[i].ra;
        }
    }

    return maior + 1;
}

static void cadastrarAlunoManual(void) {
    Aluno aluno;
    char resposta[8];

    printf("\n=== Cadastro de Aluno ===\n");
    printf("Gerar RA automaticamente? (s/n): ");
    lerLinha(resposta, sizeof(resposta));

    if (resposta[0] == 's' || resposta[0] == 'S') {
        aluno.ra = gerarRaDisponivel();
        printf("RA gerado: %d\n", aluno.ra);
    } else {
        aluno.ra = lerInteiroObrigatorio("Informe o RA: ");
    }

    lerStringObrigatoria("Nome completo: ", aluno.nome, sizeof(aluno.nome));
    lerStringObrigatoria("Email institucional: ", aluno.email, sizeof(aluno.email));

    aluno.ativo = 1;

    if (cadastrarAluno(&aluno)) {
        printf("Aluno cadastrado com sucesso!\n");
    } else {
        printf("Falha ao cadastrar aluno.\n");
    }

    aguardarEnter();
}

static void listarAlunosManual(void) {
    Aluno alunos[MAX_ALUNOS];
    int total = listarAlunos(alunos, MAX_ALUNOS);

    printf("\n=== Relatorio de Alunos (%d encontrados) ===\n", total);

    if (total == 0) {
        printf("Nenhum aluno cadastrado.\n");
        aguardarEnter();
        return;
    }

    printf("%-8s %-30s %-30s %-6s\n", "RA", "Nome", "Email", "Ativo");
    printf("-------------------------------------------------------------------------------\n");

    for (int i = 0; i < total; i++) {
        printf("%-8d %-30s %-30s %-6s\n",
               alunos[i].ra,
               alunos[i].nome,
               alunos[i].email,
               alunos[i].ativo ? "Sim" : "Nao");
    }

    aguardarEnter();
}

static void atualizarAlunoManual(void) {
    int ra = lerInteiroObrigatorio("\nInforme o RA do aluno: ");
    Aluno *encontrado = buscarAlunoPorRA(ra);

    if (encontrado == NULL) {
        printf("Aluno nao encontrado.\n");
        aguardarEnter();
        return;
    }

    Aluno atualizado = *encontrado;
    char buffer[MAX_NOME];

    printf("Nome atual: %s\n", atualizado.nome);
    printf("Novo nome (ENTER para manter): ");
    lerLinha(buffer, sizeof(buffer));
    if (buffer[0] != '\0') {
        strncpy(atualizado.nome, buffer, sizeof(atualizado.nome));
        atualizado.nome[sizeof(atualizado.nome) - 1] = '\0';
    }

    printf("Email atual: %s\n", atualizado.email);
    printf("Novo email (ENTER para manter): ");
    lerLinha(buffer, sizeof(buffer));
    if (buffer[0] != '\0') {
        strncpy(atualizado.email, buffer, sizeof(atualizado.email));
        atualizado.email[sizeof(atualizado.email) - 1] = '\0';
    }

    char prompt[64];
    snprintf(prompt, sizeof(prompt), "Ativo (1=Sim, 0=Nao) [%d]: ", atualizado.ativo);
    atualizado.ativo = lerInteiroOpcional(prompt, atualizado.ativo);

    if (atualizarAluno(&atualizado)) {
        printf("Aluno atualizado com sucesso!\n");
    } else {
        printf("Falha ao atualizar aluno.\n");
    }

    aguardarEnter();
}

static void excluirAlunoManual(void) {
    int ra = lerInteiroObrigatorio("\nInforme o RA do aluno para desativar: ");

    if (excluirAluno(ra)) {
        printf("Aluno desativado com sucesso.\n");
    } else {
        printf("Falha ao desativar aluno.\n");
    }

    aguardarEnter();
}

static void menuAlunos(void) {
    int opcao;

    do {
        printf("\n=== ALUNO MANAGER ===\n");
        printf("1. Cadastrar aluno\n");
        printf("2. Relatorio de alunos\n");
        printf("3. Alterar aluno\n");
        printf("4. Remover aluno (soft delete)\n");
        printf("0. Voltar ao menu principal\n");

        opcao = lerInteiroObrigatorio("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                cadastrarAlunoManual();
                break;
            case 2:
                listarAlunosManual();
                break;
            case 3:
                atualizarAlunoManual();
                break;
            case 4:
                excluirAlunoManual();
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}

static void cadastrarTurmaManual(void) {
    Turma turma;

    turma.id = gerarProximoIDTurma();
    printf("\n=== Cadastro de Turma ===\n");
    printf("ID gerado automaticamente: %d\n", turma.id);

    lerStringObrigatoria("Nome da turma: ", turma.nome, sizeof(turma.nome));
    lerStringObrigatoria("Nome do professor: ", turma.professor, sizeof(turma.professor));
    turma.ano = lerInteiroObrigatorio("Ano letivo: ");
    turma.semestre = lerInteiroObrigatorio("Semestre (1 ou 2): ");

    if (cadastrarTurma(&turma)) {
        printf("Turma cadastrada com sucesso!\n");
    } else {
        printf("Falha ao cadastrar turma.\n");
    }

    aguardarEnter();
}

static void listarTurmasManual(void) {
    Turma turmas[MAX_TURMAS];
    int total = listarTurmas(turmas, MAX_TURMAS);

    printf("\n=== Relatorio de Turmas (%d encontradas) ===\n", total);

    if (total == 0) {
        printf("Nenhuma turma cadastrada.\n");
        aguardarEnter();
        return;
    }

    printf("%-6s %-20s %-25s %-6s %-8s\n", "ID", "Nome", "Professor", "Ano", "Semestre");
    printf("-----------------------------------------------------------------------\n");

    for (int i = 0; i < total; i++) {
        printf("%-6d %-20s %-25s %-6d %-8d\n",
               turmas[i].id,
               turmas[i].nome,
               turmas[i].professor,
               turmas[i].ano,
               turmas[i].semestre);
    }

    aguardarEnter();
}

static void atualizarTurmaManual(void) {
    int id = lerInteiroObrigatorio("\nInforme o ID da turma: ");
    Turma *encontrada = buscarTurmaPorID(id);

    if (encontrada == NULL) {
        printf("Turma nao encontrada.\n");
        aguardarEnter();
        return;
    }

    Turma atualizada = *encontrada;
    char buffer[MAX_TURMA_NOME];

    printf("Nome atual: %s\n", atualizada.nome);
    printf("Novo nome (ENTER para manter): ");
    lerLinha(buffer, sizeof(buffer));
    if (buffer[0] != '\0') {
        strncpy(atualizada.nome, buffer, sizeof(atualizada.nome));
        atualizada.nome[sizeof(atualizada.nome) - 1] = '\0';
    }

    printf("Professor atual: %s\n", atualizada.professor);
    printf("Novo professor (ENTER para manter): ");
    lerLinha(buffer, sizeof(buffer));
    if (buffer[0] != '\0') {
        strncpy(atualizada.professor, buffer, sizeof(atualizada.professor));
        atualizada.professor[sizeof(atualizada.professor) - 1] = '\0';
    }

    char prompt[64];
    snprintf(prompt, sizeof(prompt), "Ano letivo [%d]: ", atualizada.ano);
    atualizada.ano = lerInteiroOpcional(prompt, atualizada.ano);

    snprintf(prompt, sizeof(prompt), "Semestre [%d]: ", atualizada.semestre);
    atualizada.semestre = lerInteiroOpcional(prompt, atualizada.semestre);

    if (atualizarTurma(&atualizada)) {
        printf("Turma atualizada com sucesso!\n");
    } else {
        printf("Falha ao atualizar turma.\n");
    }

    aguardarEnter();
}

static void excluirTurmaManual(void) {
    int id = lerInteiroObrigatorio("\nInforme o ID da turma para excluir: ");

    if (excluirTurma(id)) {
        printf("Turma excluida com sucesso.\n");
    } else {
        printf("Falha ao excluir turma.\n");
    }

    aguardarEnter();
}

static void associarAlunoTurmaManual(void) {
    printf("\n=== Matricula de Aluno em Turma ===\n");
    int ra = lerInteiroObrigatorio("RA do aluno: ");
    int idTurma = lerInteiroObrigatorio("ID da turma: ");

    if (buscarAlunoPorRA(ra) == NULL) {
        printf("Aluno nao encontrado.\n");
        aguardarEnter();
        return;
    }

    if (buscarTurmaPorID(idTurma) == NULL) {
        printf("Turma nao encontrada.\n");
        aguardarEnter();
        return;
    }

    if (associarAlunoTurma(ra, idTurma)) {
        printf("Aluno matriculado na turma.\n");
    } else {
        printf("Falha ao matricular aluno.\n");
    }

    aguardarEnter();
}

static void removerAlunoTurmaManual(void) {
    printf("\n=== Remover Aluno da Turma ===\n");
    int ra = lerInteiroObrigatorio("RA do aluno: ");
    int idTurma = lerInteiroObrigatorio("ID da turma: ");

    if (removerAlunoTurma(ra, idTurma)) {
        printf("Aluno removido da turma.\n");
    } else {
        printf("Falha ao remover aluno da turma.\n");
    }

    aguardarEnter();
}

static void listarAlunosDaTurmaManual(void) {
    int idTurma = lerInteiroObrigatorio("\nInforme o ID da turma: ");
    int ras[MAX_ALUNOS];
    int total = listarAlunosDaTurma(idTurma, ras, MAX_ALUNOS);

    printf("\n=== Alunos da Turma %d (%d registros) ===\n", idTurma, total);

    if (total <= 0) {
        printf("Nenhum aluno matriculado ou turma inexistente.\n");
        aguardarEnter();
        return;
    }

    for (int i = 0; i < total; i++) {
        Aluno *aluno = buscarAlunoPorRA(ras[i]);
        if (aluno != NULL) {
            printf("RA %d - %s (%s)\n", aluno->ra, aluno->nome, aluno->ativo ? "Ativo" : "Inativo");
        } else {
            printf("RA %d - [nao encontrado]\n", ras[i]);
        }
    }

    aguardarEnter();
}

static void listarTurmasDoAlunoManual(void) {
    int ra = lerInteiroObrigatorio("\nInforme o RA do aluno: ");
    int ids[MAX_TURMAS];
    int total = listarTurmasDoAluno(ra, ids, MAX_TURMAS);

    printf("\n=== Turmas do aluno %d (%d registros) ===\n", ra, total);

    if (total <= 0) {
        printf("Aluno sem turmas cadastradas ou nao encontrado.\n");
        aguardarEnter();
        return;
    }

    for (int i = 0; i < total; i++) {
        Turma *turma = buscarTurmaPorID(ids[i]);
        if (turma != NULL) {
            printf("Turma %d - %s (%d/%d)\n",
                   turma->id, turma->nome, turma->ano, turma->semestre);
        } else {
            printf("Turma %d - [nao encontrada]\n", ids[i]);
        }
    }

    aguardarEnter();
}

static void menuTurmas(void) {
    int opcao;

    do {
        printf("\n=== TURMA MANAGER ===\n");
        printf("1. Cadastrar turma\n");
        printf("2. Relatorio de turmas\n");
        printf("3. Alterar turma\n");
        printf("4. Excluir turma\n");
        printf("5. Matricular aluno em turma\n");
        printf("6. Remover aluno da turma\n");
        printf("7. Listar alunos da turma\n");
        printf("8. Listar turmas de um aluno\n");
        printf("0. Voltar ao menu principal\n");

        opcao = lerInteiroObrigatorio("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                cadastrarTurmaManual();
                break;
            case 2:
                listarTurmasManual();
                break;
            case 3:
                atualizarTurmaManual();
                break;
            case 4:
                excluirTurmaManual();
                break;
            case 5:
                associarAlunoTurmaManual();
                break;
            case 6:
                removerAlunoTurmaManual();
                break;
            case 7:
                listarAlunosDaTurmaManual();
                break;
            case 8:
                listarTurmasDoAlunoManual();
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}

static void registrarAulaManual(void) {
    Aula aula;
    char buffer[MAX_CONTEUDO];

    aula.id = gerarProximoIDAula();
    printf("\n=== Registro de Aula ===\n");
    printf("ID gerado automaticamente: %d\n", aula.id);

    aula.id_turma = lerInteiroObrigatorio("ID da turma: ");

    while (1) {
        lerStringObrigatoria("Data da aula (DD/MM/AAAA): ", aula.data, sizeof(aula.data));
        if (validarData(aula.data)) {
            break;
        }
        printf("Data invalida. Utilize o formato DD/MM/AAAA.\n");
    }

    lerStringObrigatoria("Conteudo ministrado: ", buffer, sizeof(buffer));
    strncpy(aula.conteudo, buffer, sizeof(aula.conteudo));
    aula.conteudo[sizeof(aula.conteudo) - 1] = '\0';

    if (registrarAula(&aula)) {
        printf("Aula registrada com sucesso!\n");
    } else {
        printf("Falha ao registrar aula.\n");
    }

    aguardarEnter();
}

static void listarTodasAulasManual(void) {
    Aula aulas[MAX_AULAS];
    int total = listarTodasAulas(aulas, MAX_AULAS);

    printf("\n=== Relatorio de Aulas (%d registradas) ===\n", total);

    if (total == 0) {
        printf("Nenhuma aula cadastrada.\n");
        aguardarEnter();
        return;
    }

    for (int i = 0; i < total; i++) {
        printf("ID: %d | Turma: %d | Data: %s\nConteudo: %s\n---\n",
               aulas[i].id, aulas[i].id_turma, aulas[i].data, aulas[i].conteudo);
    }

    aguardarEnter();
}

static void listarAulasPorTurmaManual(void) {
    int idTurma = lerInteiroObrigatorio("\nInforme o ID da turma: ");
    Aula aulas[MAX_AULAS];
    int total = listarAulasDaTurma(idTurma, aulas, MAX_AULAS);

    printf("\n=== Aulas da Turma %d (%d registros) ===\n", idTurma, total);

    if (total == 0) {
        printf("Nenhuma aula encontrada para a turma informada.\n");
        aguardarEnter();
        return;
    }

    for (int i = 0; i < total; i++) {
        printf("ID: %d | Data: %s\nConteudo: %s\n---\n",
               aulas[i].id, aulas[i].data, aulas[i].conteudo);
    }

    aguardarEnter();
}

static void atualizarAulaManual(void) {
    int id = lerInteiroObrigatorio("\nInforme o ID da aula: ");
    Aula *encontrada = buscarAulaPorID(id);

    if (encontrada == NULL) {
        printf("Aula nao encontrada.\n");
        aguardarEnter();
        return;
    }

    Aula atualizada = *encontrada;
    char buffer[MAX_CONTEUDO];

    char prompt[64];
    snprintf(prompt, sizeof(prompt), "ID da turma [%d]: ", atualizada.id_turma);
    atualizada.id_turma = lerInteiroOpcional(prompt, atualizada.id_turma);

    printf("Data atual: %s\n", atualizada.data);
    while (1) {
        printf("Nova data (ENTER para manter): ");
        if (!lerLinha(buffer, sizeof(buffer))) {
            continue;
        }
        if (buffer[0] == '\0') {
            break;
        }
        if (validarData(buffer)) {
            strncpy(atualizada.data, buffer, sizeof(atualizada.data));
            atualizada.data[sizeof(atualizada.data) - 1] = '\0';
            break;
        }
        printf("Data invalida. Utilize o formato DD/MM/AAAA.\n");
    }

    printf("Conteudo atual: %s\n", atualizada.conteudo);
    printf("Novo conteudo (ENTER para manter): ");
    if (lerLinha(buffer, sizeof(buffer)) && buffer[0] != '\0') {
        strncpy(atualizada.conteudo, buffer, sizeof(atualizada.conteudo));
        atualizada.conteudo[sizeof(atualizada.conteudo) - 1] = '\0';
    }

    if (atualizarAula(&atualizada)) {
        printf("Aula atualizada com sucesso!\n");
    } else {
        printf("Falha ao atualizar aula.\n");
    }

    aguardarEnter();
}

static void excluirAulaManual(void) {
    int id = lerInteiroObrigatorio("\nInforme o ID da aula para excluir: ");

    if (excluirAula(id)) {
        printf("Aula excluida com sucesso.\n");
    } else {
        printf("Falha ao excluir aula.\n");
    }

    aguardarEnter();
}

static void menuAulas(void) {
    int opcao;

    do {
        printf("\n=== AULA MANAGER ===\n");
        printf("1. Registrar aula\n");
        printf("2. Relatorio de todas as aulas\n");
        printf("3. Listar aulas por turma\n");
        printf("4. Alterar aula\n");
        printf("5. Excluir aula\n");
        printf("0. Voltar ao menu principal\n");

        opcao = lerInteiroObrigatorio("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                registrarAulaManual();
                break;
            case 2:
                listarTodasAulasManual();
                break;
            case 3:
                listarAulasPorTurmaManual();
                break;
            case 4:
                atualizarAulaManual();
                break;
            case 5:
                excluirAulaManual();
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}

static void cadastrarAtividadeManual(void) {
    Atividade atividade;
    char buffer[MAX_CONTEUDO];

    atividade.id = gerarProximoIDAtividade();
    printf("\n=== Cadastro de Atividade ===\n");
    printf("ID gerado automaticamente: %d\n", atividade.id);

    atividade.id_turma = lerInteiroObrigatorio("ID da turma: ");
    lerStringObrigatoria("Titulo: ", atividade.titulo, sizeof(atividade.titulo));
    lerStringObrigatoria("Descricao: ", buffer, sizeof(buffer));
    strncpy(atividade.descricao, buffer, sizeof(atividade.descricao));
    atividade.descricao[sizeof(atividade.descricao) - 1] = '\0';

    printf("Path do arquivo (ENTER para deixar vazio): ");
    if (!lerLinha(atividade.path_arquivo, sizeof(atividade.path_arquivo))) {
        atividade.path_arquivo[0] = '\0';
    }

    if (cadastrarAtividade(&atividade)) {
        printf("Atividade cadastrada com sucesso!\n");
    } else {
        printf("Falha ao cadastrar atividade.\n");
    }

    aguardarEnter();
}

static void listarAtividadesManual(void) {
    Atividade atividades[MAX_ATIVIDADES];
    int total = listarAtividades(atividades, MAX_ATIVIDADES);

    printf("\n=== Relatorio de Atividades (%d registradas) ===\n", total);

    if (total == 0) {
        printf("Nenhuma atividade cadastrada.\n");
        aguardarEnter();
        return;
    }

    for (int i = 0; i < total; i++) {
        printf("ID: %d | Turma: %d | Titulo: %s\nDescricao: %s\nArquivo: %s\n---\n",
               atividades[i].id,
               atividades[i].id_turma,
               atividades[i].titulo,
               atividades[i].descricao,
               atividades[i].path_arquivo[0] ? atividades[i].path_arquivo : "(vazio)");
    }

    aguardarEnter();
}

static void listarAtividadesPorTurmaManual(void) {
    int idTurma = lerInteiroObrigatorio("\nInforme o ID da turma: ");
    Atividade atividades[MAX_ATIVIDADES];
    int total = listarAtividadesDaTurma(idTurma, atividades, MAX_ATIVIDADES);

    printf("\n=== Atividades da Turma %d (%d registros) ===\n", idTurma, total);

    if (total == 0) {
        printf("Nenhuma atividade encontrada para a turma informada.\n");
        aguardarEnter();
        return;
    }

    for (int i = 0; i < total; i++) {
        printf("ID: %d | Titulo: %s\nDescricao: %s\nArquivo: %s\n---\n",
               atividades[i].id,
               atividades[i].titulo,
               atividades[i].descricao,
               atividades[i].path_arquivo[0] ? atividades[i].path_arquivo : "(vazio)");
    }

    aguardarEnter();
}

static void atualizarAtividadeManual(void) {
    int id = lerInteiroObrigatorio("\nInforme o ID da atividade: ");
    Atividade *encontrada = buscarAtividadePorID(id);

    if (encontrada == NULL) {
        printf("Atividade nao encontrada.\n");
        aguardarEnter();
        return;
    }

    Atividade atualizada = *encontrada;
    char buffer[MAX_CONTEUDO];
    char prompt[64];

    snprintf(prompt, sizeof(prompt), "ID da turma [%d]: ", atualizada.id_turma);
    atualizada.id_turma = lerInteiroOpcional(prompt, atualizada.id_turma);

    printf("Titulo atual: %s\n", atualizada.titulo);
    printf("Novo titulo (ENTER para manter): ");
    if (lerLinha(buffer, sizeof(buffer)) && buffer[0] != '\0') {
        strncpy(atualizada.titulo, buffer, sizeof(atualizada.titulo));
        atualizada.titulo[sizeof(atualizada.titulo) - 1] = '\0';
    }

    printf("Descricao atual: %s\n", atualizada.descricao);
    printf("Nova descricao (ENTER para manter): ");
    if (lerLinha(buffer, sizeof(buffer)) && buffer[0] != '\0') {
        strncpy(atualizada.descricao, buffer, sizeof(atualizada.descricao));
        atualizada.descricao[sizeof(atualizada.descricao) - 1] = '\0';
    }

    printf("Arquivo atual: %s\n", atualizada.path_arquivo[0] ? atualizada.path_arquivo : "(vazio)");
    printf("Novo path (ENTER para manter): ");
    if (lerLinha(buffer, sizeof(buffer)) && buffer[0] != '\0') {
        strncpy(atualizada.path_arquivo, buffer, sizeof(atualizada.path_arquivo));
        atualizada.path_arquivo[sizeof(atualizada.path_arquivo) - 1] = '\0';
    }

    if (atualizarAtividade(&atualizada)) {
        printf("Atividade atualizada com sucesso!\n");
    } else {
        printf("Falha ao atualizar atividade.\n");
    }

    aguardarEnter();
}

static void excluirAtividadeManual(void) {
    int id = lerInteiroObrigatorio("\nInforme o ID da atividade para excluir: ");

    if (excluirAtividade(id)) {
        printf("Atividade excluida com sucesso.\n");
    } else {
        printf("Falha ao excluir atividade.\n");
    }

    aguardarEnter();
}

static void menuAtividades(void) {
    int opcao;

    do {
        printf("\n=== ATIVIDADE MANAGER ===\n");
        printf("1. Cadastrar atividade\n");
        printf("2. Relatorio de todas as atividades\n");
        printf("3. Listar atividades por turma\n");
        printf("4. Alterar atividade\n");
        printf("5. Excluir atividade\n");
        printf("0. Voltar ao menu principal\n");

        opcao = lerInteiroObrigatorio("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                cadastrarAtividadeManual();
                break;
            case 2:
                listarAtividadesManual();
                break;
            case 3:
                listarAtividadesPorTurmaManual();
                break;
            case 4:
                atualizarAtividadeManual();
                break;
            case 5:
                excluirAtividadeManual();
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}

static int lerAtivoOpcional(int atual) {
    char prompt[64];
    snprintf(prompt, sizeof(prompt), "Ativo (1=Sim, 0=Nao) [%d]: ", atual);
    return lerInteiroOpcional(prompt, atual);
}

static void cadastrarUsuarioManual(void) {
    Usuario usuario;

    usuario.id = gerarProximoIDUsuario();
    printf("\n=== Cadastro de Usuario ===\n");
    printf("ID gerado automaticamente: %d\n", usuario.id);

    lerStringObrigatoria("Login: ", usuario.login, sizeof(usuario.login));
    lerStringObrigatoria("Senha: ", usuario.senha, sizeof(usuario.senha));
    lerStringObrigatoria("Tipo (ADMIN/PROFESSOR/ALUNO): ", usuario.tipo, sizeof(usuario.tipo));
    tornarMaiusculo(usuario.tipo);
    usuario.ativo = 1;

    if (cadastrarUsuario(&usuario)) {
        printf("Usuario cadastrado com sucesso!\n");
    } else {
        printf("Falha ao cadastrar usuario.\n");
    }

    aguardarEnter();
}

static void listarUsuariosManual(void) {
    Usuario usuarios[MAX_USUARIOS];
    int total = listarUsuarios(usuarios, MAX_USUARIOS);

    printf("\n=== Relatorio de Usuarios (%d encontrados) ===\n", total);

    if (total == 0) {
        printf("Nenhum usuario cadastrado.\n");
        aguardarEnter();
        return;
    }

    printf("%-6s %-15s %-12s %-6s\n", "ID", "Login", "Tipo", "Ativo");
    printf("---------------------------------------------\n");
    for (int i = 0; i < total; i++) {
        printf("%-6d %-15s %-12s %-6s\n",
               usuarios[i].id,
               usuarios[i].login,
               usuarios[i].tipo,
               usuarios[i].ativo ? "Sim" : "Nao");
    }

    aguardarEnter();
}

static void atualizarUsuarioManual(void) {
    int id = lerInteiroObrigatorio("\nInforme o ID do usuario: ");
    Usuario *encontrado = buscarUsuarioPorID(id);

    if (encontrado == NULL) {
        printf("Usuario nao encontrado.\n");
        aguardarEnter();
        return;
    }

    Usuario atualizado = *encontrado;
    char buffer[MAX_LOGIN];

    printf("Login atual: %s\n", atualizado.login);
    printf("Novo login (ENTER para manter): ");
    if (lerLinha(buffer, sizeof(buffer)) && buffer[0] != '\0') {
        strncpy(atualizado.login, buffer, sizeof(atualizado.login));
        atualizado.login[sizeof(atualizado.login) - 1] = '\0';
    }

    printf("Tipo atual: %s\n", atualizado.tipo);
    printf("Novo tipo (ADMIN/PROFESSOR/ALUNO) ou ENTER para manter: ");
    if (lerLinha(buffer, sizeof(buffer)) && buffer[0] != '\0') {
        tornarMaiusculo(buffer);
        strncpy(atualizado.tipo, buffer, sizeof(atualizado.tipo));
        atualizado.tipo[sizeof(atualizado.tipo) - 1] = '\0';
    }

    atualizado.ativo = lerAtivoOpcional(atualizado.ativo);

    if (atualizarUsuario(&atualizado)) {
        printf("Usuario atualizado com sucesso!\n");
    } else {
        printf("Falha ao atualizar usuario.\n");
    }

    aguardarEnter();
}

static void alterarSenhaManual(void) {
    int id = lerInteiroObrigatorio("\nInforme o ID do usuario: ");
    char senhaAntiga[MAX_SENHA];
    char senhaNova[MAX_SENHA];

    lerStringObrigatoria("Senha atual: ", senhaAntiga, sizeof(senhaAntiga));
    lerStringObrigatoria("Nova senha: ", senhaNova, sizeof(senhaNova));

    if (alterarSenha(id, senhaAntiga, senhaNova)) {
        printf("Senha alterada com sucesso!\n");
    } else {
        printf("Falha ao alterar senha.\n");
    }

    aguardarEnter();
}

static void resetarSenhaManual(void) {
    int id = lerInteiroObrigatorio("\nInforme o ID do usuario: ");
    char novaSenha[MAX_SENHA];

    lerStringObrigatoria("Nova senha: ", novaSenha, sizeof(novaSenha));

    if (resetarSenha(id, novaSenha)) {
        printf("Senha resetada com sucesso!\n");
    } else {
        printf("Falha ao resetar senha.\n");
    }

    aguardarEnter();
}

static void excluirUsuarioManual(void) {
    int id = lerInteiroObrigatorio("\nInforme o ID do usuario para desativar: ");

    if (excluirUsuario(id)) {
        printf("Usuario desativado com sucesso.\n");
    } else {
        printf("Falha ao desativar usuario.\n");
    }

    aguardarEnter();
}

static void menuUsuarios(void) {
    int opcao;

    do {
        printf("\n=== USUARIO MANAGER ===\n");
        printf("1. Cadastrar usuario\n");
        printf("2. Relatorio de usuarios\n");
        printf("3. Alterar usuario\n");
        printf("4. Alterar senha\n");
        printf("5. Resetar senha (admin)\n");
        printf("6. Desativar usuario\n");
        printf("0. Voltar ao menu principal\n");

        opcao = lerInteiroObrigatorio("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                cadastrarUsuarioManual();
                break;
            case 2:
                listarUsuariosManual();
                break;
            case 3:
                atualizarUsuarioManual();
                break;
            case 4:
                alterarSenhaManual();
                break;
            case 5:
                resetarSenhaManual();
                break;
            case 6:
                excluirUsuarioManual();
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}

int main(void) {
    int opcao;

    do {
        printf("\n============================================\n");
        printf("       SISTEMA ACADEMICO - MODO MANUAL       \n");
        printf("============================================\n");
        printf("1. Gerenciar alunos\n");
        printf("2. Gerenciar turmas\n");
        printf("3. Gerenciar aulas\n");
        printf("4. Gerenciar atividades\n");
        printf("5. Gerenciar usuarios\n");
        printf("0. Sair\n");

        opcao = lerInteiroObrigatorio("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                menuAlunos();
                break;
            case 2:
                menuTurmas();
                break;
            case 3:
                menuAulas();
                break;
            case 4:
                menuAtividades();
                break;
            case 5:
                menuUsuarios();
                break;
            case 0:
                printf("Encerrando o sistema. Ate logo!\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;
}
