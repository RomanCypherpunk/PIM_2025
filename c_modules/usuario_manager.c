#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "usuario_manager.h"

// ========== ARRAYS GLOBAIS (EM MEMÓRIA) ==========
static Usuario usuarios[MAX_USUARIOS];
static int total_usuarios = 0;

// ========== FUNÇÕES AUXILIARES PRIVADAS ==========

// Carrega usuários do arquivo para memória
static void carregarUsuariosMemoria(void) {
    FILE *arquivo = fopen(ARQUIVO_USUARIOS, "r");
    if (arquivo == NULL) {
        printf("Aviso: arquivo %s não encontrado. Será criado ao salvar.\n", ARQUIVO_USUARIOS);
        total_usuarios = 0;
        return;
    }
    
    char linha[512];
    fgets(linha, sizeof(linha), arquivo); // Pular cabeçalho
    
    total_usuarios = 0;
    while (fgets(linha, sizeof(linha), arquivo) != NULL && total_usuarios < MAX_USUARIOS) {
        linha[strcspn(linha, "\n")] = 0; // Remove quebra de linha
        
        sscanf(linha, "%d,%[^,],%[^,],%[^,],%d",
               &usuarios[total_usuarios].id,
               usuarios[total_usuarios].login,
               usuarios[total_usuarios].senha_hash,
               usuarios[total_usuarios].tipo,
               &usuarios[total_usuarios].ativo);
        
        total_usuarios++;
    }
    
    fclose(arquivo);
}

// Salva usuários da memória para o arquivo
static void salvarUsuariosArquivo(void) {
    FILE *arquivo = fopen(ARQUIVO_USUARIOS, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo %s para escrita.\n", ARQUIVO_USUARIOS);
        return;
    }
    
    fprintf(arquivo, "ID,Login,SenhaHash,Tipo,Ativo\n");
    
    for (int i = 0; i < total_usuarios; i++) {
        fprintf(arquivo, "%d,%s,%s,%s,%d\n",
                usuarios[i].id,
                usuarios[i].login,
                usuarios[i].senha_hash,
                usuarios[i].tipo,
                usuarios[i].ativo);
    }
    
    fclose(arquivo);
    printf("Usuários salvos com sucesso em %s\n", ARQUIVO_USUARIOS);
}

// Hash simples de senha (para fins didáticos - NÃO usar em produção)
static void hashSenha(const char *senha, char *hash_destino) {
    unsigned long hash = 5381;
    int c;
    const char *str = senha;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    sprintf(hash_destino, "%lu", hash);
}

// ========== IMPLEMENTAÇÃO DAS FUNÇÕES PÚBLICAS ==========

// Cadastrar um novo usuário
int cadastrarUsuario(Usuario *usuario) {
    if (usuario == NULL) {
        printf("Erro: usuário inválido.\n");
        return 0;
    }
    
    // Validar login
    if (!validarLogin(usuario->login)) {
        printf("Erro: login inválido.\n");
        return 0;
    }
    
    carregarUsuariosMemoria();
    
    // Verificar se login já existe
    if (loginExiste(usuario->login)) {
        printf("Erro: login '%s' já está em uso.\n", usuario->login);
        return 0;
    }
    
    // Verificar limite
    if (total_usuarios >= MAX_USUARIOS) {
        printf("Erro: limite de usuários atingido.\n");
        return 0;
    }
    
    // Adicionar novo usuário
    usuarios[total_usuarios] = *usuario;
    total_usuarios++;
    salvarUsuariosArquivo();
    
    printf("Usuário '%s' cadastrado com sucesso!\n", usuario->login);
    return 1;
}

// Buscar usuário por ID
Usuario* buscarUsuarioPorID(int id) {
    carregarUsuariosMemoria();
    
    for (int i = 0; i < total_usuarios; i++) {
        if (usuarios[i].id == id) {
            return &usuarios[i];
        }
    }
    
    return NULL;
}

