"""
Sistema Acadêmico PIM 2025 - Versão Completa com CustomTkinter
Integração da main.py original com design moderno
TODAS as funcionalidades mantidas + Visual moderno aplicado
"""

from __future__ import annotations
import csv
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, Dict, Iterable, List, Optional, Sequence, Tuple
import customtkinter as ctk
from tkinter import messagebox, ttk
import tkinter as tk

# ============= CONFIGURAÇÕES =============

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
        "CADASTRAR_TURMA", "EDITAR_TURMA", "REGISTRAR_AULA",
        "UPLOAD_ATIVIDADE", "VISUALIZAR_ALUNOS", "GERAR_RELATORIO",
        "CONSULTAR_TURMAS", "CONSULTAR_AULAS", "BAIXAR_ATIVIDADE",
    },
    "ALUNO": {
        "CONSULTAR_TURMAS", "CONSULTAR_AULAS", "BAIXAR_ATIVIDADE", "VISUALIZAR_NOTAS",
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

# ============= CORES E ESTILOS =============

class Config:
    COR_FUNDO_ESCURO = "#0a1929"
    COR_FUNDO_CARD = "#112240"
    COR_GRADIENTE_1 = "#1a4d6f"
    COR_GRADIENTE_2 = "#0f3854"
    COR_PRIMARIA = "#4a9eff"
    COR_SECUNDARIA = "#00d4ff"
    COR_SUCESSO = "#00e5a0"
    COR_ERRO = "#ff4d6d"
    COR_TEXTO = "#e6f1ff"
    COR_TEXTO_SECUNDARIO = "#8892b0"
    COR_HOVER = "#1e3a5f"
    
    FONTE_TITULO = ("Segoe UI", 24, "bold")
    FONTE_SUBTITULO = ("Segoe UI", 16, "bold")
    FONTE_NORMAL = ("Segoe UI", 11)
    FONTE_PEQUENA = ("Segoe UI", 9)

# ============= DATACLASSES =============

@dataclass
class Session:
    user_id: int
    login: str
    tipo: str

@dataclass
class FormField:
    name: str
    label: str
    default: str = ""
    options: Optional[Sequence[str]] = None

# ============= FUNÇÕES AUXILIARES =============

def is_truthy(value: str) -> bool:
    return str(value).strip() in {"1", "True", "true", "SIM", "Sim"}

def truthy_flag(value: str) -> str:
    return "Sim" if is_truthy(value) else "Nao"

# ============= REPOSITÓRIO DE DADOS =============

class DataRepository:
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

    def write_table(self, filename: str, rows: List[Dict[str, str]], headers: Optional[Sequence[str]] = None) -> None:
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

# ============= SERVIÇO DE AUTENTICAÇÃO =============

class AuthService:
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

# ============= COMPONENTES UI =============

class ComponentesUI:
    @staticmethod
    def criar_botao_primario(parent, texto, comando, width=200):
        btn = ctk.CTkButton(
            parent, text=texto, command=comando, width=width, height=40,
            corner_radius=8, fg_color=Config.COR_PRIMARIA,
            hover_color=Config.COR_SECUNDARIA, font=Config.FONTE_NORMAL
        )
        return btn
    
    @staticmethod
    def criar_botao_secundario(parent, texto, comando, width=150):
        btn = ctk.CTkButton(
            parent, text=texto, command=comando, width=width, height=35,
            corner_radius=8, fg_color="transparent", border_width=2,
            border_color=Config.COR_PRIMARIA, hover_color=Config.COR_HOVER,
            font=Config.FONTE_NORMAL
        )
        return btn
    
    @staticmethod
    def criar_tabela(parent, columns: Sequence[Tuple[str, str]], height=12):
        frame = ctk.CTkFrame(parent, fg_color=Config.COR_FUNDO_CARD, corner_radius=10)
        tree_frame = tk.Frame(frame, bg=Config.COR_FUNDO_CARD)
        tree_frame.pack(fill="both", expand=True, padx=2, pady=2)
        
        style = ttk.Style()
        style.theme_use('clam')
        style.configure("Custom.Treeview", background=Config.COR_FUNDO_CARD,
                       foreground=Config.COR_TEXTO, fieldbackground=Config.COR_FUNDO_CARD,
                       borderwidth=0, font=Config.FONTE_NORMAL)
        style.configure("Custom.Treeview.Heading", background=Config.COR_GRADIENTE_2,
                       foreground=Config.COR_TEXTO, borderwidth=0,
                       font=("Segoe UI", 11, "bold"))
        style.map('Custom.Treeview', background=[('selected', Config.COR_PRIMARIA)])
        
        scroll_y = tk.Scrollbar(tree_frame, orient="vertical")
        scroll_x = tk.Scrollbar(tree_frame, orient="horizontal")
        
        tree = ttk.Treeview(
            tree_frame, columns=[col for col, _ in columns], show="headings",
            height=height, yscrollcommand=scroll_y.set, xscrollcommand=scroll_x.set,
            style="Custom.Treeview"
        )
        
        scroll_y.config(command=tree.yview)
        scroll_x.config(command=tree.xview)
        
        for column, label in columns:
            tree.heading(column, text=label)
            width_val = 100 if column.upper() in {"ID", "RA", "ANO"} else 200
            tree.column(column, width=width_val, anchor="w")
        
        tree.grid(row=0, column=0, sticky="nsew")
        scroll_y.grid(row=0, column=1, sticky="ns")
        scroll_x.grid(row=1, column=0, sticky="ew")
        
        tree_frame.grid_rowconfigure(0, weight=1)
        tree_frame.grid_columnconfigure(0, weight=1)
        
        return frame, tree

# ============= TELA DE LOGIN =============

class LoginFrame(ctk.CTkFrame):
    def __init__(self, master, app) -> None:
        super().__init__(master, fg_color=Config.COR_FUNDO_ESCURO)
        self.app = app
        
        container = ctk.CTkFrame(
            self, width=450, height=600, fg_color=Config.COR_FUNDO_CARD,
            corner_radius=20, border_width=2, border_color=Config.COR_PRIMARIA
        )
        container.place(relx=0.5, rely=0.5, anchor="center")
        container.pack_propagate(False)
        
        ctk.CTkLabel(container, text="🎓", font=("Segoe UI", 60)).pack(pady=(40, 10))
        ctk.CTkLabel(container, text="Sistema Acadêmico", font=Config.FONTE_TITULO,
                    text_color=Config.COR_TEXTO).pack(pady=(0, 5))
        ctk.CTkLabel(container, text="PIM 2025 - Escola Lourença", font=Config.FONTE_NORMAL,
                    text_color=Config.COR_TEXTO_SECUNDARIO).pack(pady=(0, 40))
        
        # Login
        login_frame = ctk.CTkFrame(container, fg_color="transparent")
        login_frame.pack(pady=(0, 20), padx=50)
        ctk.CTkLabel(login_frame, text="Usuário", font=Config.FONTE_NORMAL,
                    text_color=Config.COR_TEXTO_SECUNDARIO, anchor="w").pack(anchor="w", pady=(0, 8))
        self.login_var = tk.StringVar()
        self.login_entry = ctk.CTkEntry(
            login_frame, width=350, height=45, corner_radius=8, border_width=1,
            border_color=Config.COR_PRIMARIA, fg_color=Config.COR_FUNDO_CARD,
            font=Config.FONTE_NORMAL, textvariable=self.login_var
        )
        self.login_entry.pack()
        
        # Senha
        senha_frame = ctk.CTkFrame(container, fg_color="transparent")
        senha_frame.pack(pady=(0, 30), padx=50)
        ctk.CTkLabel(senha_frame, text="Senha", font=Config.FONTE_NORMAL,
                    text_color=Config.COR_TEXTO_SECUNDARIO, anchor="w").pack(anchor="w", pady=(0, 8))
        self.senha_var = tk.StringVar()
        self.senha_entry = ctk.CTkEntry(
            senha_frame, width=350, height=45, corner_radius=8, border_width=1,
            border_color=Config.COR_PRIMARIA, fg_color=Config.COR_FUNDO_CARD,
            font=Config.FONTE_NORMAL, textvariable=self.senha_var, show="●"
        )
        self.senha_entry.pack()
        
        ComponentesUI.criar_botao_primario(container, "Entrar", self._submit, 350).pack(pady=(0, 15), padx=50)
        ComponentesUI.criar_botao_secundario(container, "Sair", self.master.quit, 350).pack(pady=(0, 30), padx=50)
        
        self.senha_entry.bind("<Return>", lambda _: self._submit())
        self.login_entry.focus()
        
        # Info
        info_frame = ctk.CTkFrame(container, fg_color="transparent")
        info_frame.pack(pady=10)
        ctk.CTkLabel(info_frame, text="👤 Usuários de Teste", font=("Segoe UI", 10, "bold"),
                    text_color=Config.COR_PRIMARIA).pack(pady=(0, 8))
        for text in ["Admin: admin / admin123", "Professor: professor / professor", "Aluno: aluno / aluno"]:
            ctk.CTkLabel(info_frame, text=text, font=Config.FONTE_PEQUENA,
                        text_color=Config.COR_TEXTO_SECUNDARIO).pack()
    
    def _submit(self) -> None:
        self.app.handle_login(self.login_var.get(), self.senha_var.get())

# ============= DASHBOARD PRINCIPAL =============

class DashboardFrame(ctk.CTkFrame):
    PAGE_BLUEPRINTS: Sequence[Tuple[str, str, Optional[Sequence[str]]]] = (
        ("Visao Geral", "_build_overview_tab", None),
        ("Usuarios", "_build_users_tab", ("ADMIN_ONLY",)),
        ("Alunos", "_build_students_tab", ("VISUALIZAR_ALUNOS",)),
        ("Turmas", "_build_classes_tab", ("CADASTRAR_TURMA", "EDITAR_TURMA", "CONSULTAR_TURMAS")),
        ("Aulas", "_build_lessons_tab", ("REGISTRAR_AULA", "CONSULTAR_AULAS")),
        ("Atividades", "_build_activities_tab", ("UPLOAD_ATIVIDADE", "BAIXAR_ATIVIDADE")),
        ("Relatorios", "_build_reports_tab", ("GERAR_RELATORIO", "VISUALIZAR_NOTAS")),
    )

    def __init__(self, master, app, session: Session) -> None:
        super().__init__(master, fg_color=Config.COR_FUNDO_ESCURO)
        self.app = app
        self.session = session
        self.permissions = self.app.permissions_for(self.session.tipo)
        
        # Header
        header = ctk.CTkFrame(self, fg_color=Config.COR_FUNDO_CARD, corner_radius=0, height=80)
        header.pack(fill="x")
        header.pack_propagate(False)
        
        header_content = ctk.CTkFrame(header, fg_color="transparent")
        header_content.pack(fill="both", expand=True, padx=30, pady=20)
        
        left_frame = ctk.CTkFrame(header_content, fg_color="transparent")
        left_frame.pack(side="left", fill="y")
        
        ctk.CTkLabel(left_frame, text=f"👋 Bem-vindo, {self.session.login}",
                    font=("Segoe UI", 16, "bold"), text_color=Config.COR_TEXTO).pack(side="left")
        ctk.CTkLabel(left_frame, text=f"  •  {self.session.tipo}", font=Config.FONTE_NORMAL,
                    text_color=Config.COR_PRIMARIA).pack(side="left", padx=(10, 0))
        
        ctk.CTkButton(header_content, text="← Sair", command=self.app.logout, width=100, height=35,
                     corner_radius=8, fg_color=Config.COR_ERRO, hover_color="#cc3d57",
                     font=Config.FONTE_NORMAL).pack(side="right")
        
        # Tabs
        self.tabview = ctk.CTkTabview(
            self, fg_color=Config.COR_FUNDO_ESCURO,
            segmented_button_fg_color=Config.COR_FUNDO_CARD,
            segmented_button_selected_color=Config.COR_PRIMARIA,
            segmented_button_selected_hover_color=Config.COR_SECUNDARIA,
            segmented_button_unselected_color=Config.COR_FUNDO_CARD,
            segmented_button_unselected_hover_color=Config.COR_HOVER
        )
        self.tabview.pack(fill="both", expand=True, padx=20, pady=20)
        
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
            tab = self.tabview.add(title)
            builder(tab)
    
    def _refresh_tree(self, tree, rows: List[Dict[str, str]], columns: Sequence[Tuple[str, str]]) -> None:
        for item in tree.get_children():
            tree.delete(item)
        for row in rows:
            values = []
            for column, _ in columns:
                cell = row.get(column, "")
                if column == "Ativo":
                    cell = truthy_flag(cell)
                values.append(str(cell))
            tree.insert("", "end", values=values)
    
    def _get_selected_row(self, tree, columns: Sequence[Tuple[str, str]]) -> Optional[Dict[str, str]]:
        selection = tree.selection()
        if not selection:
            messagebox.showwarning("Selecao vazia", "Selecione uma linha primeiro.")
            return None
        values = tree.item(selection[0], "values")
        row = {}
        for index, (column, _) in enumerate(columns):
            row[column] = str(values[index])
        return row
    
    # ============= VISÃO GERAL =============
    
    def _build_overview_tab(self, parent) -> None:
        scroll_frame = ctk.CTkScrollableFrame(parent, fg_color="transparent")
        scroll_frame.pack(fill="both", expand=True)
        
        ctk.CTkLabel(scroll_frame, text="📊 Visão Geral do Sistema", font=Config.FONTE_SUBTITULO,
                    text_color=Config.COR_TEXTO).pack(anchor="w", pady=(0, 20))
        
        card = ctk.CTkFrame(scroll_frame, fg_color=Config.COR_FUNDO_CARD, corner_radius=15,
                           border_width=1, border_color=Config.COR_GRADIENTE_1)
        card.pack(fill="x", pady=(0, 20))
        
        ctk.CTkLabel(card, text="Dados da Sessão", font=("Segoe UI", 13, "bold"),
                    text_color=Config.COR_PRIMARIA).pack(anchor="w", padx=20, pady=(20, 15))
        
        info_frame = ctk.CTkFrame(card, fg_color="transparent")
        info_frame.pack(fill="x", padx=20, pady=(0, 20))
        
        ctk.CTkLabel(info_frame, text=f"Login: {self.session.login}",
                    text_color=Config.COR_TEXTO).pack(anchor="w", pady=5)
        ctk.CTkLabel(info_frame, text=f"Perfil: {self.session.tipo}",
                    text_color=Config.COR_TEXTO).pack(anchor="w", pady=5)
        
        # Permissões
        perm_card = ctk.CTkFrame(scroll_frame, fg_color=Config.COR_FUNDO_CARD, corner_radius=15,
                                border_width=1, border_color=Config.COR_GRADIENTE_1)
        perm_card.pack(fill="both", expand=True)
        
        ctk.CTkLabel(perm_card, text="Permissões Liberadas", font=("Segoe UI", 13, "bold"),
                    text_color=Config.COR_PRIMARIA).pack(anchor="w", padx=20, pady=(20, 15))
        
        listbox_frame = ctk.CTkFrame(perm_card, fg_color=Config.COR_FUNDO_ESCURO)
        listbox_frame.pack(fill="both", expand=True, padx=20, pady=(0, 20))
        
        listbox = tk.Listbox(listbox_frame, height=10, bg=Config.COR_FUNDO_ESCURO,
                           fg=Config.COR_TEXTO, font=Config.FONTE_NORMAL, borderwidth=0)
        listbox.pack(fill="both", expand=True, padx=10, pady=10)
        
        if self.is_admin():
            listbox.insert(tk.END, "🔓 Administrador - acesso total")
        else:
            allowed = sorted({ACTION_LABELS.get(action, action) for action in self.permissions})
            for label in allowed:
                listbox.insert(tk.END, f"✓ {label}")
            if not allowed:
                listbox.insert(tk.END, "❌ Nenhuma ação liberada.")
    
    # ============= USUÁRIOS =============
    
    def _build_users_tab(self, parent) -> None:
        scroll_frame = ctk.CTkScrollableFrame(parent, fg_color="transparent")
        scroll_frame.pack(fill="both", expand=True)
        
        ctk.CTkLabel(scroll_frame, text="👥 Gerenciamento de Usuários", font=Config.FONTE_SUBTITULO,
                    text_color=Config.COR_TEXTO).pack(anchor="w", pady=(0, 20))
        
        columns = [("ID", "ID"), ("Login", "Login"), ("Tipo", "Tipo"), ("Ativo", "Ativo")]
        table_frame, tree = ComponentesUI.criar_tabela(scroll_frame, columns)
        table_frame.pack(fill="both", expand=True, pady=(0, 20))
        
        self._refresh_users(tree, columns)
        
        btn_frame = ctk.CTkFrame(scroll_frame, fg_color="transparent")
        btn_frame.pack(fill="x")
        
        ComponentesUI.criar_botao_secundario(btn_frame, "➕ Adicionar", 
            lambda: self._add_user(tree, columns), 150).pack(side="left", padx=5)
        ComponentesUI.criar_botao_secundario(btn_frame, "🔑 Resetar Senha",
            lambda: self._reset_user_password(tree, columns), 150).pack(side="left", padx=5)
        ComponentesUI.criar_botao_secundario(btn_frame, "⚡ Ativar/Desativar",
            lambda: self._toggle_user(tree, columns), 180).pack(side="left", padx=5)
    
    def _refresh_users(self, tree, columns) -> None:
        _, rows = self.app.repo.read_table("usuarios.csv")
        self._refresh_tree(tree, rows, columns)
    
    def _add_user(self, tree, columns) -> None:
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
                messagebox.showerror("Usuarios", "Login e senha são obrigatórios.")
                return False
            if tipo not in {"ADMIN", "PROFESSOR", "ALUNO"}:
                messagebox.showerror("Usuarios", "Tipo inválido.")
                return False
            
            headers, rows = self.app.repo.read_table("usuarios.csv")
            for row in rows:
                if row.get("Login", "").lower() == login.lower():
                    messagebox.showerror("Usuarios", "Login já cadastrado.")
                    return False
            
            new_id = DataRepository.next_numeric_id(rows, "ID")
            rows.append({"ID": str(new_id), "Login": login, "Senha": senha, "Tipo": tipo, "Ativo": "1"})
            self.app.repo.write_table("usuarios.csv", rows, headers)
            self._refresh_users(tree, columns)
            return True
        
        open_form_dialog(self, "Novo usuário", fields, submit)
    
    def _reset_user_password(self, tree, columns) -> None:
        selected = self._get_selected_row(tree, columns)
        if selected is None:
            return
        
        fields = [FormField("senha", f"Nova senha para {selected.get('Login', '')}")]
        
        def submit(values: Dict[str, str]) -> bool:
            nova_senha = values["senha"].strip()
            if not nova_senha:
                messagebox.showerror("Usuarios", "Senha não pode ser vazia.")
                return False
            
            headers, rows = self.app.repo.read_table("usuarios.csv")
            for row in rows:
                if row.get("Login") == selected.get("Login"):
                    row["Senha"] = nova_senha
                    self.app.repo.write_table("usuarios.csv", rows, headers)
                    self._refresh_users(tree, columns)
                    messagebox.showinfo("Usuarios", "Senha atualizada com sucesso.")
                    return True
            
            messagebox.showerror("Usuarios", "Usuário não encontrado.")
            return False
        
        open_form_dialog(self, "Resetar senha", fields, submit)
    
    def _toggle_user(self, tree, columns) -> None:
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
        
        messagebox.showerror("Usuarios", "Usuário não encontrado.")
    
    # ============= ALUNOS =============
    
    def _build_students_tab(self, parent) -> None:
        scroll_frame = ctk.CTkScrollableFrame(parent, fg_color="transparent")
        scroll_frame.pack(fill="both", expand=True)
        
        ctk.CTkLabel(scroll_frame, text="👨‍🎓 Gerenciamento de Alunos", font=Config.FONTE_SUBTITULO,
                    text_color=Config.COR_TEXTO).pack(anchor="w", pady=(0, 20))
        
        columns = [("RA", "RA"), ("Nome", "Nome"), ("Email", "Email"), ("Ativo", "Ativo")]
        table_frame, tree = ComponentesUI.criar_tabela(scroll_frame, columns)
        table_frame.pack(fill="both", expand=True, pady=(0, 20))
        
        self._refresh_students(tree, columns)
        
        btn_frame = ctk.CTkFrame(scroll_frame, fg_color="transparent")
        btn_frame.pack(fill="x")
        
        add_btn = ComponentesUI.criar_botao_secundario(btn_frame, "➕ Adicionar",
            lambda: self._add_student(tree, columns), 150)
        toggle_btn = ComponentesUI.criar_botao_secundario(btn_frame, "⚡ Ativar/Desativar",
            lambda: self._toggle_student(tree, columns), 180)
        
        add_btn.pack(side="left", padx=5)
        toggle_btn.pack(side="left", padx=5)
        
        if not self.is_admin():
            add_btn.configure(state="disabled")
            toggle_btn.configure(state="disabled")
        
        ComponentesUI.criar_botao_secundario(btn_frame, "🔄 Atualizar",
            lambda: self._refresh_students(tree, columns), 120).pack(side="right")
    
    def _refresh_students(self, tree, columns) -> None:
        _, rows = self.app.repo.read_table("alunos.csv")
        self._refresh_tree(tree, rows, columns)
    
    def _add_student(self, tree, columns) -> None:
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
                messagebox.showerror("Alunos", "Nome e email são obrigatórios.")
                return False
            
            if not ra:
                ra = str(DataRepository.next_numeric_id(rows, "RA"))
            
            if not ra.isdigit():
                messagebox.showerror("Alunos", "RA deve ser numérico.")
                return False
            
            for row in rows:
                if row.get("RA") == ra:
                    messagebox.showerror("Alunos", "RA já existente.")
                    return False
            
            rows.append({"RA": ra, "Nome": nome, "Email": email, "Ativo": ativo})
            self.app.repo.write_table("alunos.csv", rows, headers)
            self._refresh_students(tree, columns)
            return True
        
        open_form_dialog(self, "Novo aluno", fields, submit)
    
    def _toggle_student(self, tree, columns) -> None:
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
        
        messagebox.showerror("Alunos", "Aluno não encontrado.")
    
    # ============= TURMAS =============
    
    def _build_classes_tab(self, parent) -> None:
        scroll_frame = ctk.CTkScrollableFrame(parent, fg_color="transparent")
        scroll_frame.pack(fill="both", expand=True)
        
        ctk.CTkLabel(scroll_frame, text="📚 Gerenciamento de Turmas", font=Config.FONTE_SUBTITULO,
                    text_color=Config.COR_TEXTO).pack(anchor="w", pady=(0, 20))
        
        columns = [("ID", "ID"), ("Nome", "Turma"), ("Professor", "Professor"),
                  ("Ano", "Ano"), ("Semestre", "Semestre")]
        table_frame, tree = ComponentesUI.criar_tabela(scroll_frame, columns)
        table_frame.pack(fill="both", expand=True, pady=(0, 20))
        
        self._refresh_classes(tree, columns)
        
        btn_frame = ctk.CTkFrame(scroll_frame, fg_color="transparent")
        btn_frame.pack(fill="x")
        
        add_btn = ComponentesUI.criar_botao_secundario(btn_frame, "➕ Cadastrar",
            lambda: self._add_class(tree, columns), 150)
        edit_btn = ComponentesUI.criar_botao_secundario(btn_frame, "✎ Editar",
            lambda: self._edit_class(tree, columns), 120)
        
        add_btn.pack(side="left", padx=5)
        edit_btn.pack(side="left", padx=5)
        
        ComponentesUI.criar_botao_secundario(btn_frame, "🔄 Atualizar",
            lambda: self._refresh_classes(tree, columns), 120).pack(side="right")
        
        if not (self.is_admin() or self.has_action("CADASTRAR_TURMA")):
            add_btn.configure(state="disabled")
        if not (self.is_admin() or self.has_action("EDITAR_TURMA")):
            edit_btn.configure(state="disabled")
    
    def _refresh_classes(self, tree, columns) -> None:
        _, rows = self.app.repo.read_table("turmas.csv")
        self._refresh_tree(tree, rows, columns)
    
    def _add_class(self, tree, columns) -> None:
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
                messagebox.showerror("Turmas", "Nome e professor são obrigatórios.")
                return False
            
            new_id = DataRepository.next_numeric_id(rows, "ID")
            rows.append({
                "ID": str(new_id), "Nome": nome, "Professor": professor,
                "Ano": ano or "0", "Semestre": semestre or "0"
            })
            self.app.repo.write_table("turmas.csv", rows, headers)
            self._refresh_classes(tree, columns)
            return True
        
        open_form_dialog(self, "Cadastrar turma", fields, submit)
    
    def _edit_class(self, tree, columns) -> None:
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
                    row.update({
                        "Nome": values["nome"].strip(),
                        "Professor": values["professor"].strip(),
                        "Ano": values["ano"].strip(),
                        "Semestre": values["semestre"].strip(),
                    })
                    self.app.repo.write_table("turmas.csv", rows, headers)
                    self._refresh_classes(tree, columns)
                    return True
            
            messagebox.showerror("Turmas", "Turma não encontrada.")
            return False
        
        open_form_dialog(self, "Editar turma", fields, submit)
    
    # ============= AULAS =============
    
    def _build_lessons_tab(self, parent) -> None:
        scroll_frame = ctk.CTkScrollableFrame(parent, fg_color="transparent")
        scroll_frame.pack(fill="both", expand=True)
        
        ctk.CTkLabel(scroll_frame, text="📝 Registro de Aulas", font=Config.FONTE_SUBTITULO,
                    text_color=Config.COR_TEXTO).pack(anchor="w", pady=(0, 20))
        
        columns = [("ID", "ID"), ("ID_Turma", "Turma"), ("Data", "Data"), ("Conteudo", "Conteúdo")]
        table_frame, tree = ComponentesUI.criar_tabela(scroll_frame, columns)
        table_frame.pack(fill="both", expand=True, pady=(0, 20))
        
        self._refresh_lessons(tree, columns)
        
        btn_frame = ctk.CTkFrame(scroll_frame, fg_color="transparent")
        btn_frame.pack(fill="x")
        
        add_btn = ComponentesUI.criar_botao_secundario(btn_frame, "➕ Registrar",
            lambda: self._add_lesson(tree, columns), 150)
        add_btn.pack(side="left", padx=5)
        
        ComponentesUI.criar_botao_secundario(btn_frame, "🔄 Atualizar",
            lambda: self._refresh_lessons(tree, columns), 120).pack(side="right")
        
        if not (self.is_admin() or self.has_action("REGISTRAR_AULA")):
            add_btn.configure(state="disabled")
    
    def _refresh_lessons(self, tree, columns) -> None:
        _, rows = self.app.repo.read_table("aulas.csv")
        self._refresh_tree(tree, rows, columns)
    
    def _add_lesson(self, tree, columns) -> None:
        fields = [
            FormField("id_turma", "ID da turma"),
            FormField("data", "Data (dd/mm/aaaa)"),
            FormField("conteudo", "Conteúdo abordado"),
        ]
        
        def submit(values: Dict[str, str]) -> bool:
            headers, rows = self.app.repo.read_table("aulas.csv")
            id_turma = values["id_turma"].strip()
            data = values["data"].strip()
            conteudo = values["conteudo"].strip()
            
            if not id_turma.isdigit():
                messagebox.showerror("Aulas", "ID da turma deve ser numérico.")
                return False
            
            if not data or not conteudo:
                messagebox.showerror("Aulas", "Data e conteúdo são obrigatórios.")
                return False
            
            new_id = DataRepository.next_numeric_id(rows, "ID")
            rows.append({"ID": str(new_id), "ID_Turma": id_turma, "Data": data, "Conteudo": conteudo})
            self.app.repo.write_table("aulas.csv", rows, headers)
            self._refresh_lessons(tree, columns)
            return True
        
        open_form_dialog(self, "Registrar aula", fields, submit)
    
    # ============= ATIVIDADES =============
    
    def _build_activities_tab(self, parent) -> None:
        scroll_frame = ctk.CTkScrollableFrame(parent, fg_color="transparent")
        scroll_frame.pack(fill="both", expand=True)
        
        ctk.CTkLabel(scroll_frame, text="📄 Gerenciamento de Atividades", font=Config.FONTE_SUBTITULO,
                    text_color=Config.COR_TEXTO).pack(anchor="w", pady=(0, 20))
        
        columns = [("ID", "ID"), ("ID_Turma", "Turma"), ("Titulo", "Título"),
                  ("Descricao", "Descrição"), ("Arquivo", "Arquivo")]
        table_frame, tree = ComponentesUI.criar_tabela(scroll_frame, columns)
        table_frame.pack(fill="both", expand=True, pady=(0, 20))
        
        self._refresh_activities(tree, columns)
        
        btn_frame = ctk.CTkFrame(scroll_frame, fg_color="transparent")
        btn_frame.pack(fill="x")
        
        add_btn = ComponentesUI.criar_botao_secundario(btn_frame, "➕ Cadastrar",
            lambda: self._add_activity(tree, columns), 150)
        show_btn = ComponentesUI.criar_botao_secundario(btn_frame, "📁 Mostrar Arquivo",
            lambda: self._show_activity_file(tree), 180)
        
        add_btn.pack(side="left", padx=5)
        show_btn.pack(side="left", padx=5)
        
        ComponentesUI.criar_botao_secundario(btn_frame, "🔄 Atualizar",
            lambda: self._refresh_activities(tree, columns), 120).pack(side="right")
        
        if not (self.is_admin() or self.has_action("UPLOAD_ATIVIDADE")):
            add_btn.configure(state="disabled")
        if not (self.is_admin() or self.has_action("BAIXAR_ATIVIDADE")):
            show_btn.configure(state="disabled")
    
    def _refresh_activities(self, tree, columns) -> None:
        _, rows = self.app.repo.read_table("atividades.csv")
        self._refresh_tree(tree, rows, columns)
    
    def _add_activity(self, tree, columns) -> None:
        fields = [
            FormField("id_turma", "ID da turma"),
            FormField("titulo", "Título da atividade"),
            FormField("descricao", "Descrição"),
            FormField("arquivo", "Caminho do arquivo", default="documents/modelo_crud.pdf"),
        ]
        
        def submit(values: Dict[str, str]) -> bool:
            headers, rows = self.app.repo.read_table("atividades.csv")
            id_turma = values["id_turma"].strip()
            titulo = values["titulo"].strip()
            descricao = values["descricao"].strip()
            arquivo = values["arquivo"].strip()
            
            if not id_turma.isdigit():
                messagebox.showerror("Atividades", "ID da turma deve ser numérico.")
                return False
            
            if not titulo:
                messagebox.showerror("Atividades", "Título obrigatório.")
                return False
            
            new_id = DataRepository.next_numeric_id(rows, "ID")
            rows.append({
                "ID": str(new_id), "ID_Turma": id_turma, "Titulo": titulo,
                "Descricao": descricao, "Arquivo": arquivo
            })
            self.app.repo.write_table("atividades.csv", rows, headers)
            self._refresh_activities(tree, columns)
            return True
        
        open_form_dialog(self, "Cadastrar atividade", fields, submit)
    
    def _show_activity_file(self, tree) -> None:
        selection = tree.selection()
        if not selection:
            messagebox.showwarning("Atividades", "Selecione uma atividade primeiro.")
            return
        
        values = tree.item(selection[0], "values")
        if len(values) < 5:
            messagebox.showerror("Atividades", "Linha inválida.")
            return
        
        relative_path = values[4]
        absolute_path = (BASE_DIR / relative_path).resolve()
        
        if absolute_path.exists():
            messagebox.showinfo("Atividades",
                f"Arquivo localizado em:\n{absolute_path}\n\nAbra manualmente ou copie para acesso rápido.")
        else:
            messagebox.showwarning("Atividades",
                f"Arquivo {relative_path} não encontrado. Verifique o caminho informado.")
    
    # ============= RELATÓRIOS =============
    
    def _build_reports_tab(self, parent) -> None:
        scroll_frame = ctk.CTkScrollableFrame(parent, fg_color="transparent")
        scroll_frame.pack(fill="both", expand=True)
        
        ctk.CTkLabel(scroll_frame, text="📊 Relatórios e Estatísticas", font=Config.FONTE_SUBTITULO,
                    text_color=Config.COR_TEXTO).pack(anchor="w", pady=(0, 20))
        
        # Card de resumo
        summary_card = ctk.CTkFrame(scroll_frame, fg_color=Config.COR_FUNDO_CARD, corner_radius=15,
                                   border_width=1, border_color=Config.COR_GRADIENTE_1)
        summary_card.pack(fill="x", pady=(0, 20))
        
        ctk.CTkLabel(summary_card, text="Resumo Rápido", font=("Segoe UI", 13, "bold"),
                    text_color=Config.COR_PRIMARIA).pack(anchor="w", padx=20, pady=(20, 15))
        
        summary_text = ctk.CTkTextbox(summary_card, height=150, fg_color=Config.COR_FUNDO_ESCURO,
                                      border_width=0, font=Config.FONTE_NORMAL)
        summary_text.pack(fill="x", padx=20, pady=(0, 15))
        
        def generate_summary() -> None:
            lines = self._generate_summary_lines()
            summary_text.delete("1.0", "end")
            summary_text.insert("1.0", "\n".join(lines))
        
        btn_summary = ComponentesUI.criar_botao_secundario(summary_card, "📈 Gerar Relatório",
                                                           generate_summary, 180)
        btn_summary.pack(anchor="e", padx=20, pady=(0, 20))
        
        if not (self.is_admin() or self.has_action("GERAR_RELATORIO")):
            btn_summary.configure(state="disabled")
        
        # Card de relatórios salvos
        notes_card = ctk.CTkFrame(scroll_frame, fg_color=Config.COR_FUNDO_CARD, corner_radius=15,
                                 border_width=1, border_color=Config.COR_GRADIENTE_1)
        notes_card.pack(fill="both", expand=True)
        
        ctk.CTkLabel(notes_card, text="Relatórios Salvos (Notas)", font=("Segoe UI", 13, "bold"),
                    text_color=Config.COR_PRIMARIA).pack(anchor="w", padx=20, pady=(20, 15))
        
        ctk.CTkLabel(notes_card, text="Selecione um arquivo de relatório da pasta data/",
                    text_color=Config.COR_TEXTO_SECUNDARIO).pack(anchor="w", padx=20)
        
        report_files = sorted(DATA_DIR.glob("relatorio_turma_*.txt"))
        options = [file.name for file in report_files] if report_files else ["Nenhum arquivo encontrado"]
        
        selected_var = tk.StringVar(value=options[0] if options else "")
        combo = ctk.CTkComboBox(notes_card, variable=selected_var, values=options,
                               width=300, state="readonly")
        combo.pack(anchor="w", padx=20, pady=10)
        
        notes_text = ctk.CTkTextbox(notes_card, height=200, fg_color=Config.COR_FUNDO_ESCURO,
                                   border_width=0, font=Config.FONTE_NORMAL)
        notes_text.pack(fill="both", expand=True, padx=20, pady=(0, 15))
        
        def load_report() -> None:
            target = selected_var.get()
            if not target or target == "Nenhum arquivo encontrado":
                messagebox.showinfo("Relatórios", "Nenhum arquivo encontrado em data/relatorio_turma_*.txt.")
                return
            
            path = DATA_DIR / target
            if not path.exists():
                messagebox.showerror("Relatórios", f"Arquivo {target} não encontrado.")
                return
            
            notes_text.delete("1.0", "end")
            notes_text.insert("1.0", path.read_text(encoding="utf-8"))
        
        btn_load = ComponentesUI.criar_botao_secundario(notes_card, "📂 Abrir Relatório",
                                                        load_report, 180)
        btn_load.pack(anchor="e", padx=20, pady=(0, 20))
        
        if not (self.is_admin() or self.has_action("VISUALIZAR_NOTAS")):
            combo.configure(state="disabled")
            btn_load.configure(state="disabled")
    
    def _generate_summary_lines(self) -> List[str]:
        _, alunos = self.app.repo.read_table("alunos.csv")
        _, turmas = self.app.repo.read_table("turmas.csv")
        _, aulas = self.app.repo.read_table("aulas.csv")
        _, atividades = self.app.repo.read_table("atividades.csv")
        
        ativos = sum(1 for aluno in alunos if is_truthy(aluno.get("Ativo", "1")))
        
        lines = [
            "=" * 50,
            "RELATÓRIO GERAL DO SISTEMA ACADÊMICO",
            "=" * 50,
            "",
            f"📚 Total de alunos: {len(alunos)} (ativos: {ativos})",
            f"🏫 Total de turmas: {len(turmas)}",
            f"📝 Total de aulas registradas: {len(aulas)}",
            f"📄 Atividades disponíveis: {len(atividades)}",
            "",
            "=" * 50,
        ]
        return lines

# ============= DIÁLOGO DE FORMULÁRIO =============

def open_form_dialog(parent, title: str, fields: Sequence[FormField],
                    on_submit: Callable[[Dict[str, str]], bool]) -> None:
    top = ctk.CTkToplevel(parent)
    top.title(title)
    top.transient(parent.winfo_toplevel())
    top.grab_set()
    
    # Estilizar janela
    top.configure(fg_color=Config.COR_FUNDO_CARD)
    
    # Container
    container = ctk.CTkFrame(top, fg_color=Config.COR_FUNDO_CARD)
    container.pack(fill="both", expand=True, padx=30, pady=30)
    
    ctk.CTkLabel(container, text=title, font=Config.FONTE_SUBTITULO,
                text_color=Config.COR_PRIMARIA).pack(pady=(0, 20))
    
    entries: Dict[str, tk.Variable] = {}
    
    for field in fields:
        field_frame = ctk.CTkFrame(container, fg_color="transparent")
        field_frame.pack(fill="x", pady=10)
        
        ctk.CTkLabel(field_frame, text=field.label, text_color=Config.COR_TEXTO_SECUNDARIO,
                    anchor="w").pack(anchor="w", pady=(0, 5))
        
        if field.options:
            var = tk.StringVar(value=field.default or (field.options[0] if field.options else ""))
            widget = ctk.CTkComboBox(field_frame, variable=var, values=list(field.options or []),
                                    state="readonly", width=300, height=35)
        else:
            var = tk.StringVar(value=field.default)
            widget = ctk.CTkEntry(field_frame, textvariable=var, width=300, height=35,
                                 border_color=Config.COR_PRIMARIA)
        
        widget.pack(fill="x")
        entries[field.name] = var
    
    def submit() -> None:
        values = {name: var.get() for name, var in entries.items()}
        if on_submit(values):
            top.destroy()
    
    btn_frame = ctk.CTkFrame(container, fg_color="transparent")
    btn_frame.pack(pady=(20, 0))
    
    ComponentesUI.criar_botao_primario(btn_frame, "💾 Salvar", submit, 150).pack(side="left", padx=5)
    ComponentesUI.criar_botao_secundario(btn_frame, "✕ Cancelar", top.destroy, 150).pack(side="left", padx=5)
    
    top.bind("<Return>", lambda _: submit())
    
    # Centralizar
    top.update_idletasks()
    width = top.winfo_width()
    height = top.winfo_height()
    x = (top.winfo_screenwidth() // 2) - (width // 2)
    y = (top.winfo_screenheight() // 2) - (height // 2)
    top.geometry(f'{width}x{height}+{x}+{y}')
    
    top.wait_window()

# ============= APLICAÇÃO PRINCIPAL =============

class AcademicApp:
    def __init__(self, root: ctk.CTk) -> None:
        self.root = root
        self.root.title("Sistema Acadêmico PIM 2025")
        self.root.geometry("1400x800")
        self.root.configure(fg_color=Config.COR_FUNDO_ESCURO)
        
        self.repo = DataRepository(DATA_DIR)
        self.auth = AuthService(self.repo)
        self.session: Optional[Session] = None
        self.current_frame: Optional[ctk.CTkFrame] = None
        
        self._centralizar_janela()
        self.show_login()
    
    def _centralizar_janela(self):
        self.root.update_idletasks()
        width = 1400
        height = 800
        x = (self.root.winfo_screenwidth() // 2) - (width // 2)
        y = (self.root.winfo_screenheight() // 2) - (height // 2)
        self.root.geometry(f'{width}x{height}+{x}+{y}')
    
    def replace_frame(self, frame: ctk.CTkFrame) -> None:
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
            messagebox.showerror("Login inválido",
                "Verifique login, senha e se o usuário está ativo.")
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

# ============= MAIN =============

def main() -> None:
    ctk.set_appearance_mode("dark")
    ctk.set_default_color_theme("blue")
    
    root = ctk.CTk()
    AcademicApp(root)
    root.mainloop()

if __name__ == "__main__":
    main()