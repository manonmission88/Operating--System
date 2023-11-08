#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

/**
 * Executes the command "cat scores | grep Lakers | sort". In this quick-and-dirty
 * implementation, the parent waits for the child processes to finish.
 */

int main(int argc, char **argv)
{
    char* grep_arg = argv[1];

    int pipefd[2], pid;

    // Create a pipe (file descriptors go in pipefd[0] and pipefd[1)
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "pipe failed!");
        return 1;
    }
    char *cat_args[] = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", grep_arg, NULL};

    // Create the first child process to handle "cat scores"
    pid = fork();

    if (pid == 0)
    {
        int pipefd1[2], pid1;
        // Create a second pipe (file descriptors go in pipefd1[0] and pipefd1[1])
        if (pipe(pipefd1) == -1) {
            fprintf(stderr, "pipe failed!");
            return 1;
        }
        char *sort_args[] = {"sort", NULL};

        // Close the writing end of the first pipe
        close(pipefd[1]);

        // Create the second child process to handle "sort"
        pid1 = fork();

        if (pid1 == 0) {
            // Child process handles "sort"
            // Replace standard input with the input part of pipe1
            dup2(pipefd1[0], 0);
            close(pipefd1[1]);
            // Execute the "sort" command
            execvp("sort", sort_args);
        }
        else {
            // Parent process handles "grep"
            // Close the reading end of the first pipe
            dup2(pipefd[0], 0);
            close(pipefd[1]);
            // Replace standard output with the output part of pipe1
            dup2(pipefd1[1], 1);
            close(pipefd1[0]);
            // Execute the "grep" command
            execvp("grep", grep_args);
        }
    }
    else {
        // Parent process handles "cat scores"
        // Modify the "scores" file to add a newline before "Houston"
        FILE *scores_file = fopen("scores", "r");
        FILE *temp_file = fopen("temp_scores", "w");

        if (scores_file && temp_file) {
            char line[256];
            while (fgets(line, sizeof(line), scores_file)) {
                if (strstr(line, "Houston") != NULL) {
                    fprintf(temp_file, "\n");
                }
                fputs(line, temp_file);
            }
            fclose(scores_file);
            fclose(temp_file);
            rename("temp_scores", "scores");
        }

        // Replace standard output with the output part of the pipe
        dup2(pipefd[1], 1);
        // Close the reading end of the first pipe
        close(pipefd[0]);
        // Execute the "cat" command
        execvp("cat", cat_args);
        exit(0);
    }

    // Parent process waits for all child processes to finish
    int status;
    while (wait(&status) > 0);

    return 0;
}
