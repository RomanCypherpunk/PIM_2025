#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "auth_manager.h"

#define ARQUIVO_LOG_AUTH "data/auth_log.txt"
#define ARQUIVO_ACOES "data/acoes_log.txt"

// ========== FUNÇÕES AUXILIARES PRIVADAS ==========

// Hash simples de senha (mesma função do usuario_manager)
static void hashSenha(const char *senha, char *hash_destino) {
    unsigned long hash = 5381;
    int c;
    const char *str = senha;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    sprintf(hash_destino, "%lu", hash);
}

// Obter timestamp atual
static long getTimestamp(void) {
    return (long)time(NULL);
}

// ========== IMPLEMENTAÇÃO DAS FUNÇÕES DE AUTENTICAÇÃO ==========

// Autenticar um usuário
Sessao* autenticar(const char *login, const char *senha, Sessao *sessao) {
    if (login == NULL || senha == NULL || sessao == NULL) {
        printf("Erro: parâmetros inválidos para autenticação.\n");
        registrarTentativaLogin(login ? login : "NULL", 0);
        return NULL;
    }
    
    // Buscar usuário por login
    Usuario *usuario = buscarUsuarioPorLogin(login);
    
    if (usuario == NULL) {
        printf("Erro: usuário '%s' não encontrado.\n", login);
        registrarTentativaLogin(login, 0);
        return NULL;
    }
    
    // Verificar se está ativo
    if (!usuario->ativo) {
        printf("Erro: usuário '%s' está desativado.\n", login);
        registrarTentativaLogin(login, 0);
        return NULL;
    }
    
    // Verificar senha
    if (!verificarSenha(login, senha)) {
        printf("Erro: senha incorreta para usuário '%s'.\n", login);
        registrarTentativaLogin(login, 0);
        return NULL;
    }
    
    // Criar sessão
    sessao->id_usuario = usuario->id;
    strcpy(sessao->login, usuario->login);
    strcpy(sessao->tipo, usuario->tipo);
    sessao->ativo = 1;
    sessao->timestamp_login = getTimestamp();
    
    printf("✓ Login bem-sucedido!\n");
    printf("  Usuário: %s\n", sessao->login);
    printf("  Tipo: %s\n", sessao->tipo);
    
    registrarTentativaLogin(login, 1);
    registrarAcao(sessao, "LOGIN", "Login realizado com sucesso");
    
    return sessao;
}

// Verificar se a senha está correta
int verificarSenha(const char *login, const char *senha) {
    if (login == NULL || senha == NULL) {
        return 0;
    }
    
    Usuario *usuario = buscarUsuarioPorLogin(login);
    
    if (usuario == NULL) {
        return 0;
    }
    
    // Gerar hash da senha fornecida
    char hash_fornecido[65];
    hashSenha(senha, hash_fornecido);
    
    // Comparar com o hash armazenado
    return (strcmp(usuario->senha_hash, hash_fornecido) == 0);
}

// Fazer logout
int logout(Sessao *sessao) {
    if (sessao == NULL) {
        return 0;
    }
    
    registrarAcao(sessao, "LOGOUT", "Logout realizado");
    
    // Limpar dados da sessão
    sessao->id_usuario = 0;
    memset(sessao->login, 0, sizeof(sessao->login));
    memset(sessao->tipo, 0, sizeof(sessao->tipo));
    sessao->ativo = 0;
    sessao->timestamp_login = 0;
    
    printf("Logout realizado com sucesso.\n");
    return 1;
}

// ========== FUNÇÕES DE AUTORIZAÇÃO ==========

// Verificar se usuário tem permissão para uma ação
int temPermissao(Sessao *sessao, const char *acao) {
    if (sessao == NULL || acao == NULL || !sessao->ativo) {
        return 0;
    }
    
    // Admin tem permissão para tudo
    if (isAdmin(sessao)) {
        return 1;
    }
    
    // Permissões específicas por tipo de usuário
    if (isProfessor(sessao)) {
        // Professor pode:
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
        // Aluno pode:
        if (strcmp(acao, "CONSULTAR_TURMAS") == 0 ||
            strcmp(acao, "CONSULTAR_AULAS") == 0 ||
            strcmp(acao, "BAIXAR_ATIVIDADE") == 0 ||
            strcmp(acao, "VISUALIZAR_NOTAS") == 0) {
            return 1;
        }
    }
    
    return 0;
}

// Verificar se é admin
int isAdmin(Sessao *sessao) {
    if (sessao == NULL || !sessao->ativo) {
        return 0;
    }
    
    return (strcmp(sessao->tipo, "ADMIN") == 0);
}

// Verificar se é professor
int isProfessor(Sessao *sessao) {
    if (sessao == NULL || !sessao->ativo) {
        return 0;
    }
    
    return (strcmp(sessao->tipo, "PROFESSOR") == 0);
}

// Verificar se é aluno
int isAluno(Sessao *sessao) {
    if (sessao == NULL || !sessao->ativo) {
        return 0;
    }
    
    return (strcmp(sessao->tipo, "ALUNO") == 0);
}

// ========== FUNÇÕES DE GESTÃO DE SESSÃO ==========

// Validar se a sessão está ativa
int validarSessao(Sessao *sessao) {
    if (sessao == NULL) {
        return 0;
    }
    
    // Verificar se sessão está marcada como ativa
    if (!sessao->ativo) {
        return 0;
    }
    
    // Verificar se usuário ainda existe e está ativo
    Usuario *usuario = buscarUsuarioPorID(sessao->id_usuario);
    if (usuario == NULL || !usuario->ativo) {
        sessao->ativo = 0;
        return 0;
    }
    
    // Timeout de sessão: 8 horas (28800 segundos)
    long tempo_atual = getTimestamp();
    long tempo_sessao = tempo_atual - sessao->timestamp_login;
    
    if (tempo_sessao > 28800) {
        printf("Sessão expirada. Faça login novamente.\n");
        sessao->ativo = 0;
        return 0;
    }
    
    return 1;
}

// Obter informações do usuário da sessão
Usuario* getUsuarioSessao(Sessao *sessao) {
    if (sessao == NULL || !validarSessao(sessao)) {
        return NULL;
    }
    
    return buscarUsuarioPorID(sessao->id_usuario);
}

// ========== FUNÇÕES DE LOG/AUDITORIA ==========

// Registrar tentativa de login
int registrarTentativaLogin(const char *login, int sucesso) {
    FILE *arquivo = fopen(ARQUIVO_LOG_AUTH, "a");
    if (arquivo == NULL) {
        return 0;
    }
    
    time_t agora = time(NULL);
    char *timestamp = ctime(&agora);
    timestamp[strcspn(timestamp, "\n")] = 0; // Remove \n
    
    fprintf(arquivo, "[%s] LOGIN %s - Usuario: %s\n",
            timestamp,
            sucesso ? "SUCESSO" : "FALHA",
            login ? login : "NULL");
    
    fclose(arquivo);
    return 1;
}

// Registrar ação do usuário
int registrarAcao(Sessao *sessao, const char *acao, const char *detalhes) {
    if (sessao == NULL || acao == NULL) {
        return 0;
    }
    
    FILE *arquivo = fopen(ARQUIVO_ACOES, "a");
}