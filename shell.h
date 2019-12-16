#ifndef SHELL_H
#define SHELL_H
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif

char ** parse_args(char * line, char * d, int size);
void errcheck();
void redirect_out(char **, int initial, int size);
