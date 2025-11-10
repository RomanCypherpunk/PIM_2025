"""Interface Tkinter simples para o Sistema Academico PIM 2025."""

from __future__ import annotations

import csv
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, Dict, Iterable, List, Optional, Sequence, Tuple

import tkinter as tk
from tkinter import messagebox, ttk

BASE_DIR = Path(__file__).resolve().parents[1]
DATA_DIR = BASE_DIR / "data"

TABLE_HEADERS: Dict[str, List[str]] = {
    "usuarios.csv": ["ID", "Login", "Senha", "Tipo", "Ativo"],
    "alunos.csv": ["RA", "Nome", "Email", "Ativo"],
    "turmas.csv": ["ID", "Nome", "Professor", "Ano", "Semestre"],
    "aulas.csv": ["ID", "ID_Turma", "Data", "Conteudo"],
    "atividades.csv": ["ID", "ID_Turma", "Titulo", "Descricao", "Arquivo"],
    "aluno_turma.csv": ["RA", "ID_Turma"],
}

ROLE_ACTIONS: Dict[str, set[str]] = {
    "ADMIN": {"*"},
    "PROFESSOR": {
        "CADASTRAR_TURMA",
        "EDITAR_TURMA",
        "REGISTRAR_AULA",
        "UPLOAD_ATIVIDADE",
        "VISUALIZAR_ALUNOS",
        "GERAR_RELATORIO",
        "CONSULTAR_TURMAS",
        "CONSULTAR_AULAS",
        "BAIXAR_ATIVIDADE",
    },
    "ALUNO": {
        "CONSULTAR_TURMAS",
        "CONSULTAR_AULAS",
        "BAIXAR_ATIVIDADE",
        "VISUALIZAR_NOTAS",
    },
}

ACTION_LABELS = {
    "CADASTRAR_TURMA": "Cadastrar turmas",
    "EDITAR_TURMA": "Editar turmas",
    "REGISTRAR_AULA": "Registrar aulas",
    "UPLOAD_ATIVIDADE": "Cadastrar atividades",
    "VISUALIZAR_ALUNOS": "Listar alunos",
    "GERAR_RELATORIO": "Gerar relatorios",
    "CONSULTAR_TURMAS": "Consultar turmas",
    "CONSULTAR_AULAS": "Consultar aulas",
    "BAIXAR_ATIVIDADE": "Baixar atividades",
    "VISUALIZAR_NOTAS": "Visualizar notas/relatorios",
}


@dataclass
class Session:
    """Dados minimos da sessao autenticada."""

    user_id: int
    login: str
    tipo: str


@dataclass
class FormField:
    """Representa um campo exibido em formularios modais."""

    name: str
    label: str
    default: str = ""
    options: Optional[Sequence[str]] = None


def is_truthy(value: str) -> bool:
    return str(value).strip() in {"1", "True", "true", "SIM", "Sim"}


def truthy_flag(value: str) -> str:
    return "Sim" if is_truthy(value) else "Nao"


class DataRepository:
    """Opera leituras e escritas nos arquivos CSV do projeto."""

    def __init__(self, data_dir: Path) -> None:
        self.data_dir = data_dir

    def _path_for(self, filename: str) -> Path:
        return self.data_dir / filename

    def read_table(self, filename: str) -> Tuple[List[str], List[Dict[str, str]]]:
        path = self._path_for(filename)
        if not path.exists():
            headers = list(TABLE_HEADERS.get(filename, []))
            return headers, []

        with path.open("r", newline="", encoding="utf-8") as csvfile:
            reader = csv.DictReader(csvfile)
            rows = [dict(row) for row in reader]
            headers = list(reader.fieldnames or TABLE_HEADERS.get(filename, []))
            return headers, rows

    def write_table(
        self, filename: str, rows: List[Dict[str, str]], headers: Optional[Sequence[str]] = None
    ) -> None:
        path = self._path_for(filename)
        path.parent.mkdir(parents=True, exist_ok=True)
        header_order = list(headers or TABLE_HEADERS.get(filename, []))

        if not header_order and rows:
            header_order = list(rows[0].keys())

        if not header_order:
            raise ValueError(f"Cabecalho desconhecido para {filename}")

        with path.open("w", newline="", encoding="utf-8") as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=header_order)
            writer.writeheader()
            for row in rows:
                writer.writerow(row)

    @staticmethod
    def next_numeric_id(rows: Iterable[Dict[str, str]], field: str) -> int:
        max_value = 0
        for row in rows:
            try:
                value = int(str(row.get(field, "0")).strip())
                if value > max_value:
                    max_value = value
            except ValueError:
                continue
        return max_value + 1


