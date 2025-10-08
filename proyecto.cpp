#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <unistd.h>     
#include <sys/wait.h>   
#include <cstring>      

using namespace std;

vector<char*> parse_command(string input){
    vector<char*> args;
    char *token = strtok(&input[0], " ");
    while(token != nullptr){
        args.push_back(token);
        token = strtok(nullptr, " ");
    }
    args.push_back(nullptr); 
    return args;
}

int main() {
    string comando;

    //ejecución: minishell> comando que se quiere ejecutar
    while(true){
        cout << "minishell> ";
        if(!getline(cin, comando)){
            break;
        }

        if (comando.empty()) continue;

        if (comando == "salir"){
            cout << "saliendo de la mini-shell...\n";
            break;
        }

        vector<char*> args = parse_command(comando); //funcion parse para el comando

        pid_t pid = fork();

        if (pid == 0){
            if (execvp(args[0], args.data()) == -1){
                perror("Error al ejecutar comando");
            }
            exit(EXIT_FAILURE);
        } else if(pid > 0){
            int status;
            waitpid(pid, &status, 0);
        } else{
            perror("Error al crear proceso");
        }
    }

    return 0;
}

/*
Comandos ejecutables (siendo o no una dirección absoluta):
ls
pwd
echo
salir
*/
