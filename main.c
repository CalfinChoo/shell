#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>

char ** parse_args(char * line, char * d, int size);
void errcheck();

int main() {
 int size = 8;
 char input[256];
 char ** commands;
 while (1) {
   char dir[1024];
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
         char ** roarr = parse_args(commands[i], ">", size);
         char ** riarr = parse_args(commands[i], "<", size);
         if (roarr[1] || riarr[1]){
           redirected = 1;
           int j = 0;
           while (roarr[j + 1] || riarr[j + 1]){ // cannot chain <, but can chain >. Cannot combine < and >
             char ** left;
             char ** right;
             int fd;
             int std;
             if (roarr[j + 1]){
               left = parse_args(roarr[j], " ", size);
               right = parse_args(roarr[j + 1], " ", size);
               fd = open(right[0], O_WRONLY | O_CREAT, 0644);
               std = 1;
             }
             else {
               left = parse_args(riarr[j], " ", size);
               right = parse_args(riarr[j + 1], " ", size);
               fd = open(right[0], O_RDONLY);
               std = 0;
             }
             if (j > 0 && roarr[j + 1]){
               int fd0 = open(left[0], O_RDONLY);
               int bsize = 1024;
               char buffer[bsize];
               read(fd0, buffer, bsize);
               errcheck();
               write(fd, buffer, bsize);
               errcheck();
               close(fd0);
             }
             else{
               int nfd = dup(std);
               dup2(fd, std);
               if(!fork()){
                 execvp(left[0], left);
                 if (errno) printf("%s: command not found\n", args[0]);
                 return 0;
               }
               else{
                 int status;
                 wait(&status);
                 dup2(nfd, std);
               }
             }
             close(fd);
             free(left);
             free(right);
             j++;
           }
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