class AuthService:
    """Valida credenciais consultando usuarios.csv."""

    def __init__(self, repo: DataRepository) -> None:
        self.repo = repo

    def authenticate(self, login: str, senha: str) -> Optional[Session]:
        login = login.strip()
        senha = senha.strip()
        if not login or not senha:
            return None

        _, usuarios = self.repo.read_table("usuarios.csv")
        for row in usuarios:
            if row.get("Login", "").lower() != login.lower():
                continue
            if not is_truthy(row.get("Ativo", "1")):
                continue
            if row.get("Senha", "") != senha:
                return None
            user_id = int(str(row.get("ID", "0")) or 0)
            tipo = row.get("Tipo", "ALUNO").upper()
            return Session(user_id=user_id, login=row.get("Login", ""), tipo=tipo)
        return None


class AcademicApp:
    """Coordenador principal da interface Tkinter."""

    def __init__(self, root: tk.Tk) -> None:
        self.root = root
        self.root.title("Sistema Academico PIM 2025")
        self.root.geometry("1024x640")

        self.repo = DataRepository(DATA_DIR)
        self.auth = AuthService(self.repo)
        self.session: Optional[Session] = None
        self.current_frame: Optional[tk.Frame] = None

        self._configure_style()
        self.show_login()

    def _configure_style(self) -> None:
        style = ttk.Style(self.root)
        if "clam" in style.theme_names():
            style.theme_use("clam")
        style.configure("Header.TLabel", font=("Segoe UI", 14, "bold"))
        style.configure("Small.TLabel", font=("Segoe UI", 9))

    def replace_frame(self, frame: tk.Frame) -> None:
        if self.current_frame is not None:
            self.current_frame.destroy()
        self.current_frame = frame
        self.current_frame.pack(fill="both", expand=True)

    def show_login(self) -> None:
        self.replace_frame(LoginFrame(self.root, self))

    def show_dashboard(self) -> None:
        if self.session is None:
            return
        self.replace_frame(DashboardFrame(self.root, self, self.session))

    def handle_login(self, login: str, senha: str) -> None:
        session = self.auth.authenticate(login, senha)
        if session is None:
            messagebox.showerror("Login invalido", "Verifique login, senha e se o usuario esta ativo.")
            return

        self.session = session
        self.show_dashboard()

    def logout(self) -> None:
        self.session = None
        self.show_login()

    def permissions_for(self, role: str) -> set[str]:
        if role.upper() == "ADMIN":
            return {"*"}
        return set(ROLE_ACTIONS.get(role.upper(), set()))


class LoginFrame(ttk.Frame):
    """Tela inicial para captura de login e senha."""

    def __init__(self, master: tk.Misc, app: AcademicApp) -> None:
        super().__init__(master, padding=40)
        self.app = app

        container = ttk.Frame(self, padding=20)
        container.pack(expand=True)

        ttk.Label(container, text="Sistema Academico", style="Header.TLabel").grid(
            row=0, column=0, columnspan=2, pady=(0, 20)
        )

        ttk.Label(container, text="Login").grid(row=1, column=0, sticky="e", padx=5, pady=5)
        ttk.Label(container, text="Senha").grid(row=2, column=0, sticky="e", padx=5, pady=5)

        self.login_var = tk.StringVar()
        self.senha_var = tk.StringVar()

        login_entry = ttk.Entry(container, textvariable=self.login_var, width=30)
        senha_entry = ttk.Entry(container, textvariable=self.senha_var, width=30, show="*")

        login_entry.grid(row=1, column=1, pady=5, sticky="w")
        senha_entry.grid(row=2, column=1, pady=5, sticky="w")

        login_entry.focus()
        senha_entry.bind("<Return>", lambda _event: self._submit())

        ttk.Button(container, text="Entrar", command=self._submit).grid(row=3, column=0, columnspan=2, pady=(15, 0))

    def _submit(self) -> None:
        self.app.handle_login(self.login_var.get(), self.senha_var.get())


