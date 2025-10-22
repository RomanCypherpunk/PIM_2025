#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include "structs.h"
#include "usuario_manager.h"

// Estrutura para representar uma sessão de usuário
typedef struct {
    int id_usuario;
    char login[MAX_LOGIN];
    char tipo[20];
    int ativo;
    long timestamp_login;  // Timestamp do login
} Sessao;

// ========== FUNÇÕES DE AUTENTICAÇÃO ==========

// Função para autenticar um usuário (fazer login)
// Retorna: ponteiro para sessão criada ou NULL se falhar
// Parâmetros:
//   - login: nome de usuário
//   - senha: senha em texto plano
//   - sessao: ponteiro para armazenar dados da sessão
Sessao* autenticar(const char *login, const char *senha, Sessao *sessao);

// Função para verificar se a senha está correta
// Retorna: 1 se correta, 0 se incorreta
int verificarSenha(const char *login, const char *senha);

// Função para fazer logout
// Retorna: 1 se sucesso, 0 se erro
int logout(Sessao *sessao);

// ========== FUNÇÕES DE AUTORIZAÇÃO ==========

// Função para verificar se usuário tem permissão para uma ação
// Retorna: 1 se tem permissão, 0 se não tem
int temPermissao(Sessao *sessao, const char *acao);

// Função para verificar se é admin
// Retorna: 1 se é admin, 0 se não é
int isAdmin(Sessao *sessao);

// Função para verificar se é professor
// Retorna: 1 se é professor, 0 se não é
int isProfessor(Sessao *sessao);

// Função para verificar se é aluno
// Retorna: 1 se é aluno, 0 se não é
int isAluno(Sessao *sessao);

// ========== FUNÇÕES DE GESTÃO DE SESSÃO ==========

// Função para validar se a sessão está ativa
// Retorna: 1 se válida, 0 se inválida
int validarSessao(Sessao *sessao);

// Função para obter informações do usuário da sessão
// Retorna: ponteiro para usuário ou NULL se não encontrado
Usuario* getUsuarioSessao(Sessao *sessao);

// ========== FUNÇÕES DE LOG/AUDITORIA ==========

// Função para registrar tentativa de login
// Retorna: 1 se sucesso, 0 se erro
int registrarTentativaLogin(const char *login, int sucesso);

// Função para registrar ação do usuário
// Retorna: 1 se sucesso, 0 se erro
int registrarAcao(Sessao *sessao, const char *acao, const char *detalhes);

#endif