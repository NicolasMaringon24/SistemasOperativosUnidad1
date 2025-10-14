#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>
<<<<<<< HEAD
=======
#include <unordered_map>
>>>>>>> a97bba6 (Update versión final)

using namespace std;

// ======================================================
// FUNCIONES DE UTILIDAD
// ======================================================

bool es_ejecutable(const string& ruta) {
    struct stat sb;
    if (stat(ruta.c_str(), &sb) == 0) {
        return (sb.st_mode & S_IXUSR) || (sb.st_mode & S_IXGRP) || (sb.st_mode & S_IXOTH);
    }
    return false;
}

// Resuelve la ruta del comando: si no es absoluta, busca en /bin y /usr/bin
string resolver_ruta(const string& comando) {
    if (comando.empty()) return "";

    if (comando[0] == '/') {
        if (es_ejecutable(comando)) return comando;
        cerr << "Error: la ruta '" << comando << "' no existe o no tiene permisos de ejecución\n";
        return "";
    }

    string ruta = "/bin/" + comando;
    if (es_ejecutable(ruta)) return ruta;

    ruta = "/usr/bin/" + comando;
    if (es_ejecutable(ruta)) return ruta;

    cerr << "Error: comando '" << comando << "' no encontrado en /bin o /usr/bin\n";
    return "";
}

vector<char*> parse_command(string input) {
    vector<char*> args;
    stringstream ss(input);
    string token;

    while (ss >> token)
        args.push_back(strdup(token.c_str()));

    args.push_back(nullptr);
    return args;
}

void liberar_args(vector<char*>& args) {
    for (auto arg : args)
        if (arg) free(arg);
}

// ======================================================
// FUNCIONES DE EJECUCIÓN
// ======================================================

void ejecutar(vector<char*> args, bool en_fondo) {
    if (args.empty() || args[0] == nullptr) return;

    string ruta = resolver_ruta(args[0]);
    if (ruta.empty()) return;

    pid_t pid = fork();

    if (pid == 0) {
        free(args[0]);
        args[0] = strdup(ruta.c_str());
        execv(ruta.c_str(), args.data());
        perror("Error al ejecutar comando");
        exit(1);
    } else if (pid > 0) {
        if (!en_fondo)
            waitpid(pid, nullptr, 0);
        else
            cout << "[Proceso en segundo plano] PID: " << pid << endl;
    } else {
        perror("Error al crear proceso");
    }
}

// Redirección de salida (>, >>)
void ejecutar_redireccion(vector<char*> args, string archivo, bool append) {
    if (args.empty() || args[0] == nullptr) return;

    string ruta = resolver_ruta(args[0]);
    if (ruta.empty()) return;

    pid_t pid = fork();

    if (pid == 0) {
        int fd = open(archivo.c_str(),
                      append ? O_WRONLY | O_CREAT | O_APPEND
                             : O_WRONLY | O_CREAT | O_TRUNC,
                      0644);
        if (fd < 0) {
            perror("Error al abrir archivo");
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);

        free(args[0]);
        args[0] = strdup(ruta.c_str());
        execv(ruta.c_str(), args.data());
        perror("Error al ejecutar comando");
        exit(1);
    } else if (pid > 0) {
        waitpid(pid, nullptr, 0);
    } else {
        perror("Error al crear proceso");
    }
}

// Pipe (|)
void ejecutar_pipe(vector<char*> cmd1, vector<char*> cmd2) {
    if (cmd1.empty() || cmd2.empty()) return;

    string ruta1 = resolver_ruta(cmd1[0]);
    string ruta2 = resolver_ruta(cmd2[0]);
    if (ruta1.empty() || ruta2.empty()) return;

    int fd[2];
    pipe(fd);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        free(cmd1[0]);
        cmd1[0] = strdup(ruta1.c_str());
        execv(ruta1.c_str(), cmd1.data());
        perror("Error primer comando");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        close(fd[0]);
        free(cmd2[0]);
        cmd2[0] = strdup(ruta2.c_str());
        execv(ruta2.c_str(), cmd2.data());
        perror("Error segundo comando");
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);
    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);
}

