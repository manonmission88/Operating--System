// Worked in a group , Team Member : Prem Oli
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;
int flag;
pid_t pid;
int command_p = -1;

void execute_command(char* arguments[]);
void parse(char* token);

// process termination function
void terminate_process(int time, int pid) {
    sleep(time);
    printf("Time out foreground process.\n");
    kill(pid, SIGINT);
}
// signal handling
void signal_handler(int signum)
{
    if (command_p != -1){
        kill(command_p, SIGINT);
    }
}

int main() {
    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
    size_t wd_size = 400;
    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_ARGS];
    char wd[wd_size];
    char *cwd;
    char cmd[256];
    signal(SIGINT,signal_handler);
    while (true) {

        do{
            fflush(stdout);
            // Print the shell prompt.
            //geting the current working directory
            cwd = getcwd(wd, wd_size);
            printf("%s %s", cwd, prompt);
            fflush(stdout);

            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")

            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
            }


        }while(command_line[0] == 0x0A);  // while just ENTER pressed


        // 1. Tokenize the command line input (split it on whitespace)
        //saves the first string of the command_line string if not NULL
        arguments[0] = strtok(command_line, delimiters);
        int i = 0;
        while (arguments[i] != NULL){
            parse(arguments[i]);
            i++;
            arguments[i]= strtok(NULL, delimiters);
            // printf("Comad %s\n",arguments[i]);
        }

        // printf("CLicked %s\n",arguments[0]);
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }
        char* last_arg = arguments[i-1];
        bool background_process = false;
        if (strcmp(last_arg,"&")==0){
            background_process = true;
            arguments[i-1] = NULL;
        }

        // 2. Implement Built-In Commands
        if (strcmp(arguments[0],"cd")==0){
            // printf("doneeee %s",arguments[1]);
            chdir(arguments[1]);
        } else if (strcmp(arguments[0],"pwd")==0){
            printf("%s\n",getcwd(wd,sizeof(wd)));
        } else if (strcmp(arguments[0],"echo")==0){
            i = 1;
            while(arguments[i] != NULL){
                if (arguments[i][0] == '$'){
                    char *value = getenv(arguments[i]+1);
                    if(value != NULL) {
                        printf("%s ", value);
                    } else {
                        printf("null ");
                    }
                }
                else{
                    printf("%s ", arguments[i]);
                }
                i++;
                printf("\n");

            }


        } else if (strcmp(arguments[0],"env")==0){
            if (arguments[1] != NULL){
                printf("%s\n",getenv(arguments[1]));
            } else {
                char** env= environ;
                for (; *env; env++)
                    printf("%s\n", *env);
            }
        } else if (strcmp(arguments[0],"setenv")==0) {
            char* temp_val[2];
            temp_val[0] = strtok(arguments[1], "=");
            i=0;
            while(temp_val[i] != NULL || i == 2) {
                i++;
                temp_val[i] = strtok(NULL, "=");
            }
            setenv(temp_val[0], temp_val[1], 1);
        } else if (strcmp(arguments[0],"exit")==0){
            exit(0);
        } else {
            int pid = fork();
            if (pid == 0){
                signal(SIGINT, SIG_DFL);
                execute_command(arguments);
                exit(0);
            }
            else{
                if (background_process){
                    command_p = -1;
                }
                else {
                    command_p = pid;
                    int k = fork();
                    if (k==0){
                        signal(SIGINT, SIG_DFL);
                        terminate_process(10000, pid);
                        exit(0);
                    }
                    else{
                        waitpid(pid, NULL, 0);
                        kill(k, SIGINT);
                        waitpid(k, NULL, 0);
                    }
                }
            }

        }

    }
    // This should never be reached.

    return -1;
}

// extra credit
void parse(char* token){
    bool quotes = false;
    char first;
    char last;
    if (token[0]=='\"' || token[0]=='\''){
        first = token[0];
        quotes = true;
    }
    if (quotes){
        int i=0;
        while (token[i]!='\0'){
            last = token[i];
            i++;
        }
        if (first == last){
            i=1;
            while (token[i]!='\0'){
                token[i-1] = token[i];
                i++;
            }
            token[i-1]='\0';
            token[i-2]='\0';
        }
    }

}

// for extra credit -- io operation and pipping out from one process to another
void execute_command(char* arguments[]){
    char* args_by_pipe[MAX_COMMAND_LINE_ARGS][MAX_COMMAND_LINE_ARGS];
    char* file;
    int fd;
    int i = 0;
    int command_count = 0;
    int command_token_count = 0;

    // Process arguments to detect pipes and I/O redirection
    while (arguments[i] != NULL) {
        // Output redirection
        if (strcmp(arguments[i], ">") == 0) {
            arguments[i] = NULL;
            file = arguments[++i];
            fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);

            // Input redirection
        } else if (strcmp(arguments[i], "<") == 0) {
            arguments[i] = NULL;
            file = arguments[++i];
            fd = open(file, O_RDONLY);
            if (fd < 0) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);

            // Command piping
        } else if (strcmp(arguments[i], "|") == 0) {
            if (command_token_count == 0) {
                printf("Invalid pipe command\n");
                return;
            }
            args_by_pipe[command_count][command_token_count] = NULL;
            command_count++;
            command_token_count = 0;
        } else {
            args_by_pipe[command_count][command_token_count++] = arguments[i];
        }
        i++;
    }

    int num_pipes = command_count;
    command_count++;
    // If no pipes, directly execute the command
    if (num_pipes == 0) {
        execvp(args_by_pipe[0][0], args_by_pipe[0]);
        return;  // Return in case execvp fails

    } else{
        int pipes[num_pipes][2];
        for (i=0;i<num_pipes;i++){
            pipe(pipes[i]);
        }
        int pids[command_count];
        for (i=0;i<command_count;i++){
            pids[i]=fork();
            if (pids[i] == 0){
                int j;
                if (i == 0){
                    dup2(pipes[0][1], 1);
                    close(pipes[0][0]);
                    for (j=1; j < num_pipes; j++){
                        close(pipes[j][0]);
                        close(pipes[j][1]);
                    }
                }
                else if (i == num_pipes){
                    dup2(pipes[num_pipes-1][0], 0);
                    close(pipes[num_pipes-1][1]);
                    for (j=0; j < num_pipes-1; j++){
                        close(pipes[j][0]);
                        close(pipes[j][1]);
                    }
                }
                else{
                    dup2(pipes[i-1][0],0);
                    dup2(pipes[i][1],1);
                    for (j=0; j < num_pipes; j++){
                        if (j== i-1){
                            close(pipes[j][1]);
                        }
                        else if (j == i){
                            close(pipes[j][0]);
                        }
                        else{
                            close(pipes[j][0]);
                            close(pipes[j][1]);
                        }
                    }
                }
                execvp(args_by_pipe[i][0],args_by_pipe[i]);
            }
            else if (pids[i] < 0){
                printf("An error occured\n");
                return;
            }
        }
        for (i=0; i<num_pipes; i++){
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
        for(i = 0; i < command_count; i++){
            wait(NULL);
        }

    }
}



