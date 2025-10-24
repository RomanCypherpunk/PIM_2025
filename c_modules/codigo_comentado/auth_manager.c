#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "auth_manager.h"

/*
 * Estou usando este arquivo para explicar o módulo de autenticação passo a passo.
 * Todos os comentários foram escritos em primeira pessoa para que eu possa narrar
 * o funcionamento como se estivesse em sala de aula.
 */

#define ARQUIVO_LOG_AUTH "data/auth_log.txt"
#define ARQUIVO_ACOES    "data/acoes_log.txt"

/* Eu calculo um timestamp básico (segundos desde 1970) sempre que preciso. */
static long getTimestamp(void) {
    return (long)time(NULL);
}

/*
 * Função principal de login:
 *  1. Valido parâmetros.
 *  2. Busco o usuário pelo login.
 *  3. Confiro se está ativo.
 *  4. Verifico a senha.
 *  5. Preencho a estrutura de sessão e registro logs.
 */
Sessao* autenticar(const char *login, const char *senha, Sessao *sessao) {
    if (login == NULL || senha == NULL || sessao == NULL) {
        printf("Erro: parâmetros inválidos para autenticação.\n");
        registrarTentativaLogin(login ? login : "NULL", 0);
        return NULL;
    }

    Usuario *usuario = buscarUsuarioPorLogin(login);
    if (usuario == NULL) {
        printf("Erro: usuário '%s' não encontrado.\n", login);
        registrarTentativaLogin(login, 0);
        return NULL;
    }

    if (!usuario->ativo) {
        printf("Erro: usuário '%s' está desativado.\n", login);
        registrarTentativaLogin(login, 0);
        return NULL;
    }

    if (!verificarSenha(login, senha)) {
        printf("Erro: senha incorreta para usuário '%s'.\n", login);
        registrarTentativaLogin(login, 0);
        return NULL;
    }

    /* Aqui eu monto a sessão em memória. */
    sessao->id_usuario = usuario->id;
    strcpy(sessao->login, usuario->login);
    strcpy(sessao->tipo, usuario->tipo);
    sessao->ativo = 1;
    sessao->timestamp_login = getTimestamp();

    printf("Login bem-sucedido!\n");
    printf("  Usuário: %s\n", sessao->login);
    printf("  Tipo: %s\n", sessao->tipo);

    registrarTentativaLogin(login, 1);
    registrarAcao(sessao, "LOGIN", "Login realizado com sucesso");

    return sessao;
}

/*
 * Verifico a senha comparando texto puro (sem hash nesta versão).
 * Devolvo 1 se a senha bate, 0 caso contrário.
 */
int verificarSenha(const char *login, const char *senha) {
    if (login == NULL || senha == NULL) {
        return 0;
    }

    Usuario *usuario = buscarUsuarioPorLogin(login);
    if (usuario == NULL) {
        return 0;
    }

    return (strcmp(usuario->senha, senha) == 0);
}

/*
 * Quando o usuário sai do sistema, registro a ação e limpo a estrutura de sessão.
 */
int logout(Sessao *sessao) {
    if (sessao == NULL) {
        return 0;
    }

    registrarAcao(sessao, "LOGOUT", "Logout realizado");

    sessao->id_usuario = 0;
    memset(sessao->login, 0, sizeof(sessao->login));
    memset(sessao->tipo, 0, sizeof(sessao->tipo));
    sessao->ativo = 0;
    sessao->timestamp_login = 0;

    printf("Logout realizado com sucesso.\n");
    return 1;
}

/*
 * Aqui defino regras de autorização por perfil.
 * Admin pode tudo, professor tem um conjunto e aluno outro.
 */
