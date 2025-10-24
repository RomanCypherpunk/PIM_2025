#ifndef USUARIO_MANAGER_H
#define USUARIO_MANAGER_H

#include "structs.h"

#define MAX_USUARIOS 500
#define ARQUIVO_USUARIOS "data/usuarios.csv"

// Enumeração para tipos de usuário
typedef enum {
    TIPO_ADMIN = 1,
    TIPO_PROFESSOR = 2,
    TIPO_ALUNO = 3
} TipoUsuario;

// ========== FUNÇÕES DE GERENCIAMENTO DE USUÁRIOS ==========

// Função para cadastrar um novo usuário
// Retorna: 1 se sucesso, 0 se erro
int cadastrarUsuario(Usuario *usuario);

// Função para buscar usuário por ID
// Retorna: ponteiro para o usuário ou NULL se não encontrado
Usuario* buscarUsuarioPorID(int id);

// Função para buscar usuário por login
// Retorna: ponteiro para o usuário ou NULL se não encontrado
Usuario* buscarUsuarioPorLogin(const char *login);

// Função para listar todos os usuários
// Retorna: número de usuários listados
int listarUsuarios(Usuario *destino, int max);

// Função para atualizar dados de um usuário
// Retorna: 1 se sucesso, 0 se erro
int atualizarUsuario(Usuario *usuario);

// Função para excluir um usuário (desativar)
// Retorna: 1 se sucesso, 0 se erro
int excluirUsuario(int id);

// Função para alterar senha de um usuário
// Retorna: 1 se sucesso, 0 se erro
int alterarSenha(int id, const char *senha_antiga, const char *senha_nova);

// Função para resetar senha (admin)
// Retorna: 1 se sucesso, 0 se erro
int resetarSenha(int id, const char *nova_senha);

// ========== FUNÇÕES DE VALIDAÇÃO ==========

// Função para validar formato de login
// Retorna: 1 se válido, 0 se inválido
int validarLogin(const char *login);

// Função para validar força da senha
// Retorna: 1 se válida, 0 se inválida
int validarSenha(const char *senha);

// Função para verificar se login já existe
// Retorna: 1 se existe, 0 se não existe
int loginExiste(const char *login);

// ========== FUNÇÕES AUXILIARES ==========

// Função para gerar próximo ID disponível de usuário
int gerarProximoIDUsuario(void);

// Função para converter tipo de usuário em string
const char* tipoUsuarioParaString(TipoUsuario tipo);

// Função para converter string em tipo de usuário
TipoUsuario stringParaTipoUsuario(const char *tipo_str);

// ========== FUNÇÕES DE INICIALIZAÇÃO ==========

// Função para criar usuário admin padrão (se não existir)
// Retorna: 1 se criou, 0 se já existe
int criarAdminPadrao(void);

#endif