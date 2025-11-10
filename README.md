# PIM 2025 - Sistema Academico

## Como iniciar rapidamente

1. **Compilar os módulos C**  
   ```powershell
   Set-Location "C:\Users\enzox\OneDrive\Area de Trabalho\PIM 3\PIM_2025\c_modules"
   mingw32-make all CC=gcc         # ou use apenas mingw32-make
   ```
   > Esse comando gera os executáveis `sistema_cli.exe` (modo manual) e `sistema_teste.exe` (fluxo automatizado), acessando apenas os arquivos CSV locais.

2. **Modo manual em C (CRUD interativo)**  
   ```powershell
   Set-Location "C:\Users\enzox\OneDrive\Area de Trabalho\PIM 3\PIM_2025\c_modules"
   mingw32-make run-cli CC=gcc     # ou apenas execute .\sistema_cli.exe se já compilou
   ```
   > O executável `sistema_cli` apresenta menus para criar, listar, alterar e remover registros de alunos, turmas, aulas, atividades e usuários diretamente nos CSVs da pasta `data`.

3. **Testes automatizados em C**  
   ```powershell
   Set-Location "C:\Users\enzox\OneDrive\Area de Trabalho\PIM 3\PIM_2025\c_modules"
   mingw32-make run CC=gcc         # roda o executável sistema_teste.exe
   ```

4. **Frontend Python**  
   ```powershell
   Set-Location "C:\Users\enzox\OneDrive\Area de Trabalho\PIM 3\PIM_2025"
   & .\venv\Scripts\Activate.ps1
   pip install -r frontend\requirements.txt
   python -m frontend.main
   ```
   > A interface Tkinter abre com tela de login (lembrando que os dados sao lidos de `data/usuarios.csv`).  
   > Depois de autenticado, o menu habilita apenas as abas permitidas para o perfil:
   > - **ADMIN**: acesso total aos cadastros (usuarios, alunos, turmas, aulas e atividades) e relatorios.
   > - **PROFESSOR**: pode consultar alunos, cadastrar/editar turmas, registrar aulas, enviar atividades e gerar relatorios.
   > - **ALUNO**: pode consultar turmas/aulas, visualizar atividades e abrir relatorios de notas.


## Cadastro de usuarios (sem hash)

As senhas agora vao em texto plano no arquivo `data/usuarios.csv`. Estrutura:

```
ID,Login,Senha,Tipo,Ativo
1,admin,admin123,ADMIN,1
```

Para adicionar alguem:

1. Feche o `sistema_cli.exe` se estiver em execução.
2. Abra `data/usuarios.csv` e acrescente uma linha:
   ```
   2,professor1,prof123,PROFESSOR,1
   ```
3. Salve o arquivo e reabra o `sistema_cli.exe` (ou o frontend) para que os dados sejam recarregados.
4. Faça login com o novo usuário.

Tipos validos: `ADMIN`, `PROFESSOR`, `ALUNO`. A coluna `Ativo` deve ser `1` para permitir login.

Pronto: sem hash, sem precisar recalcular nada.