int temPermissao(Sessao *sessao, const char *acao) {
    if (sessao == NULL || acao == NULL || !sessao->ativo) {
        return 0;
    }

    if (isAdmin(sessao)) {
        return 1;
    }

    if (isProfessor(sessao)) {
        if (strcmp(acao, "CADASTRAR_TURMA") == 0 ||
            strcmp(acao, "EDITAR_TURMA") == 0 ||
            strcmp(acao, "REGISTRAR_AULA") == 0 ||
            strcmp(acao, "UPLOAD_ATIVIDADE") == 0 ||
            strcmp(acao, "VISUALIZAR_ALUNOS") == 0 ||
            strcmp(acao, "GERAR_RELATORIO") == 0) {
            return 1;
        }
    }

    if (isAluno(sessao)) {
        if (strcmp(acao, "CONSULTAR_TURMAS") == 0 ||
            strcmp(acao, "CONSULTAR_AULAS") == 0 ||
            strcmp(acao, "BAIXAR_ATIVIDADE") == 0 ||
            strcmp(acao, "VISUALIZAR_NOTAS") == 0) {
            return 1;
        }
    }

    return 0;
}

/* Pequenos atalhos para checar o tipo do usuário logado. */
int isAdmin(Sessao *sessao) {
    if (sessao == NULL || !sessao->ativo) {
        return 0;
    }
    return (strcmp(sessao->tipo, "ADMIN") == 0);
}

int isProfessor(Sessao *sessao) {
    if (sessao == NULL || !sessao->ativo) {
        return 0;
    }
    return (strcmp(sessao->tipo, "PROFESSOR") == 0);
}

int isAluno(Sessao *sessao) {
    if (sessao == NULL || !sessao->ativo) {
        return 0;
    }
    return (strcmp(sessao->tipo, "ALUNO") == 0);
}

/*
 * Valido a sessão verificando:
 *  - se está ativa,
 *  - se o usuário ainda existe e está ativo,
 *  - se não expirou (timeout de 8 horas).
 */
int validarSessao(Sessao *sessao) {
    if (sessao == NULL) {
        return 0;
    }

    if (!sessao->ativo) {
        return 0;
    }

    Usuario *usuario = buscarUsuarioPorID(sessao->id_usuario);
    if (usuario == NULL || !usuario->ativo) {
        sessao->ativo = 0;
        return 0;
    }

    long tempo_atual = getTimestamp();
    long tempo_sessao = tempo_atual - sessao->timestamp_login;
    if (tempo_sessao > 28800) { /* 8 horas */
        printf("Sessão expirada. Faça login novamente.\n");
        sessao->ativo = 0;
        return 0;
    }

    return 1;
}

/*
 * Se a sessão estiver válida, retorno o ponteiro do usuário correspondente.
 */
Usuario* getUsuarioSessao(Sessao *sessao) {
    if (sessao == NULL || !validarSessao(sessao)) {
        return NULL;
    }

    return buscarUsuarioPorID(sessao->id_usuario);
}

/*
 * Registro cada tentativa de login em um arquivo de log.
 * Anoto timestamp, se foi sucesso ou falha e o login informado.
 */
int registrarTentativaLogin(const char *login, int sucesso) {
    FILE *arquivo = fopen(ARQUIVO_LOG_AUTH, "a");
    if (arquivo == NULL) {
        return 0;
    }

    time_t agora = time(NULL);
    char *timestamp = ctime(&agora);
    timestamp[strcspn(timestamp, "\n")] = 0;

    fprintf(arquivo, "[%s] LOGIN %s - Usuario: %s\n",
            timestamp,
            sucesso ? "SUCESSO" : "FALHA",
            login ? login : "NULL");

    fclose(arquivo);
    return 1;
}

/*
 * Também mantenho um log de ações importantes (login/logout etc.).
 * Escrevo o tipo de usuário, a ação e um detalhe opcional.
 */
int registrarAcao(Sessao *sessao, const char *acao, const char *detalhes) {
    if (sessao == NULL || acao == NULL) {
        return 0;
    }

    FILE *arquivo = fopen(ARQUIVO_ACOES, "a");
    if (arquivo == NULL) {
        return 0;
    }

    time_t agora = time(NULL);
    char *timestamp = ctime(&agora);
    timestamp[strcspn(timestamp, "\n")] = 0;

    fprintf(arquivo, "[%s] Usuario: %s (%s) - Ação: %s - %s\n",
            timestamp,
            sessao->login,
            sessao->tipo,
            acao,
            detalhes ? detalhes : "");

    fclose(arquivo);
    return 1;
}