// ======================================================
// FUNCIÓN PRINCIPAL
// ======================================================
int main() {
    string comando;
    vector<string> historial;
    unordered_map<string, string> alias;

    cout << "=== MiniShell Mejorada ===" << endl;
    cout << "Soporta:\n"
         << "  - Ejecución normal (ls, pwd, echo)\n"
         << "  - Rutas absolutas (/bin/ls)\n"
         << "  - Pipes (|)\n"
         << "  - Redirección (> y >>)\n"
         << "  - Segundo plano (&)\n"
<<<<<<< HEAD
=======
         << "  - Built-ins: cd, help, history, alias\n"
>>>>>>> a97bba6 (Update versión final)
         << "  - Comando salir\n\n";

    while (true) {
        cout << "minishell> ";
        getline(cin, comando);
        if (comando.empty()) continue;

<<<<<<< HEAD
=======
        // Guardar en historial
        historial.push_back(comando);

        // Limpieza de procesos zombie
        while (waitpid(-1, nullptr, WNOHANG) > 0);

        // Detectar salida
>>>>>>> a97bba6 (Update versión final)
        if (comando == "salir") {
            cout << "Saliendo de la mini-shell..." << endl;
            break;
        }

<<<<<<< HEAD
        // Limpieza de procesos zombie
        while (waitpid(-1, nullptr, WNOHANG) > 0);

        // Detectar segundo plano
        bool en_fondo = false;
        if (!comando.empty() && comando.back() == '&') {
            en_fondo = true;
            comando.pop_back();
=======
        // Alias sustitución
        stringstream ss(comando);
        string primerToken;
        ss >> primerToken;
        if (alias.find(primerToken) != alias.end()) {
            comando = alias[primerToken] + comando.substr(primerToken.size());
>>>>>>> a97bba6 (Update versión final)
        }

        // Eliminar espacios innecesarios
        auto trim = [](string &s) {
            while (!s.empty() && isspace(s.front())) s.erase(s.begin());
            while (!s.empty() && isspace(s.back())) s.pop_back();
        };
        trim(comando);

<<<<<<< HEAD
        // PIPE
=======
        // ------------------------------------
        //  BUILT-INS
        // ------------------------------------
        if (comando.rfind("cd", 0) == 0) {
            string path = comando.size() > 2 ? comando.substr(3) : getenv("HOME");
            trim(path);
            if (chdir(path.c_str()) != 0)
                perror("Error al cambiar de directorio");
            continue;
        }

        if (comando == "help") {
            cout << "=== AYUDA DE MINISHELL ===\n"
                 << "Comandos internos disponibles:\n"
                 << "  cd [dir]     - Cambiar de directorio\n"
                 << "  help         - Mostrar esta ayuda\n"
                 << "  history      - Mostrar historial de comandos\n"
                 << "  alias n='c'  - Crear alias (ej: alias ll='ls -l')\n"
                 << "  salir        - Cerrar la minishell\n\n";
            continue;
        }

        if (comando == "history") {
            cout << "=== HISTORIAL ===" << endl;
            for (size_t i = 0; i < historial.size(); ++i)
                cout << i + 1 << ": " << historial[i] << endl;
            continue;
        }

        if (comando.rfind("alias", 0) == 0) {
            size_t eq_pos = comando.find('=');
            if (eq_pos != string::npos) {
                string nombre = comando.substr(6, eq_pos - 6);
                string valor = comando.substr(eq_pos + 1);
                trim(nombre);
                trim(valor);
                if (!valor.empty() && valor.front() == '\'' && valor.back() == '\'')
                    valor = valor.substr(1, valor.size() - 2);
                alias[nombre] = valor;
                cout << "Alias agregado: " << nombre << "='" << valor << "'" << endl;
            } else {
                cout << "Uso: alias nombre='comando'" << endl;
            }
            continue;
        }

        // ------------------------------------
        //  SEGUNDO PLANO
        // ------------------------------------
        bool en_fondo = false;
        if (!comando.empty() && comando.back() == '&') {
            en_fondo = true;
            comando.pop_back();
        }
        trim(comando);

        // ------------------------------------
        //  PIPE
        // ------------------------------------
>>>>>>> a97bba6 (Update versión final)
        size_t pipe_pos = comando.find('|');
        if (pipe_pos != string::npos) {
            string left = comando.substr(0, pipe_pos);
            string right = comando.substr(pipe_pos + 1);
            trim(left);
            trim(right);

            vector<char*> cmd1 = parse_command(left);
            vector<char*> cmd2 = parse_command(right);

            ejecutar_pipe(cmd1, cmd2);

            liberar_args(cmd1);
            liberar_args(cmd2);
            continue;
        }

<<<<<<< HEAD
        // REDIRECCIÓN
=======
        // ------------------------------------
        //  REDIRECCIÓN
        // ------------------------------------
>>>>>>> a97bba6 (Update versión final)
        size_t redir_pos = comando.find('>');
        if (redir_pos != string::npos) {
            bool append = (comando.find(">>") != string::npos);
            string cmd = comando.substr(0, redir_pos);
            string archivo = comando.substr(append ? redir_pos + 2 : redir_pos + 1);
            trim(cmd);
            trim(archivo);

            vector<char*> args = parse_command(cmd);
            ejecutar_redireccion(args, archivo, append);
            liberar_args(args);
            continue;
        }

<<<<<<< HEAD
        // COMANDO NORMAL
=======
        // ------------------------------------
        //  COMANDO NORMAL
        // ------------------------------------
>>>>>>> a97bba6 (Update versión final)
        vector<char*> args = parse_command(comando);
        ejecutar(args, en_fondo);
        liberar_args(args);
    }

    return 0;
}


/*
=======================================================
CASOS DE PRUEBA RECOMENDADOS
=======================================================

COMANDOS NORMALES:
  minishell> ls
  minishell> pwd
  minishell> echo Hola Mundo

RUTAS ABSOLUTAS:
  minishell> /bin/ls
  minishell> /usr/bin/whoami

PIPE:
  minishell> ls | grep cpp
  minishell> cat /etc/passwd | wc -l

REDIRECCIÓN:
  minishell> ls > lista.txt
  minishell> echo Hola >> lista.txt

SEGUNDO PLANO:
  minishell> sleep 5 &
  [Proceso en segundo plano] PID: XXXX

SALIDA:
  minishell> salir
  Saliendo de minishell...
*/
