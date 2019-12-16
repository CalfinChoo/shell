#include "shell.h"

/*======== int main() ====================
Inputs: None (at least not at runtime)
Returns: 0

Runs the shell!
Prints current working directory
Prints the command being run (mainly to aid in readability when redirecting stdin to the program)
Checks for user inputs
====================*/
int main() {
  int size = 8;
  char input[1024];
  char ** commands;
  while (1) {
    char dir[256];
    getcwd(dir, sizeof(dir));
    printf("%s$ ", dir);
    fgets(input, sizeof(input) - 1, stdin);
    errcheck();
    if (input[strlen(input) - 1] == '\n') input[strlen(input) - 1] = '\0';
    commands = parse_args(input, ";", size);
    printf("%s\n", input);
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
          int redirected = 0;// check for redirection
          char ** riarr = parse_args(commands[i], "<", size);
          if (riarr[1]){
            redirected = 1;
            char ** left = parse_args(riarr[0], " ", size);
            char ** right = parse_args(riarr[1], ">", size);
            char ** right1 = parse_args(right[0], " ", size);
            char ** right2;
            int fd = open(right1[0], O_RDONLY);
            int fd2;
            int nfd = dup(0);
            int nfd2;
            dup2(fd, 0);
            if (right[1]){
              redirected = 2;
              char q[256];
              strcpy(q, right[1]);
              right2 = parse_args(q, " ", size);
              fd2 = open(right2[0], O_WRONLY | O_TRUNC | O_CREAT, 0644);
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
                free(right2);
                dup2(nfd2, 1);
                close(fd2);
              }
            }
            free(right1);
            close(fd);
            free(left);
            if (redirected == 2) redirect_out(right, 1, size);
            free(right);
            return 0;
          }
          free(riarr);
          char ** roarr = parse_args(commands[i], ">", size);
          if (roarr[1]) {
            redirected = 1;
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
    errcheck();
  }
  return 0;
}
