/*
 * Arquivo: servidor.c
 * Descrição: Servidor TCP para o Sistema Acadêmico
 * Suporta múltiplos clientes simultâneos usando threads
 * Porta: 5000
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdarg.h>

// Includes dos managers
#include "structs.h"
#include "auth_manager.h"
#include "usuario_manager.h"
#include "aluno_manager.h"
#include "turma_manager.h"
#include "aula_manager.h"

// Configurações do servidor
#define PORTA 5000
#define MAX_CONEXOES 10
#define BUFFER_SIZE 4096

// Cores para o terminal
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// Estrutura para passar dados para thread
typedef struct {
    int socket_cliente;
    struct sockaddr_in endereco_cliente;
    int numero_conexao;
} DadosCliente;

// Variável global para contar conexões
static int contador_conexoes = 0;
static pthread_mutex_t mutex_contador = PTHREAD_MUTEX_INITIALIZER;

// ========== FUNÇÕES AUXILIARES ==========

// Função para enviar resposta ao cliente
void enviarResposta(int socket_cliente, const char *resposta) {
    send(socket_cliente, resposta, strlen(resposta), 0);
}

// Função utilitária para construir respostas dinamicamente
static int anexarFormato(char **destino, size_t *capacidade, size_t *tamanho_atual,
                         const char *formato, ...) {
    va_list args;
    while (1) {
        va_start(args, formato);
        int escrito = vsnprintf((*destino) + (*tamanho_atual),
                                (*capacidade) - (*tamanho_atual),
                                formato,
                                args);
        va_end(args);

        if (escrito < 0) {
            return 0;
        }

        if ((size_t)escrito >= (*capacidade) - (*tamanho_atual)) {
            size_t nova_capacidade = (*capacidade) * 2;
            size_t necessario = (*tamanho_atual) + (size_t)escrito + 1;
            if (nova_capacidade < necessario) {
                nova_capacidade = necessario;
            }

            char *novo_buffer = realloc(*destino, nova_capacidade);
            if (novo_buffer == NULL) {
                return 0;
            }

            *destino = novo_buffer;
            *capacidade = nova_capacidade;
            continue;
        }

        *tamanho_atual += (size_t)escrito;
        return 1;
    }
}

// Função para processar comando LOGIN
void processarLogin(int socket_cliente, char *parametros) {
    char login[MAX_LOGIN], senha[MAX_SENHA];
    
    // Parse: LOGIN:usuario,senha
    if (sscanf(parametros, "%[^,],%s", login, senha) != 2) {
        enviarResposta(socket_cliente, "ERRO:Formato inválido\n");
        return;
    }
    
    printf("%s[LOGIN]%s Tentativa de login: %s\n", YELLOW, RESET, login);
    
    Sessao sessao;
    if (autenticar(login, senha, &sessao)) {
        char resposta[256];
        sprintf(resposta, "OK:%s:%d:%s\n", sessao.tipo, sessao.id_usuario, sessao.login);
        enviarResposta(socket_cliente, resposta);
        printf("%s[LOGIN]%s Sucesso: %s (%s)\n", GREEN, RESET, login, sessao.tipo);
    } else {
        enviarResposta(socket_cliente, "ERRO:Login ou senha incorretos\n");
        printf("%s[LOGIN]%s Falha: %s\n", RED, RESET, login);
    }
}

// Função para processar comando LISTAR_ALUNOS
void processarListarAlunos(int socket_cliente) {
    printf("%s[LISTAR_ALUNOS]%s Processando...\n", CYAN, RESET);
    
    Aluno alunos[MAX_ALUNOS];
    int total = listarAlunos(alunos, MAX_ALUNOS);

    size_t capacidade = BUFFER_SIZE;
    size_t tamanho = 0;
    char *resposta = (char *)malloc(capacidade);

    if (resposta == NULL) {
        enviarResposta(socket_cliente, "ERRO:Falha de memória\n");
        printf("%s[LISTAR_ALUNOS]%s Falha por falta de memória\n", RED, RESET);
        return;
    }

    resposta[0] = '\0';

    if (!anexarFormato(&resposta, &capacidade, &tamanho, "OK:%d\n", total)) {
        enviarResposta(socket_cliente, "ERRO:Falha ao montar resposta\n");
        free(resposta);
        printf("%s[LISTAR_ALUNOS]%s Falha ao montar resposta\n", RED, RESET);
        return;
    }

    for (int i = 0; i < total; i++) {
        if (!anexarFormato(&resposta, &capacidade, &tamanho, "%d,%s,%s,%d\n",
                           alunos[i].ra,
                           alunos[i].nome,
                           alunos[i].email,
                           alunos[i].ativo)) {
            enviarResposta(socket_cliente, "ERRO:Falha ao montar resposta\n");
            free(resposta);
            printf("%s[LISTAR_ALUNOS]%s Falha ao montar resposta\n", RED, RESET);
            return;
        }
    }
    
    enviarResposta(socket_cliente, resposta);
    free(resposta);
    printf("%s[LISTAR_ALUNOS]%s Enviados %d alunos\n", GREEN, RESET, total);
}

// Função para processar comando CADASTRAR_ALUNO
void processarCadastrarAluno(int socket_cliente, char *parametros) {
    Aluno aluno;
    
    // Parse: CADASTRAR_ALUNO:ra,nome,email
    if (sscanf(parametros, "%d,%[^,],%[^,]", 
               &aluno.ra, aluno.nome, aluno.email) != 3) {
        enviarResposta(socket_cliente, "ERRO:Formato inválido\n");
        return;
    }
    
    aluno.ativo = 1;
    
    printf("%s[CADASTRAR_ALUNO]%s RA: %d, Nome: %s\n", 
           CYAN, RESET, aluno.ra, aluno.nome);
    
    if (cadastrarAluno(&aluno)) {
        enviarResposta(socket_cliente, "OK:Aluno cadastrado com sucesso\n");
        printf("%s[CADASTRAR_ALUNO]%s Sucesso\n", GREEN, RESET);
    } else {
        enviarResposta(socket_cliente, "ERRO:Falha ao cadastrar aluno\n");
        printf("%s[CADASTRAR_ALUNO]%s Falha\n", RED, RESET);
    }
}

// Função para processar comando BUSCAR_ALUNO
void processarBuscarAluno(int socket_cliente, char *parametros) {
    int ra = atoi(parametros);
    
    printf("%s[BUSCAR_ALUNO]%s RA: %d\n", CYAN, RESET, ra);
    
    Aluno *aluno = buscarAlunoPorRA(ra);
    
    if (aluno != NULL) {
        char resposta[512];
        sprintf(resposta, "OK:%d,%s,%s,%d\n", 
                aluno->ra, aluno->nome, aluno->email, aluno->ativo);
        enviarResposta(socket_cliente, resposta);
        printf("%s[BUSCAR_ALUNO]%s Encontrado: %s\n", GREEN, RESET, aluno->nome);
    } else {
        enviarResposta(socket_cliente, "ERRO:Aluno não encontrado\n");
        printf("%s[BUSCAR_ALUNO]%s Não encontrado\n", RED, RESET);
    }
}

// Função para processar comando LISTAR_TURMAS
void processarListarTurmas(int socket_cliente) {
    printf("%s[LISTAR_TURMAS]%s Processando...\n", CYAN, RESET);
    
    Turma turmas[MAX_TURMAS];
    int total = listarTurmas(turmas, MAX_TURMAS);

    size_t capacidade = BUFFER_SIZE;
    size_t tamanho = 0;
    char *resposta = (char *)malloc(capacidade);

    if (resposta == NULL) {
        enviarResposta(socket_cliente, "ERRO:Falha de memória\n");
        printf("%s[LISTAR_TURMAS]%s Falha por falta de memória\n", RED, RESET);
        return;
    }

    resposta[0] = '\0';

    if (!anexarFormato(&resposta, &capacidade, &tamanho, "OK:%d\n", total)) {
        enviarResposta(socket_cliente, "ERRO:Falha ao montar resposta\n");
        free(resposta);
        printf("%s[LISTAR_TURMAS]%s Falha ao montar resposta\n", RED, RESET);
        return;
    }

    for (int i = 0; i < total; i++) {
        if (!anexarFormato(&resposta, &capacidade, &tamanho, "%d,%s,%s,%d,%d\n",
                           turmas[i].id,
                           turmas[i].nome,
                           turmas[i].professor,
                           turmas[i].ano,
                           turmas[i].semestre)) {
            enviarResposta(socket_cliente, "ERRO:Falha ao montar resposta\n");
            free(resposta);
            printf("%s[LISTAR_TURMAS]%s Falha ao montar resposta\n", RED, RESET);
            return;
        }
    }
    
    enviarResposta(socket_cliente, resposta);
    free(resposta);
    printf("%s[LISTAR_TURMAS]%s Enviadas %d turmas\n", GREEN, RESET, total);
}

// Função para processar comando CADASTRAR_TURMA
void processarCadastrarTurma(int socket_cliente, char *parametros) {
    Turma turma;
    
    // Parse: CADASTRAR_TURMA:nome,professor,ano,semestre
    if (sscanf(parametros, "%[^,],%[^,],%d,%d", 
               turma.nome, turma.professor, &turma.ano, &turma.semestre) != 4) {
        enviarResposta(socket_cliente, "ERRO:Formato inválido\n");
        return;
    }
    
    turma.id = gerarProximoIDTurma();
    
    printf("%s[CADASTRAR_TURMA]%s Nome: %s, Professor: %s\n", 
           CYAN, RESET, turma.nome, turma.professor);
    
    if (cadastrarTurma(&turma)) {
        char resposta[128];
        sprintf(resposta, "OK:%d:Turma cadastrada com sucesso\n", turma.id);
        enviarResposta(socket_cliente, resposta);
        printf("%s[CADASTRAR_TURMA]%s Sucesso (ID: %d)\n", GREEN, RESET, turma.id);
    } else {
        enviarResposta(socket_cliente, "ERRO:Falha ao cadastrar turma\n");
        printf("%s[CADASTRAR_TURMA]%s Falha\n", RED, RESET);
    }
}

// Função para processar comando REGISTRAR_AULA
void processarRegistrarAula(int socket_cliente, char *parametros) {
    Aula aula;
    
    // Parse: REGISTRAR_AULA:id_turma,data,conteudo
    char buffer_temp[BUFFER_SIZE];
    strcpy(buffer_temp, parametros);
    
    char *token = strtok(buffer_temp, ",");
    if (token == NULL) {
        enviarResposta(socket_cliente, "ERRO:Formato inválido\n");
        return;
    }
    aula.id_turma = atoi(token);
    
    token = strtok(NULL, ",");
    if (token == NULL) {
        enviarResposta(socket_cliente, "ERRO:Formato inválido\n");
        return;
    }
    strcpy(aula.data, token);
    
    token = strtok(NULL, "");
    if (token == NULL) {
        enviarResposta(socket_cliente, "ERRO:Formato inválido\n");
        return;
    }
    strcpy(aula.conteudo, token);
    
    aula.id = gerarProximoIDAula();
    
    printf("%s[REGISTRAR_AULA]%s Turma: %d, Data: %s\n", 
           CYAN, RESET, aula.id_turma, aula.data);
    
    if (registrarAula(&aula)) {
        char resposta[128];
        sprintf(resposta, "OK:%d:Aula registrada com sucesso\n", aula.id);
        enviarResposta(socket_cliente, resposta);
        printf("%s[REGISTRAR_AULA]%s Sucesso (ID: %d)\n", GREEN, RESET, aula.id);
    } else {
        enviarResposta(socket_cliente, "ERRO:Falha ao registrar aula\n");
        printf("%s[REGISTRAR_AULA]%s Falha\n", RED, RESET);
    }
}

// Função para processar comando LISTAR_AULAS_TURMA
void processarListarAulasTurma(int socket_cliente, char *parametros) {
    int id_turma = atoi(parametros);
    
    printf("%s[LISTAR_AULAS_TURMA]%s ID Turma: %d\n", CYAN, RESET, id_turma);
    
    Aula aulas[MAX_AULAS];
    int total = listarAulasDaTurma(id_turma, aulas, MAX_AULAS);

    size_t capacidade = BUFFER_SIZE;
    size_t tamanho = 0;
    char *resposta = (char *)malloc(capacidade);

    if (resposta == NULL) {
        enviarResposta(socket_cliente, "ERRO:Falha de memória\n");
        printf("%s[LISTAR_AULAS_TURMA]%s Falha por falta de memória\n", RED, RESET);
        return;
    }

    resposta[0] = '\0';

    if (!anexarFormato(&resposta, &capacidade, &tamanho, "OK:%d\n", total)) {
        enviarResposta(socket_cliente, "ERRO:Falha ao montar resposta\n");
        free(resposta);
        printf("%s[LISTAR_AULAS_TURMA]%s Falha ao montar resposta\n", RED, RESET);
        return;
    }

    for (int i = 0; i < total; i++) {
        if (!anexarFormato(&resposta, &capacidade, &tamanho, "%d,%d,%s,%s\n",
                           aulas[i].id,
                           aulas[i].id_turma,
                           aulas[i].data,
                           aulas[i].conteudo)) {
            enviarResposta(socket_cliente, "ERRO:Falha ao montar resposta\n");
            free(resposta);
            printf("%s[LISTAR_AULAS_TURMA]%s Falha ao montar resposta\n", RED, RESET);
            return;
        }
    }
    
    enviarResposta(socket_cliente, resposta);
    free(resposta);
    printf("%s[LISTAR_AULAS_TURMA]%s Enviadas %d aulas\n", GREEN, RESET, total);
}

// Função para processar comando ASSOCIAR_ALUNO_TURMA
void processarAssociarAlunoTurma(int socket_cliente, char *parametros) {
    int ra, id_turma;
    
    // Parse: ASSOCIAR_ALUNO_TURMA:ra,id_turma
    if (sscanf(parametros, "%d,%d", &ra, &id_turma) != 2) {
        enviarResposta(socket_cliente, "ERRO:Formato inválido\n");
        return;
    }
    
    printf("%s[ASSOCIAR_ALUNO_TURMA]%s RA: %d, Turma: %d\n", 
           CYAN, RESET, ra, id_turma);
    
    if (associarAlunoTurma(ra, id_turma)) {
        enviarResposta(socket_cliente, "OK:Aluno associado à turma\n");
        printf("%s[ASSOCIAR_ALUNO_TURMA]%s Sucesso\n", GREEN, RESET);
    } else {
        enviarResposta(socket_cliente, "ERRO:Falha ao associar\n");
        printf("%s[ASSOCIAR_ALUNO_TURMA]%s Falha\n", RED, RESET);
    }
}

// ========== FUNÇÃO PRINCIPAL DE PROCESSAMENTO ==========

void processarComando(int socket_cliente, char *comando) {
    // Remove newline
    comando[strcspn(comando, "\r\n")] = 0;
    
    if (strlen(comando) == 0) {
        return;
    }
    
    printf("\n%s► COMANDO RECEBIDO:%s %s\n", MAGENTA, RESET, comando);
    
    // Separar comando e parâmetros
    char *dois_pontos = strchr(comando, ':');
    char *parametros = NULL;
    
    if (dois_pontos != NULL) {
        *dois_pontos = '\0';
        parametros = dois_pontos + 1;
    }
    
    // Processar comando
    if (strcmp(comando, "LOGIN") == 0) {
        processarLogin(socket_cliente, parametros);
    }
    else if (strcmp(comando, "LISTAR_ALUNOS") == 0) {
        processarListarAlunos(socket_cliente);
    }
    else if (strcmp(comando, "CADASTRAR_ALUNO") == 0) {
        processarCadastrarAluno(socket_cliente, parametros);
    }
    else if (strcmp(comando, "BUSCAR_ALUNO") == 0) {
        processarBuscarAluno(socket_cliente, parametros);
    }
    else if (strcmp(comando, "LISTAR_TURMAS") == 0) {
        processarListarTurmas(socket_cliente);
    }
    else if (strcmp(comando, "CADASTRAR_TURMA") == 0) {
        processarCadastrarTurma(socket_cliente, parametros);
    }
    else if (strcmp(comando, "REGISTRAR_AULA") == 0) {
        processarRegistrarAula(socket_cliente, parametros);
    }
    else if (strcmp(comando, "LISTAR_AULAS_TURMA") == 0) {
        processarListarAulasTurma(socket_cliente, parametros);
    }
    else if (strcmp(comando, "ASSOCIAR_ALUNO_TURMA") == 0) {
        processarAssociarAlunoTurma(socket_cliente, parametros);
    }
    else if (strcmp(comando, "PING") == 0) {
        enviarResposta(socket_cliente, "PONG\n");
        printf("%s[PING]%s Respondido\n", GREEN, RESET);
    }
    else if (strcmp(comando, "SAIR") == 0) {
        enviarResposta(socket_cliente, "OK:Desconectando\n");
        printf("%s[SAIR]%s Cliente solicitou desconexão\n", YELLOW, RESET);
    }
    else {
        enviarResposta(socket_cliente, "ERRO:Comando desconhecido\n");
        printf("%s[ERRO]%s Comando desconhecido: %s\n", RED, RESET, comando);
    }
}

// ========== FUNÇÃO DA THREAD DO CLIENTE ==========

void* threadCliente(void *arg) {
    DadosCliente *dados = (DadosCliente *)arg;
    int socket_cliente = dados->socket_cliente;
    int numero = dados->numero_conexao;
    char ip_cliente[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, &(dados->endereco_cliente.sin_addr), ip_cliente, INET_ADDRSTRLEN);
    
    printf("\n%s╔════════════════════════════════════════╗%s\n", GREEN, RESET);
    printf("%s║  CLIENTE #%d CONECTADO                  ║%s\n", GREEN, numero, RESET);
    printf("%s║  IP: %-32s ║%s\n", GREEN, ip_cliente, RESET);
    printf("%s╚════════════════════════════════════════╝%s\n\n", GREEN, RESET);
    
    // Mensagem de boas-vindas
    enviarResposta(socket_cliente, "OK:Conectado ao Sistema Acadêmico\n");
    
    // Loop de recebimento de comandos
    char buffer[BUFFER_SIZE];
    int bytes_recebidos;
    
    while ((bytes_recebidos = recv(socket_cliente, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_recebidos] = '\0';
        
        // Processar comando
        processarComando(socket_cliente, buffer);
        
        // Se recebeu SAIR, encerrar
        if (strncmp(buffer, "SAIR", 4) == 0) {
            break;
        }
    }
    
    printf("\n%s╔════════════════════════════════════════╗%s\n", YELLOW, RESET);
    printf("%s║  CLIENTE #%d DESCONECTADO               ║%s\n", YELLOW, numero, RESET);
    printf("%s╚════════════════════════════════════════╝%s\n\n", YELLOW, RESET);
    
    close(socket_cliente);
    free(dados);
    
    pthread_mutex_lock(&mutex_contador);
    contador_conexoes--;
    pthread_mutex_unlock(&mutex_contador);
    
    return NULL;
}

// ========== FUNÇÃO PRINCIPAL ==========

int main() {
    int socket_servidor, socket_cliente;
    struct sockaddr_in endereco_servidor, endereco_cliente;
    socklen_t tamanho_endereco;
    pthread_t thread_id;
    
    printf("\n");
    printf("%s╔══════════════════════════════════════════════╗%s\n", CYAN, RESET);
    printf("%s║                                              ║%s\n", CYAN, RESET);
    printf("%s║   SERVIDOR SISTEMA ACADÊMICO - PIM II        ║%s\n", CYAN, RESET);
    printf("%s║   Porta: %d                               ║%s\n", CYAN, PORTA, RESET);
    printf("%s║                                              ║%s\n", CYAN, RESET);
    printf("%s╚══════════════════════════════════════════════╝%s\n\n", CYAN, RESET);
    
    // Criar usuário admin padrão se não existir
    printf("%s[INIT]%s Verificando usuário admin...\n", YELLOW, RESET);
    criarAdminPadrao();
    
    // Criar socket
    socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_servidor < 0) {
        perror("Erro ao criar socket");
        exit(1);
    }
    
    // Configurar opções do socket (reutilizar endereço)
    int opcao = 1;
    setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &opcao, sizeof(opcao));
    
    // Configurar endereço do servidor
    memset(&endereco_servidor, 0, sizeof(endereco_servidor));
    endereco_servidor.sin_family = AF_INET;
    endereco_servidor.sin_addr.s_addr = INADDR_ANY;
    endereco_servidor.sin_port = htons(PORTA);
    
    // Bind
    if (bind(socket_servidor, (struct sockaddr *)&endereco_servidor, sizeof(endereco_servidor)) < 0) {
        perror("Erro no bind");
        close(socket_servidor);
        exit(1);
    }
    
    // Listen
    if (listen(socket_servidor, MAX_CONEXOES) < 0) {
        perror("Erro no listen");
        close(socket_servidor);
        exit(1);
    }
    
    printf("%s[SERVIDOR]%s Aguardando conexões...\n\n", GREEN, RESET);
    
    // Loop principal - aceitar conexões
    tamanho_endereco = sizeof(endereco_cliente);
    
    while (1) {
        socket_cliente = accept(socket_servidor, (struct sockaddr *)&endereco_cliente, &tamanho_endereco);
        
        if (socket_cliente < 0) {
            perror("Erro ao aceitar conexão");
            continue;
        }
        
        // Incrementar contador de conexões
        pthread_mutex_lock(&mutex_contador);
        int numero_conexao = ++contador_conexoes;
        pthread_mutex_unlock(&mutex_contador);
        
        // Criar dados para passar à thread
        DadosCliente *dados = (DadosCliente *)malloc(sizeof(DadosCliente));
        dados->socket_cliente = socket_cliente;
        dados->endereco_cliente = endereco_cliente;
        dados->numero_conexao = numero_conexao;
        
        // Criar thread para o cliente
        if (pthread_create(&thread_id, NULL, threadCliente, (void *)dados) != 0) {
            perror("Erro ao criar thread");
            close(socket_cliente);
            free(dados);
            continue;
        }
        
        // Detach da thread (não precisamos fazer join)
        pthread_detach(thread_id);
    }
    
    close(socket_servidor);
    return 0;
}
