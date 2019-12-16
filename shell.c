#include "shell.h"

/*======== char * * parse_args() ==========
Inputs:

char * line
char * d
int size
Returns: Array of strings from the given string parsed by the given delimeter

Parses line by delimiter d and separates it into multiple strings stored in an array that is returned
Checks for and removes any extraneous spaces in line, if " " was the delimeter.
====================*/
char ** parse_args(char * line, char * d, int size) { // up to size - 1 commands/args
  char ** arr = malloc(size * sizeof(char *));
  if (line[0] == '\0'){
    arr[0] = NULL;
    return arr;
  }
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

/*======== void errcheck() ==========
Inputs: None
Returns: None

Prints an errno message when called and resets errno.
====================*/
void errcheck(){
  if (errno) {
    printf("Error: %d - %s\n", errno, strerror(errno));
    errno = 0;
  }
}

/*======== void redirect_out() ==========
Inputs:

char * * arr
int initial
int size
Returns: None

Function for handling ">" redirect and chaining redirects
====================*/
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
