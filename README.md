# PIM 2025 - Sistema Academico

## Como iniciar rapidamente

1. **Compilar os módulos C**  
   ```powershell
   Entrar no terminal do arquivo raiz e executar o comando para compilar
   mingw32-make
   ```
   > Esse comando gera os executáveis `sistema_cli.exe` (modo manual) e `sistema_teste.exe` (fluxo automatizado), acessando apenas os arquivos CSV locais.

2. **Modo manual em C (CRUD interativo)**  
   ```powershell
   mingw32-make run-cli
   ```
   > O executável `sistema_cli` apresenta menus para criar, listar, alterar e remover registros de alunos, turmas, aulas, atividades e usuários diretamente nos CSVs da pasta `data`.

3. **Testes automatizados em C**  
   ```powershell
   mingw32-make run
   ```

4. **Frontend Python**  
   ```powershell
   na pasta front_end, executar o modulo main.py
   ```
   > A interface Tkinter abre com tela de login (lembrando que os dados sao lidos de `data/usuarios.csv`).  
   > Depois de autenticado, o menu habilita apenas as abas permitidas para o perfil:
   > - **ADMIN**: acesso total aos cadastros (usuarios, alunos, turmas, aulas e atividades) e relatorios.
   > - **PROFESSOR**: pode consultar alunos, cadastrar/editar turmas, registrar aulas, enviar atividades e gerar relatorios.
   > - **ALUNO**: pode consultar turmas/aulas, visualizar atividades e abrir relatorios de notas.