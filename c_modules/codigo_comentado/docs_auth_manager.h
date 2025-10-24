#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include "structs.h"
#include "usuario_manager.h"

/*
 * Este cabeçalho resume os serviços de autenticação e autorização.
 * Escrevi comentários em primeira pessoa para guiar minha apresentação.
 */

/* Estrutura de sessão que mantenho em memória enquanto o usuário está logado. */
typedef struct {
    int id_usuario;
    char login[MAX_LOGIN];
    char tipo[20];
    int ativo;
    long timestamp_login; /* Eu guardo quando o login ocorreu para tratar timeout. */
} Sessao;

/* ===== Autenticação ===== */

/* Faço o login do usuário, valido senha e preencho a sessão. */
Sessao* autenticar(const char *login, const char *senha, Sessao *sessao);

/* Uso este helper para comparar a senha informada com o que está no cadastro. */
int verificarSenha(const char *login, const char *senha);

/* Encerro a sessão atual e registro a saída. */
int logout(Sessao *sessao);

/* ===== Autorização ===== */

/* Checo se a sessão tem permissão para executar determinada ação. */
int temPermissao(Sessao *sessao, const char *acao);

/* Atalho para saber se quem está logado é administrador. */
int isAdmin(Sessao *sessao);

/* Atalho para verificar se é professor. */
int isProfessor(Sessao *sessao);

/* Atalho para verificar se é aluno. */
int isAluno(Sessao *sessao);

/* ===== Gestão de sessão ===== */

/* Confirmo se a sessão ainda é válida (usuário ativo e dentro do tempo). */
int validarSessao(Sessao *sessao);

/* A partir de uma sessão válida, obtenho o registro completo do usuário. */
Usuario* getUsuarioSessao(Sessao *sessao);

/* ===== Logs e auditoria ===== */

/* Registro o resultado de cada tentativa de login. */
int registrarTentativaLogin(const char *login, int sucesso);

/* Guardo em arquivo as ações relevantes realizadas pelo usuário. */
int registrarAcao(Sessao *sessao, const char *acao, const char *detalhes);

#endif /* AUTH_MANAGER_H */

