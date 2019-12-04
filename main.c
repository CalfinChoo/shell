#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

void parse_args(char * line, char *** args);
void errcheck();

int main() {
  char input[1024];
  char args[8][8][16];
  while (1) {
    printf("Command:\n");
    fgets(input, 735, stdin);
    if (input[strlen(input) - 1] == '\n') input[strlen(input) - 1] = '\0';
    errcheck();
    parse_args(input, args);
    int i = 0;
    while (args[i]){
      int f = fork();
      if (strcmp(args[i][0], "exit") == 0){
        printf("exiting\n\n");
        return 0;
      }
      if (f){
        int status = 0;
        wait(&status);
      }
      else{
        execvp(args[i][0], args[i]);
      }
      errcheck();
    }
  }
  return 0;
}

void parse_args(char * line, char *** args) {
  int i = 0;
  while(line != NULL) {
    args[i][0] = strsep(&line, ";");
    i++;
  }
  args[i] = NULL;
  i--;
  for (; i >= 0; i--){
    int j = 0;
    char * l = args[i][0];
    while(l){
      args[i][j] = strsep(&l, " ");
      j++;
    }
    args[i][j] = NULL;
  }
}

void errcheck(){
  if (errno) {
    printf("Error: %d - %s\n", errno, strerror(errno));
    errno = 0;
  }
}
