<unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

char ** parse_args(char * line, char * d, int size);
void errcheck();

int main() {
 int size = 8;
 char input[256];
 char ** commands;
 //char ** curr[16];
 while (1) {
   printf("$ ");
   fgets(input, sizeof(input) - 1, stdin);
   errcheck();
   if (input[strlen(input) - 1] == '\n') input[strlen(input) - 1] = '\0';
   commands = parse_args(input, ";", size);
   int i = 0;
   while (commands[i]){
     if (strcmp(commands[i], "exit") == 0) return 0;
     char ** args;
     args = parse_args(commands[i], " ", size);
     pid_t pid = fork();
     if (pid > 0) {
     /* parent process */
       int status;
       wait(&status);
       errcheck();
       free(args);
     }
     else if (pid == 0){
       /* child process. */
       execvp(args[0], args);
       if (errno) printf("%s: command not found\n", args[0]);
       return 0;
     }
     else
     {
       /* error */
       exit(EXIT_FAILURE);
     }
     errcheck();
     i++;
   }
   free(commands);
 }
 return 0;
}

char ** parse_args(char * line, char * d, int size) { // up to size - 1 commands/args
 char ** arr = malloc(size * sizeof(char *));
 int i = 0;
 while(line != NULL && i < size - 1) {
   arr[i] = strsep(&line, d);
   i++;
 }
 arr[i] = NULL;
 return arr;
}

void errcheck(){
 if (errno) {
   printf("Error: %d - %s\n", errno, strerror(errno));
   errno = 0;
 }
}
