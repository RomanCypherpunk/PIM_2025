# Makefile para o Sistema Acadêmico - PIM II
# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
LDFLAGS =

# Diretórios
SRC_DIR = c_modules
DATA_DIR = data

# Arquivos fonte para teste
SOURCES_TEST = $(SRC_DIR)/file_manager.c \
               $(SRC_DIR)/aluno_manager.c \
               $(SRC_DIR)/turma_manager.c \
               $(SRC_DIR)/aula_manager.c \
               $(SRC_DIR)/usuario_manager.c \
               $(SRC_DIR)/auth_manager.c \
               $(SRC_DIR)/main_test.c

# Arquivos fonte para servidor
SOURCES_SERVER = $(SRC_DIR)/file_manager.c \
                 $(SRC_DIR)/aluno_manager.c \
                 $(SRC_DIR)/turma_manager.c \
                 $(SRC_DIR)/aula_manager.c \
                 $(SRC_DIR)/usuario_manager.c \
                 $(SRC_DIR)/auth_manager.c \
                 $(SRC_DIR)/servidor.c

# Arquivos objeto
OBJECTS_TEST = $(SOURCES_TEST:.c=.o)
OBJECTS_SERVER = $(SOURCES_SERVER:.c=.o)

# Executáveis
TARGET_TEST = sistema_teste
TARGET_SERVER = servidor

# Regra padrão (all)
all: $(TARGET)
	@echo "✅ Compilação concluída com sucesso!"
	@echo "Execute com: ./$(TARGET)"

# Regra para criar o executável
$(TARGET): $(OBJECTS)
	@echo "🔗 Linkando arquivos objeto..."
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Regra para compilar arquivos .c em .o
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "🔨 Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpar arquivos compilados
clean:
	@echo "🧹 Limpando arquivos objeto e executável..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "✅ Limpeza concluída!"

# Regra para limpar tudo (incluindo dados)
clean-all: clean
	@echo "🗑️  Limpando arquivos de dados..."
	rm -f $(DATA_DIR)/*.csv
	@echo "✅ Todos os arquivos removidos!"

# Regra para criar a pasta data (se não existir)
setup:
	@echo "📁 Criando estrutura de diretórios..."
	mkdir -p $(DATA_DIR)
	@echo "✅ Estrutura criada!"

# Regra para executar o programa
run: $(TARGET)
	@echo "🚀 Executando o sistema de testes..."
	@echo "=================================="
	./$(TARGET)

# Regra para recompilar tudo do zero
rebuild: clean all

# Regra para mostrar ajuda
help:
	@echo "Comandos disponíveis:"
	@echo "  make          - Compila o projeto"
	@echo "  make run      - Compila e executa"
	@echo "  make clean    - Remove arquivos objeto e executável"
	@echo "  make clean-all- Remove tudo (incluindo CSVs)"
	@echo "  make setup    - Cria a estrutura de pastas"
	@echo "  make rebuild  - Recompila do zero"
	@echo "  make help     - Mostra esta mensagem"

# Declara as regras que não são arquivos
.PHONY: all clean clean-all setup run rebuild help