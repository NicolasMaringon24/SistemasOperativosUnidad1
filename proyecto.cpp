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