// Buscar usuário por login
Usuario* buscarUsuarioPorLogin(const char *login) {
    if (login == NULL) {
        return NULL;
    }
    
    carregarUsuariosMemoria();
    
    for (int i = 0; i < total_usuarios; i++) {
        if (strcmp(usuarios[i].login, login) == 0) {
            return &usuarios[i];
        }
    }
    
    return NULL;
}

// Listar todos os usuários
int listarUsuarios(Usuario *destino, int max) {
    carregarUsuariosMemoria();
    
    int count = (total_usuarios < max) ? total_usuarios : max;
    
    for (int i = 0; i < count; i++) {
        destino[i] = usuarios[i];
    }
    
    return count;
}

// Atualizar dados de um usuário
int atualizarUsuario(Usuario *usuario) {
    if (usuario == NULL) {
        return 0;
    }
    
    carregarUsuariosMemoria();
    
    for (int i = 0; i < total_usuarios; i++) {
        if (usuarios[i].id == usuario->id) {
            // Não permite alterar o login
            strcpy(usuarios[i].tipo, usuario->tipo);
            usuarios[i].ativo = usuario->ativo;
            // A senha não é alterada aqui (use alterarSenha)
            
            salvarUsuariosArquivo();
            printf("Usuário atualizado com sucesso!\n");
            return 1;
        }
    }
    
    printf("Erro: usuário não encontrado.\n");
    return 0;
}

// Excluir um usuário (desativar)
int excluirUsuario(int id) {
    carregarUsuariosMemoria();
    
    for (int i = 0; i < total_usuarios; i++) {
        if (usuarios[i].id == id) {
            usuarios[i].ativo = 0;
            salvarUsuariosArquivo();
            printf("Usuário desativado com sucesso!\n");
            return 1;
        }
    }
    
    printf("Erro: usuário não encontrado.\n");
    return 0;
}

// Alterar senha de um usuário
int alterarSenha(int id, const char *senha_antiga, const char *senha_nova) {
    if (senha_antiga == NULL || senha_nova == NULL) {
        return 0;
    }
    
    if (!validarSenha(senha_nova)) {
        printf("Erro: senha nova não atende aos requisitos.\n");
        return 0;
    }
    
    carregarUsuariosMemoria();
    
    // Gerar hash da senha antiga para verificação
    char hash_antiga[65];
    hashSenha(senha_antiga, hash_antiga);
    
    for (int i = 0; i < total_usuarios; i++) {
        if (usuarios[i].id == id) {
            // Verificar senha antiga
            if (strcmp(usuarios[i].senha_hash, hash_antiga) != 0) {
                printf("Erro: senha antiga incorreta.\n");
                return 0;
            }
            
            // Gerar hash da senha nova
            char hash_nova[65];
            hashSenha(senha_nova, hash_nova);
            strcpy(usuarios[i].senha_hash, hash_nova);
            
            salvarUsuariosArquivo();
            printf("Senha alterada com sucesso!\n");
            return 1;
        }
    }
    
    printf("Erro: usuário não encontrado.\n");
    return 0;
}

// Resetar senha (admin)
int resetarSenha(int id, const char *nova_senha) {
    if (nova_senha == NULL) {
        return 0;
    }
    
    if (!validarSenha(nova_senha)) {
        printf("Erro: senha não atende aos requisitos.\n");
        return 0;
    }
    
    carregarUsuariosMemoria();
    
    for (int i = 0; i < total_usuarios; i++) {
        if (usuarios[i].id == id) {
            // Gerar hash da senha nova
            char hash_nova[65];
            hashSenha(nova_senha, hash_nova);
            strcpy(usuarios[i].senha_hash, hash_nova);
            
            salvarUsuariosArquivo();
            printf("Senha resetada com sucesso!\n");
            return 1;
        }
    }
    
    printf("Erro: usuário não encontrado.\n");
    return 0;
}

// ========== FUNÇÕES DE VALIDAÇÃO ==========

