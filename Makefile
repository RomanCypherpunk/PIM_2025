# Makefile for the PIM 2025 Academic System

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
LDFLAGS =

ifeq ($(OS),Windows_NT)
	EXE_EXT = .exe
	POWERSHELL = powershell -NoProfile -Command
else
	EXE_EXT =
endif

SRC_DIR = c_modules
DATA_DIR = data

COMMON_SOURCES = $(SRC_DIR)/file_manager.c \
                 $(SRC_DIR)/aluno_manager.c \
                 $(SRC_DIR)/turma_manager.c \
                 $(SRC_DIR)/aula_manager.c \
                 $(SRC_DIR)/atividade_manager.c \
                 $(SRC_DIR)/usuario_manager.c \
                 $(SRC_DIR)/auth_manager.c

SOURCES_TEST = $(COMMON_SOURCES) \
               $(SRC_DIR)/main_test.c

SOURCES_APP = $(COMMON_SOURCES) \
              $(SRC_DIR)/main.c

SOURCES_SERVER = $(COMMON_SOURCES) \
                 $(SRC_DIR)/servidor.c

TARGET_TEST = sistema_teste
TARGET_APP = sistema_cli
TARGET_SERVER = servidor

OBJECTS_TEST = $(SOURCES_TEST:.c=.o)
OBJECTS_APP = $(SOURCES_APP:.c=.o)
OBJECTS_SERVER = $(SOURCES_SERVER:.c=.o)

all: $(TARGET_TEST) $(TARGET_APP)
	@echo "Compilacao concluida com sucesso."
	@echo "Use 'make run' para os testes ou 'make run-cli' para o modo manual."

$(TARGET_TEST): $(OBJECTS_TEST)
	@echo "Ligando objetos (teste)..."
	$(CC) $(CFLAGS) $(OBJECTS_TEST) -o $(TARGET_TEST) $(LDFLAGS)

$(TARGET_APP): $(OBJECTS_APP)
	@echo "Ligando objetos (modo manual)..."
	$(CC) $(CFLAGS) $(OBJECTS_APP) -o $(TARGET_APP) $(LDFLAGS)

$(TARGET_SERVER): $(OBJECTS_SERVER)
	@echo "Ligando objetos (servidor)..."
	$(CC) $(CFLAGS) $(OBJECTS_SERVER) -o $(TARGET_SERVER) $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Limpando objetos e executaveis..."
ifeq ($(OS),Windows_NT)
	@$(POWERSHELL) "Get-ChildItem -LiteralPath '$(SRC_DIR)' -Filter '*.o' -ErrorAction SilentlyContinue | ForEach-Object { Remove-Item -LiteralPath $$_.FullName -Force }"
	@$(POWERSHELL) "$$files = @('$(TARGET_TEST)','$(TARGET_TEST)$(EXE_EXT)','$(TARGET_APP)','$(TARGET_APP)$(EXE_EXT)','$(TARGET_SERVER)','$(TARGET_SERVER)$(EXE_EXT)'); foreach ($$f in $$files) { if (Test-Path $$f) { Remove-Item -LiteralPath $$f -Force } }"
else
	@rm -f $(OBJECTS_TEST) $(OBJECTS_APP) $(OBJECTS_SERVER) \
	       $(TARGET_TEST)$(EXE_EXT) $(TARGET_APP)$(EXE_EXT) $(TARGET_SERVER)$(EXE_EXT)
endif
	@echo "Limpeza concluida."

clean-all: clean
	@echo "Removendo arquivos de dados..."
ifeq ($(OS),Windows_NT)
	@$(POWERSHELL) "Get-ChildItem -LiteralPath '$(DATA_DIR)' -Filter '*.csv' -ErrorAction SilentlyContinue | ForEach-Object { Remove-Item -LiteralPath $$_.FullName -Force }"
else
	@rm -f $(DATA_DIR)/*.csv
endif
	@echo "Arquivos de dados removidos."

setup:
	@echo "Criando estrutura de dados..."
ifeq ($(OS),Windows_NT)
	@$(POWERSHELL) "New-Item -ItemType Directory -Path '$(DATA_DIR)' -ErrorAction SilentlyContinue | Out-Null"
else
	@mkdir -p $(DATA_DIR)
endif
	@echo "Estrutura criada."

run: $(TARGET_TEST)
	@echo "Executando sistema de testes..."
	@echo "=================================="
	./$(TARGET_TEST)

run-cli: $(TARGET_APP)
	@echo "Executando modo manual (CLI)..."
	@echo "=================================="
	./$(TARGET_APP)

run-server: $(TARGET_SERVER)
	@echo "Executando o servidor..."
	@echo "=================================="
	./$(TARGET_SERVER)

rebuild: clean all

help:
	@echo "Comandos disponiveis:"
	@echo "  make           - Compila os alvos principais"
	@echo "  make run       - Compila e executa os testes automatizados"
	@echo "  make run-cli   - Compila e executa o modo manual"
	@echo "  make run-server- Compila e executa o servidor"
	@echo "  make clean     - Remove objetos e binarios"
	@echo "  make clean-all - Remove tambem os arquivos de dados"
	@echo "  make setup     - Garante que a pasta de dados existe"
	@echo "  make rebuild   - Recompila do zero"
	@echo "  make help      - Mostra esta mensagem"

.PHONY: all clean clean-all setup run run-cli run-server rebuild help
