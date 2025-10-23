# PIM 2025 - Sistema Academico

## Como iniciar rapidamente

1. **Servidor C**  
   ```powershell
   Set-Location "C:\Users\enzox\OneDrive\Area de Trabalho\PIM 3\PIM_2025\c_modules"
   mingw32-make servidor CC=gcc     # ou use gcc diretamente, ver observacao abaixo
   .\servidor.exe
   ```
   > Se o `mingw32-make` acusar falta do `pthread.h`, instale o toolchain mingw-w64 com suporte a pthreads e compile com `gcc -pthread`.

2. **Modo manual em C (CRUD interativo)**  
   ```powershell
   Set-Location "C:\Users\enzox\OneDrive\Area de Trabalho\PIM 3\PIM_2025\c_modules"
   mingw32-make sistema_cli CC=gcc   # ou use mingw32-make run-cli
   .\sistema_cli.exe
   ```
   > O executavel `sistema_cli` apresenta menus para criar, listar, alterar e remover registros de alunos, turmas, aulas, atividades e usuarios.

3. **Frontend Python**  
   ```powershell
   Set-Location "C:\Users\enzox\OneDrive\Area de Trabalho\PIM 3\PIM_2025"
   & .\venv\Scripts\Activate.ps1
   pip install -r frontend\requirements.txt
   python -m frontend.main
   ```

## Cadastro de usuarios (sem hash)

As senhas agora vao em texto plano no arquivo `data/usuarios.csv`. Estrutura:

```
ID,Login,Senha,Tipo,Ativo
1,admin,admin123,ADMIN,1
```

Para adicionar alguem:

1. Pare o servidor se estiver rodando.
2. Abra `data/usuarios.csv` e acrescente uma linha:
   ```
   2,professor1,prof123,PROFESSOR,1
   ```
3. Salve o arquivo e reinicie o servidor (`.\servidor.exe`).
4. Faca login com o novo usuario no frontend.

Tipos validos: `ADMIN`, `PROFESSOR`, `ALUNO`. A coluna `Ativo` deve ser `1` para permitir login.

Pronto: sem hash, sem precisar recalcular nada.
