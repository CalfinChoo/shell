#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

char ** parse_args( char * line );

int main() {
  char input[256];
  char *args[16][16];
  while (1) {

  }
  return 0;
}

void parse_args(char * line, char *** args) {
  int i = 0;
  while(line != NULL) {
    args[i] = strsep(&line, ";");
    i++;
  }
  args[i] = NULL;
  i--;
  for (; i >= 0; i--){
    int j = 0;
    char * l = args[i];
    while(l){
      args[i][j] = strsep(&l, " ");
      j++;
    }
  }
}