class DashboardFrame(ttk.Frame):
    """Painel principal exibido apos autenticacao."""

    PAGE_BLUEPRINTS: Sequence[Tuple[str, str, Optional[Sequence[str]]]] = (
        ("Visao Geral", "_build_overview_tab", None),
        ("Usuarios", "_build_users_tab", ("ADMIN_ONLY",)),
        ("Alunos", "_build_students_tab", ("VISUALIZAR_ALUNOS",)),
        ("Turmas", "_build_classes_tab", ("CADASTRAR_TURMA", "EDITAR_TURMA", "CONSULTAR_TURMAS")),
        ("Aulas", "_build_lessons_tab", ("REGISTRAR_AULA", "CONSULTAR_AULAS")),
        ("Atividades", "_build_activities_tab", ("UPLOAD_ATIVIDADE", "BAIXAR_ATIVIDADE")),
        ("Relatorios", "_build_reports_tab", ("GERAR_RELATORIO", "VISUALIZAR_NOTAS")),
    )

    def __init__(self, master: tk.Misc, app: AcademicApp, session: Session) -> None:
        super().__init__(master, padding=10)
        self.app = app
        self.session = session
        self.permissions = self.app.permissions_for(self.session.tipo)

        header = ttk.Frame(self)
        header.pack(fill="x", pady=(0, 10))

        ttk.Label(header, text=f"Usuario: {self.session.login}", style="Header.TLabel").pack(side="left")
        ttk.Label(header, text=f"Perfil: {self.session.tipo}", padding=(10, 0)).pack(side="left")

        ttk.Button(header, text="Sair", command=self.app.logout).pack(side="right")

        self.notebook = ttk.Notebook(self)
        self.notebook.pack(fill="both", expand=True)

        self._build_tabs()

    def is_admin(self) -> bool:
        return self.session.tipo.upper() == "ADMIN" or "*" in self.permissions

    def has_action(self, action: str) -> bool:
        return self.is_admin() or action in self.permissions

    def _allow_page(self, required: Optional[Sequence[str]]) -> bool:
        if required is None:
            return True
        if "ADMIN_ONLY" in required:
            return self.is_admin()
        return any(self.has_action(action) for action in required)

    def _build_tabs(self) -> None:
        for title, builder_name, required in self.PAGE_BLUEPRINTS:
            if not self._allow_page(required):
                continue
            builder = getattr(self, builder_name)
            frame = builder()
            self.notebook.add(frame, text=title)

    # ----- Utilitarios de tabela -----
    def _create_table(self, parent: ttk.Frame, columns: Sequence[Tuple[str, str]], height: int = 12) -> ttk.Treeview:
        frame = ttk.Frame(parent)
        frame.pack(fill="both", expand=True)

        tree = ttk.Treeview(frame, columns=[col for col, _label in columns], show="headings", height=height)
        for column, label in columns:
            tree.heading(column, text=label)
            width = 100 if column.upper() in {"ID", "RA", "ANO"} else 200
            tree.column(column, width=width, anchor="w")

        y_scroll = ttk.Scrollbar(frame, orient="vertical", command=tree.yview)
        tree.configure(yscrollcommand=y_scroll.set)

        tree.grid(row=0, column=0, sticky="nsew")
        y_scroll.grid(row=0, column=1, sticky="ns")

        frame.grid_rowconfigure(0, weight=1)
        frame.grid_columnconfigure(0, weight=1)

        return tree

    def _refresh_tree(self, tree: ttk.Treeview, rows: List[Dict[str, str]], columns: Sequence[Tuple[str, str]]) -> None:
        for item in tree.get_children():
            tree.delete(item)
        for row in rows:
            values: List[str] = []
            for column, _label in columns:
                cell = row.get(column, "")
                if column == "Ativo":
                    cell = truthy_flag(cell)
                values.append(str(cell))
            tree.insert("", "end", values=values)

    # ----- Builders de abas -----
    def _build_overview_tab(self) -> ttk.Frame:
        frame = ttk.Frame(self.notebook, padding=15)
        ttk.Label(frame, text="Bem-vindo ao sistema academico", style="Header.TLabel").pack(anchor="w")
        ttk.Label(
            frame,
            text="Esta interface em Tkinter respeita as permissoes definidas para cada perfil.",
            padding=(0, 10),
        ).pack(anchor="w")

        info = ttk.LabelFrame(frame, text="Dados da sessao", padding=10)
        info.pack(fill="x", pady=10)
        ttk.Label(info, text=f"Login: {self.session.login}", style="Small.TLabel").pack(anchor="w")
        ttk.Label(info, text=f"Perfil: {self.session.tipo}", style="Small.TLabel").pack(anchor="w")

        perms = ttk.LabelFrame(frame, text="Permissoes liberadas", padding=10)
        perms.pack(fill="both", expand=True, pady=10)

        listbox = tk.Listbox(perms, height=10)
        listbox.pack(fill="both", expand=True)

        if self.is_admin():
            listbox.insert(tk.END, "Administrador - acesso total")
        else:
            allowed = sorted({ACTION_LABELS.get(action, action) for action in self.permissions})
            for label in allowed:
                listbox.insert(tk.END, label)
            if not allowed:
                listbox.insert(tk.END, "Nenhuma acao liberada.")

        return frame

    def _build_users_tab(self) -> ttk.Frame:
        frame = ttk.Frame(self.notebook, padding=15)
        columns = [("ID", "ID"), ("Login", "Login"), ("Tipo", "Tipo"), ("Ativo", "Ativo")]
        tree = self._create_table(frame, columns)
        self._refresh_users(tree, columns)

        btn_frame = ttk.Frame(frame)
        btn_frame.pack(fill="x", pady=10)

        ttk.Button(btn_frame, text="Adicionar usuario", command=lambda: self._add_user(tree, columns)).pack(
            side="left", padx=5
        )
        ttk.Button(btn_frame, text="Resetar senha", command=lambda: self._reset_user_password(tree, columns)).pack(
            side="left", padx=5
        )
        ttk.Button(btn_frame, text="Ativar/Desativar", command=lambda: self._toggle_user(tree, columns)).pack(
            side="left", padx=5
        )

        return frame

    def _build_students_tab(self) -> ttk.Frame:
        frame = ttk.Frame(self.notebook, padding=15)
        columns = [("RA", "RA"), ("Nome", "Nome"), ("Email", "Email"), ("Ativo", "Ativo")]
        tree = self._create_table(frame, columns)
        self._refresh_students(tree, columns)

        btn_frame = ttk.Frame(frame)
        btn_frame.pack(fill="x", pady=10)

        add_btn = ttk.Button(btn_frame, text="Adicionar aluno", command=lambda: self._add_student(tree, columns))
        toggle_btn = ttk.Button(
            btn_frame, text="Ativar/Desativar", command=lambda: self._toggle_student(tree, columns)
        )

        add_btn.pack(side="left", padx=5)
        toggle_btn.pack(side="left", padx=5)

        if not self.is_admin():
            add_btn.state(["disabled"])
            toggle_btn.state(["disabled"])

        ttk.Button(btn_frame, text="Atualizar", command=lambda: self._refresh_students(tree, columns)).pack(
            side="right"
        )

        return frame

    def _build_classes_tab(self) -> ttk.Frame:
        frame = ttk.Frame(self.notebook, padding=15)
        columns = [
            ("ID", "ID"),
            ("Nome", "Turma"),
            ("Professor", "Professor"),
            ("Ano", "Ano"),
            ("Semestre", "Semestre"),
        ]
        tree = self._create_table(frame, columns)
        self._refresh_classes(tree, columns)

        btn_frame = ttk.Frame(frame)
        btn_frame.pack(fill="x", pady=10)

        add_btn = ttk.Button(btn_frame, text="Cadastrar turma", command=lambda: self._add_class(tree, columns))
        edit_btn = ttk.Button(btn_frame, text="Editar selecionada", command=lambda: self._edit_class(tree, columns))

        add_btn.pack(side="left", padx=5)
        edit_btn.pack(side="left", padx=5)
        ttk.Button(btn_frame, text="Atualizar", command=lambda: self._refresh_classes(tree, columns)).pack(side="right")

        if not (self.is_admin() or self.has_action("CADASTRAR_TURMA")):
            add_btn.state(["disabled"])
        if not (self.is_admin() or self.has_action("EDITAR_TURMA")):
            edit_btn.state(["disabled"])

        return frame

    def _build_lessons_tab(self) -> ttk.Frame:
        frame = ttk.Frame(self.notebook, padding=15)
        columns = [("ID", "ID"), ("ID_Turma", "Turma"), ("Data", "Data"), ("Conteudo", "Conteudo")]
        tree = self._create_table(frame, columns)
        self._refresh_lessons(tree, columns)

        btn_frame = ttk.Frame(frame)
        btn_frame.pack(fill="x", pady=10)

        add_btn = ttk.Button(btn_frame, text="Registrar aula", command=lambda: self._add_lesson(tree, columns))
        add_btn.pack(side="left", padx=5)

        ttk.Button(btn_frame, text="Atualizar", command=lambda: self._refresh_lessons(tree, columns)).pack(side="right")

        if not (self.is_admin() or self.has_action("REGISTRAR_AULA")):
            add_btn.state(["disabled"])

        return frame

    def _build_activities_tab(self) -> ttk.Frame:
        frame = ttk.Frame(self.notebook, padding=15)
        columns = [
            ("ID", "ID"),
            ("ID_Turma", "Turma"),
            ("Titulo", "Titulo"),
            ("Descricao", "Descricao"),
            ("Arquivo", "Arquivo"),
        ]
        tree = self._create_table(frame, columns)
        self._refresh_activities(tree, columns)

        btn_frame = ttk.Frame(frame)
        btn_frame.pack(fill="x", pady=10)

        add_btn = ttk.Button(btn_frame, text="Cadastrar atividade", command=lambda: self._add_activity(tree, columns))
        show_btn = ttk.Button(btn_frame, text="Mostrar arquivo", command=lambda: self._show_activity_file(tree))

        add_btn.pack(side="left", padx=5)
        show_btn.pack(side="left", padx=5)
        ttk.Button(btn_frame, text="Atualizar", command=lambda: self._refresh_activities(tree, columns)).pack(
            side="right"
        )

        if not (self.is_admin() or self.has_action("UPLOAD_ATIVIDADE")):
            add_btn.state(["disabled"])

        if not (self.is_admin() or self.has_action("BAIXAR_ATIVIDADE")):
            show_btn.state(["disabled"])

        return frame

    def _build_reports_tab(self) -> ttk.Frame:
        frame = ttk.Frame(self.notebook, padding=15)

        summary_frame = ttk.LabelFrame(frame, text="Resumo rapido", padding=10)
        summary_frame.pack(fill="x", pady=10)

        summary_text = tk.Text(summary_frame, height=8, wrap="word")
        summary_text.pack(fill="both", expand=True)
        summary_text.configure(state="disabled")

        def generate_summary() -> None:
            lines = self._generate_summary_lines()
            summary_text.configure(state="normal")
            summary_text.delete("1.0", tk.END)
            summary_text.insert(tk.END, "\n".join(lines))
            summary_text.configure(state="disabled")

        summary_btn = ttk.Button(summary_frame, text="Gerar relatorio", command=generate_summary)
        summary_btn.pack(anchor="e", pady=(8, 0))

        if not (self.is_admin() or self.has_action("GERAR_RELATORIO")):
            summary_btn.state(["disabled"])

        notes_frame = ttk.LabelFrame(frame, text="Relatorios salvos (notas)", padding=10)
        notes_frame.pack(fill="both", expand=True, pady=10)

        ttk.Label(notes_frame, text="Selecione um arquivo de relatorio da pasta data/").pack(anchor="w")
        report_files = sorted(DATA_DIR.glob("relatorio_turma_*.txt"))
        options = [file.name for file in report_files]

        selected_var = tk.StringVar(value=options[0] if options else "")
        combo = ttk.Combobox(notes_frame, textvariable=selected_var, values=options, state="readonly", width=30)
        combo.pack(anchor="w", pady=5)

        notes_text = tk.Text(notes_frame, height=12, wrap="word")
        notes_text.pack(fill="both", expand=True)
        notes_text.configure(state="disabled")

        def load_report() -> None:
            target = selected_var.get()
            if not target:
                messagebox.showinfo("Relatorios", "Nenhum arquivo encontrado em data/relatorio_turma_*.txt.")
                return
            path = DATA_DIR / target
            if not path.exists():
                messagebox.showerror("Relatorios", f"Arquivo {target} nao encontrado.")
                return
            notes_text.configure(state="normal")
            notes_text.delete("1.0", tk.END)
            notes_text.insert(tk.END, path.read_text(encoding="utf-8"))
            notes_text.configure(state="disabled")

        ttk.Button(notes_frame, text="Abrir relatorio", command=load_report).pack(anchor="e", pady=(5, 0))

        if not (self.is_admin() or self.has_action("VISUALIZAR_NOTAS")):
            combo.state(["disabled"])

        return frame

    # ----- Operacoes especificas -----
    def _get_selected_row(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> Optional[Dict[str, str]]:
        selection = tree.selection()
        if not selection:
            messagebox.showwarning("Selecao vazia", "Selecione uma linha primeiro.")
            return None
        values = tree.item(selection[0], "values")
        row: Dict[str, str] = {}
        for index, (column, _label) in enumerate(columns):
            row[column] = str(values[index])
        return row

    def _refresh_users(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> None:
        _, rows = self.app.repo.read_table("usuarios.csv")
        self._refresh_tree(tree, rows, columns)

    def _refresh_students(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> None:
        _, rows = self.app.repo.read_table("alunos.csv")
        self._refresh_tree(tree, rows, columns)

    def _refresh_classes(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> None:
        _, rows = self.app.repo.read_table("turmas.csv")
        self._refresh_tree(tree, rows, columns)

    def _refresh_lessons(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> None:
        _, rows = self.app.repo.read_table("aulas.csv")
        self._refresh_tree(tree, rows, columns)

    def _refresh_activities(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> None:
        _, rows = self.app.repo.read_table("atividades.csv")
        self._refresh_tree(tree, rows, columns)

    def _add_user(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> None:
        fields = [
            FormField("login", "Login"),
            FormField("senha", "Senha"),
            FormField("tipo", "Tipo", default="ADMIN", options=["ADMIN", "PROFESSOR", "ALUNO"]),
        ]

        def submit(values: Dict[str, str]) -> bool:
            login = values["login"].strip()
            senha = values["senha"].strip()
            tipo = values["tipo"].strip().upper()
            if not login or not senha:
                messagebox.showerror("Usuarios", "Login e senha sao obrigatorios.")
                return False
            if tipo not in {"ADMIN", "PROFESSOR", "ALUNO"}:
                messagebox.showerror("Usuarios", "Tipo invalido.")
                return False

            headers, rows = self.app.repo.read_table("usuarios.csv")
            for row in rows:
                if row.get("Login", "").lower() == login.lower():
                    messagebox.showerror("Usuarios", "Login ja cadastrado.")
                    return False

            new_id = DataRepository.next_numeric_id(rows, "ID")
            rows.append({"ID": str(new_id), "Login": login, "Senha": senha, "Tipo": tipo, "Ativo": "1"})
            self.app.repo.write_table("usuarios.csv", rows, headers)
            self._refresh_users(tree, columns)
            return True

        open_form_dialog(self, "Novo usuario", fields, submit)

    def _reset_user_password(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> None:
        selected = self._get_selected_row(tree, columns)
        if selected is None:
            return

        fields = [FormField("senha", f"Nova senha para {selected.get('Login', '')}")]

        def submit(values: Dict[str, str]) -> bool:
            nova_senha = values["senha"].strip()
            if not nova_senha:
                messagebox.showerror("Usuarios", "Senha nao pode ser vazia.")
                return False
            headers, rows = self.app.repo.read_table("usuarios.csv")
            for row in rows:
                if row.get("Login") == selected.get("Login"):
                    row["Senha"] = nova_senha
                    self.app.repo.write_table("usuarios.csv", rows, headers)
                    self._refresh_users(tree, columns)
                    messagebox.showinfo("Usuarios", "Senha atualizada com sucesso.")
                    return True
            messagebox.showerror("Usuarios", "Usuario nao encontrado.")
            return False

        open_form_dialog(self, "Resetar senha", fields, submit)

    def _toggle_user(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> None:
        selected = self._get_selected_row(tree, columns)
        if selected is None:
            return

        headers, rows = self.app.repo.read_table("usuarios.csv")
        for row in rows:
            if row.get("Login") == selected.get("Login"):
                row["Ativo"] = "0" if is_truthy(row.get("Ativo", "1")) else "1"
                self.app.repo.write_table("usuarios.csv", rows, headers)
                self._refresh_users(tree, columns)
                return
        messagebox.showerror("Usuarios", "Usuario nao encontrado.")

    def _add_student(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> None:
        fields = [
            FormField("ra", "RA (deixe vazio para gerar)"),
            FormField("nome", "Nome completo"),
            FormField("email", "Email institucional"),
            FormField("ativo", "Ativo (1=Sim, 0=Nao)", default="1"),
        ]

        def submit(values: Dict[str, str]) -> bool:
            headers, rows = self.app.repo.read_table("alunos.csv")
            ra = values["ra"].strip()
            nome = values["nome"].strip()
            email = values["email"].strip()
            ativo = "1" if is_truthy(values.get("ativo", "1")) else "0"

            if not nome or not email:
                messagebox.showerror("Alunos", "Nome e email sao obrigatorios.")
                return False

            if not ra:
                ra = str(DataRepository.next_numeric_id(rows, "RA"))
            if not ra.isdigit():
                messagebox.showerror("Alunos", "RA deve ser numerico.")
                return False

            for row in rows:
                if row.get("RA") == ra:
                    messagebox.showerror("Alunos", "RA ja existente.")
                    return False

            rows.append({"RA": ra, "Nome": nome, "Email": email, "Ativo": ativo})
            self.app.repo.write_table("alunos.csv", rows, headers)
            self._refresh_students(tree, columns)
            return True

        open_form_dialog(self, "Novo aluno", fields, submit)

    def _toggle_student(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> None:
        selected = self._get_selected_row(tree, columns)
        if selected is None:
            return

        headers, rows = self.app.repo.read_table("alunos.csv")
        for row in rows:
            if row.get("RA") == selected.get("RA"):
                row["Ativo"] = "0" if is_truthy(row.get("Ativo", "1")) else "1"
                self.app.repo.write_table("alunos.csv", rows, headers)
                self._refresh_students(tree, columns)
                return
        messagebox.showerror("Alunos", "Aluno nao encontrado.")

    def _add_class(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> None:
        fields = [
            FormField("nome", "Nome da turma"),
            FormField("professor", "Professor responsavel"),
            FormField("ano", "Ano", default="2025"),
            FormField("semestre", "Semestre", default="1"),
        ]

        def submit(values: Dict[str, str]) -> bool:
            headers, rows = self.app.repo.read_table("turmas.csv")
            nome = values["nome"].strip()
            professor = values["professor"].strip()
            ano = values["ano"].strip()
            semestre = values["semestre"].strip()

            if not nome or not professor:
                messagebox.showerror("Turmas", "Nome e professor sao obrigatorios.")
                return False

            new_id = DataRepository.next_numeric_id(rows, "ID")
            rows.append(
                {"ID": str(new_id), "Nome": nome, "Professor": professor, "Ano": ano or "0", "Semestre": semestre or "0"}
            )
            self.app.repo.write_table("turmas.csv", rows, headers)
            self._refresh_classes(tree, columns)
            return True

        open_form_dialog(self, "Cadastrar turma", fields, submit)

    def _edit_class(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> None:
        selected = self._get_selected_row(tree, columns)
        if selected is None:
            return

        fields = [
            FormField("nome", "Nome da turma", default=selected.get("Nome", "")),
            FormField("professor", "Professor responsavel", default=selected.get("Professor", "")),
            FormField("ano", "Ano", default=selected.get("Ano", "")),
            FormField("semestre", "Semestre", default=selected.get("Semestre", "")),
        ]

        def submit(values: Dict[str, str]) -> bool:
            headers, rows = self.app.repo.read_table("turmas.csv")
            for row in rows:
                if row.get("ID") == selected.get("ID"):
                    row.update(
                        {
                            "Nome": values["nome"].strip(),
                            "Professor": values["professor"].strip(),
                            "Ano": values["ano"].strip(),
                            "Semestre": values["semestre"].strip(),
                        }
                    )
                    self.app.repo.write_table("turmas.csv", rows, headers)
                    self._refresh_classes(tree, columns)
                    return True
            messagebox.showerror("Turmas", "Turma nao encontrada.")
            return False

        open_form_dialog(self, "Editar turma", fields, submit)

    def _add_lesson(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> None:
        fields = [
            FormField("id_turma", "ID da turma"),
            FormField("data", "Data (dd/mm/aaaa)"),
            FormField("conteudo", "Conteudo abordado"),
        ]

        def submit(values: Dict[str, str]) -> bool:
            headers, rows = self.app.repo.read_table("aulas.csv")
            id_turma = values["id_turma"].strip()
            data = values["data"].strip()
            conteudo = values["conteudo"].strip()

            if not id_turma.isdigit():
                messagebox.showerror("Aulas", "ID da turma deve ser numerico.")
                return False
            if not data or not conteudo:
                messagebox.showerror("Aulas", "Data e conteudo sao obrigatorios.")
                return False

            new_id = DataRepository.next_numeric_id(rows, "ID")
            rows.append({"ID": str(new_id), "ID_Turma": id_turma, "Data": data, "Conteudo": conteudo})
            self.app.repo.write_table("aulas.csv", rows, headers)
            self._refresh_lessons(tree, columns)
            return True

        open_form_dialog(self, "Registrar aula", fields, submit)

    def _add_activity(self, tree: ttk.Treeview, columns: Sequence[Tuple[str, str]]) -> None:
        fields = [
            FormField("id_turma", "ID da turma"),
            FormField("titulo", "Titulo da atividade"),
            FormField("descricao", "Descricao"),
            FormField("arquivo", "Caminho do arquivo", default="documents/modelo_crud.pdf"),
        ]

        def submit(values: Dict[str, str]) -> bool:
            headers, rows = self.app.repo.read_table("atividades.csv")
            id_turma = values["id_turma"].strip()
            titulo = values["titulo"].strip()
            descricao = values["descricao"].strip()
            arquivo = values["arquivo"].strip()

            if not id_turma.isdigit():
                messagebox.showerror("Atividades", "ID da turma deve ser numerico.")
                return False
            if not titulo:
                messagebox.showerror("Atividades", "Titulo obrigatorio.")
                return False

            new_id = DataRepository.next_numeric_id(rows, "ID")
            rows.append(
                {"ID": str(new_id), "ID_Turma": id_turma, "Titulo": titulo, "Descricao": descricao, "Arquivo": arquivo}
            )
            self.app.repo.write_table("atividades.csv", rows, headers)
            self._refresh_activities(tree, columns)
            return True

        open_form_dialog(self, "Cadastrar atividade", fields, submit)

    def _show_activity_file(self, tree: ttk.Treeview) -> None:
        selection = tree.selection()
        if not selection:
            messagebox.showwarning("Atividades", "Selecione uma atividade primeiro.")
            return
        values = tree.item(selection[0], "values")
        if len(values) < 5:
            messagebox.showerror("Atividades", "Linha invalida.")
            return
        relative_path = values[4]
        absolute_path = (BASE_DIR / relative_path).resolve()
        if absolute_path.exists():
            messagebox.showinfo(
                "Atividades",
                f"Arquivo localizado em:\n{absolute_path}\nAbra manualmente ou copie para acesso rapido.",
            )
        else:
            messagebox.showwarning(
                "Atividades", f"Arquivo {relative_path} nao encontrado. Verifique o caminho informado."
            )

    def _generate_summary_lines(self) -> List[str]:
        _, alunos = self.app.repo.read_table("alunos.csv")
        _, turmas = self.app.repo.read_table("turmas.csv")
        _, aulas = self.app.repo.read_table("aulas.csv")
        _, atividades = self.app.repo.read_table("atividades.csv")

        ativos = sum(1 for aluno in alunos if is_truthy(aluno.get("Ativo", "1")))

        lines = [
            f"Total de alunos: {len(alunos)} (ativos: {ativos})",
            f"Total de turmas: {len(turmas)}",
            f"Total de aulas registradas: {len(aulas)}",
            f"Atividades disponiveis: {len(atividades)}",
        ]
        return lines


def open_form_dialog(
    parent: tk.Misc, title: str, fields: Sequence[FormField], on_submit: Callable[[Dict[str, str]], bool]
) -> None:
    """Abre um modal padrao e executa on_submit quando o usuario confirma."""

    top = tk.Toplevel(parent)
    top.title(title)
    top.transient(parent.winfo_toplevel())
    top.grab_set()

    entries: Dict[str, tk.Variable] = {}
    for index, field in enumerate(fields):
        ttk.Label(top, text=field.label).grid(row=index, column=0, sticky="e", padx=5, pady=5)
        if field.options:
            var = tk.StringVar(value=field.default or (field.options[0] if field.options else ""))
            widget = ttk.Combobox(top, textvariable=var, values=list(field.options or []), state="readonly")
        else:
            var = tk.StringVar(value=field.default)
            widget = ttk.Entry(top, textvariable=var, width=30)
        widget.grid(row=index, column=1, padx=5, pady=5)
        entries[field.name] = var

    def submit() -> None:
        values = {name: var.get() for name, var in entries.items()}
        if on_submit(values):
            top.destroy()

    ttk.Button(top, text="Salvar", command=submit).grid(row=len(fields), column=0, columnspan=2, pady=10)
    top.bind("<Return>", lambda _event: submit())
    top.wait_window(top)


def main() -> None:
    root = tk.Tk()
    AcademicApp(root)
    root.mainloop()


if __name__ == "__main__":
    main()
