#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>

char ** parse_args(char * line, char * d, int size);
void errcheck();
void redirect_out(char **, int initial, int size);

int main() {
 int size = 8;
 char input[256];
 char ** commands;
 while (1) {
   char dir[256];
   getcwd(dir, sizeof(dir));
   printf("%s$ ", dir);
   fgets(input, sizeof(input) - 1, stdin);
   errcheck();
   if (input[strlen(input) - 1] == '\n') input[strlen(input) - 1] = '\0';
   commands = parse_args(input, ";", size);
   int i = 0;
   while (commands[i]){
     if (strcmp(commands[i], "exit") == 0) return 0; // exit
     char ** args;
     char p[256];
     strcpy(p, commands[i]);
     args = parse_args(p, " ", size);
     if (strcmp(args[0], "cd") == 0){ // cd
       if (args[2] != NULL) printf("cd: too many arguments\n");
       else if (!args[1]){
         chdir(getenv("HOME"));
       }
       else{
         chdir(args[1]);
         if (errno){
           printf("cd: %s: No such file or directory\n", args[1]);
           errno = 0;
         }
       }
     }
     else{
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
         int j = 0; // check for redirection
         int redirected = 0;
         char ** riarr = parse_args(commands[i], "<", size);
         if (riarr[1]){
           redirected = 1;
           char ** left = parse_args(riarr[0], " ", size);
           char ** right = parse_args(riarr[1], ">", size);
           int fd = open(right[0], O_RDONLY);
           int fd2;
           int nfd = dup(0);
           int nfd2;
           dup2(fd, 0);
           if (right[1]){
             redirected = 2;
             fd2 = open(right[1], O_WRONLY | O_TRUNC | O_CREAT, 0644);
             nfd2 = dup(1);
             dup2(fd2, 1);
           }
           if(!fork()){
             execvp(left[0], left);
             if (errno) printf("%s: command not found\n", left[0]);
             return 0;
           }
           else{
             int status;
             wait(&status);
             dup2(nfd, 0);
             if (right[1]){
               dup2(nfd2, 1);
               close(fd2);
             }
           }
           close(fd);
           free(left);
           if (redirected == 2) redirect_out(right, 1, size);
           free(right);
           return 0;
         }
         free(riarr);
         if (!redirected) {
           redirected = 1;
           char ** roarr = parse_args(commands[i], ">", size);
           redirect_out(roarr, 0, size);
           free(roarr);
           return 0;
         }
         char ** rarr = parse_args(commands[i], "|", size); // cannot mix with > or <. Cannot chain
         if (rarr[2]) {
           redirected = 1;
           printf("Unsupported\n");
         }
         else if (rarr[1]) {
           redirected = 1;
           FILE * p = popen(rarr[0], "r");
           FILE * q = popen(rarr[1], "w");
           char buff[2048];
           char c = fgetc(p);
           int x = 0;
           while (c != EOF){
             buff[x] = c;
             x++;
             c = fgetc(p);
           }
           fwrite(buff, 1, strlen(buff), q);
         }
         if (!redirected) execvp(args[0], args);
         if (errno) printf("%s: command not found\n", args[0]);
         return 0;
       }
       else
       {
         /* error */
         exit(EXIT_FAILURE);
       }
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
   char * s = strsep(&line, d);
   if (strcmp(d, " ") == 0){ // remove extraneous spaces
     if(s[0] != '\0') {
       arr[i] = s;
       i++;
     }
   }
   else{
     arr[i] = s;
     i++;
   }
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

void redirect_out(char ** arr, int initial, int size){ // handles > and chain
  int x = initial;
  while (arr[x + 1]){
    char p[256];
    strcpy(p, arr[x + 1]);
    char ** left = parse_args(arr[x], " ", size);
    char ** right = parse_args(p, " ", size);
    int fd = open(right[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (x > 0){
      int fd0 = open(left[0], O_RDONLY);
      char buffer[2048];
      read(fd0, buffer, 2048);
      errcheck();
      write(fd, buffer, strlen(buffer));
      errcheck();
      close(fd0);
    }
    else{
      int nfd = dup(1);
      dup2(fd, 1);
      if(!fork()){
        execvp(left[0], left);
        if (errno) printf("%s: command not found\n", left[0]);
        exit(EXIT_FAILURE);
      }
      else{
        int status;
        wait(&status);
        dup2(nfd, 1);
      }
    }
    close(fd);
    free(left);
    free(right);
    x++;
  }
}