// Validar formato de login
int validarLogin(const char *login) {
    if (login == NULL) {
        return 0;
    }
    
    int len = strlen(login);
    
    // Tamanho mínimo e máximo
    if (len < 3 || len >= MAX_LOGIN) {
        printf("Erro: login deve ter entre 3 e %d caracteres.\n", MAX_LOGIN - 1);
        return 0;
    }
    
    // Primeiro caractere deve ser letra
    if (!isalpha(login[0])) {
        printf("Erro: login deve começar com letra.\n");
        return 0;
    }
    
    // Apenas letras, números, ponto e underscore
    for (int i = 0; i < len; i++) {
        if (!isalnum(login[i]) && login[i] != '.' && login[i] != '_') {
            printf("Erro: login contém caracteres inválidos.\n");
            return 0;
        }
    }
    
    return 1;
}

// Validar força da senha
int validarSenha(const char *senha) {
    if (senha == NULL) {
        return 0;
    }
    
    int len = strlen(senha);
    
    // Tamanho mínimo
    if (len < 6) {
        printf("Erro: senha deve ter pelo menos 6 caracteres.\n");
        return 0;
    }
    
    if (len >= MAX_SENHA) {
        printf("Erro: senha muito longa (máximo %d caracteres).\n", MAX_SENHA - 1);
        return 0;
    }
    
    return 1;
}

// Verificar se login já existe
int loginExiste(const char *login) {
    if (login == NULL) {
        return 0;
    }
    
    carregarUsuariosMemoria();
    
    for (int i = 0; i < total_usuarios; i++) {
        if (strcmp(usuarios[i].login, login) == 0) {
            return 1;
        }
    }
    
    return 0;
}

// ========== FUNÇÕES AUXILIARES ==========

// Gerar próximo ID disponível de usuário
int gerarProximoIDUsuario(void) {
    carregarUsuariosMemoria();
    
    int maior_id = 0;
    
    for (int i = 0; i < total_usuarios; i++) {
        if (usuarios[i].id > maior_id) {
            maior_id = usuarios[i].id;
        }
    }
    
    return maior_id + 1;
}

// Converter tipo de usuário em string
const char* tipoUsuarioParaString(TipoUsuario tipo) {
    switch (tipo) {
        case TIPO_ADMIN:
            return "ADMIN";
        case TIPO_PROFESSOR:
            return "PROFESSOR";
        case TIPO_ALUNO:
            return "ALUNO";
        default:
            return "DESCONHECIDO";
    }
}

// Converter string em tipo de usuário
TipoUsuario stringParaTipoUsuario(const char *tipo_str) {
    if (strcmp(tipo_str, "ADMIN") == 0) {
        return TIPO_ADMIN;
    } else if (strcmp(tipo_str, "PROFESSOR") == 0) {
        return TIPO_PROFESSOR;
    } else if (strcmp(tipo_str, "ALUNO") == 0) {
        return TIPO_ALUNO;
    }
    
    return TIPO_ALUNO; // Padrão
}

// ========== FUNÇÕES DE INICIALIZAÇÃO ==========

// Criar usuário admin padrão (se não existir)
int criarAdminPadrao(void) {
    carregarUsuariosMemoria();
    
    // Verificar se já existe um admin
    if (loginExiste("admin")) {
        return 0; // Já existe
    }
    
    // Criar usuário admin
    Usuario admin;
    admin.id = gerarProximoIDUsuario();
    strcpy(admin.login, "admin");
    
    // Senha padrão: "admin123"
    char senha_padrao[] = "admin123";
    hashSenha(senha_padrao, admin.senha_hash);
    
    strcpy(admin.tipo, "ADMIN");
    admin.ativo = 1;
    
    // Adicionar à lista
    usuarios[total_usuarios] = admin;
    total_usuarios++;
    salvarUsuariosArquivo();
    
    printf("Usuário admin padrão criado!\n");
    printf("Login: admin\n");
    printf("Senha: admin123\n");
    printf("⚠️  IMPORTANTE: Altere a senha padrão!\n");
    
    return 1;
}