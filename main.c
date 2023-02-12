#include "nano_shell.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



/*
List of builtin commands, followed by their corresponding functions. 
*/ 

char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &nano_cd,
    &nano_help,
    &nano_exit
};

int nano_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int main(int argc, char **argv) {
    // Load config files
    
    // Run loop
    nano_loop();

    // Shutdown and cleanup

    return EXIT_SUCCESS;
}

void nano_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("$ ");
        line = nano_read_line();
        args = nano_split_line(line);
        status = nano_execute(args);

        free(line);
        free(args);
    } while(status);
}

#define RL_BUFFER_SIZE 1024
char *nano_read_line(void) {
    int bufsize = RL_BUFFER_SIZE;
    int pos = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    // c is an int so we can store EOF, but is used to store chars
    int c;

    if (buffer == NULL) {
        fprintf(stderr, "nano: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();
        // null terminate upon EOF or \n
        if (c == EOF || c == '\n') {
            buffer[pos] = '\0';
            return buffer;
        }
        else {
            buffer[pos] = c;
        }
        pos++;

        if (pos >= bufsize) {
            bufsize += RL_BUFFER_SIZE;
            buffer = realloc(buffer, bufsize);
            if(buffer == NULL) {
                fprintf(stderr, "nano: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

#define TOKEN_BUFFER_SIZE 64
#define TOKEN_DELIMS " \t\r\n\a"
char **nano_split_line(char *line) {
    int bufsize = TOKEN_BUFFER_SIZE, pos = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (tokens == NULL) {
        fprintf(stderr, "nano: allocation error");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOKEN_DELIMS);
    while (token != NULL) {
        tokens[pos] = token;
        pos++;

        if (pos >= bufsize) {
            bufsize += TOKEN_BUFFER_SIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (tokens == NULL) {
                fprintf(stderr, "nano: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOKEN_DELIMS);
    }
    tokens[pos] = NULL;
    return tokens;
}

int nano_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("nano");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0) {
        // Error forking
        perror("nano");
    }
    else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

// Built-in function implementations

int nano_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "nano: expected arguments to \"cd\"\n");
    }
    else {
        if (chdir(args[1]) != 0) {
            perror("nano");
        }
    }
    return 1;
}

int nano_help(char **args) {
    int i;
    printf("Welcome to the Nano shell.\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < nano_num_builtins(); i++) {
        printf("   %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int nano_exit(char **args) {
    return 0;
}


int nano_execute(char **args) {
    int i;
    if (args[0] == NULL) {
        return 1;
    }

    for(i = 0; i < nano_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    return nano_launch(args);
}


















