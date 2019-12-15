# Shell
### by Anthony Li and Calvin Chu

## Features:
  - Forks and executes commands
  - Parses multiple commands on one line
  - Simple redirects using >, < (can chain multiple >, and one < with multiple >.)
  - Allows simple pipes (e.g. "ls | wc")
  - Can change directories (but no ~)
  - Accounts for extraneous spaces in inputs (e.g. " ls      -a  -l")
  - Error messages!

## Attempted:
  - Could not get chaining pipes to work

## Bugs:
  - Pest Control said it should be clean

## Files & Function Headers:
<b>main.c</b> <br>
Handles all line parsing, redirection, chaining, forking, executing - you name it.  

#### /*======== int main() ====================
<b>Inputs:</b> None (at least not at runtime) <br>
<b>Returns:</b> 0

  - Runs the shell!
  - Prints current working directory
  - Prints the command being run (mainly to aid in readability when redirecting stdin to the program)
  - Checks for user inputs

####	====================*/
#### /*======== char * * parse_args() ==========
<b>Inputs:</b>
  - char * line
  - char * d
  - int size <br>

<b>Returns:</b> Array of strings from the given string parsed by the given delimeter

  - Parses line by delimiter d and separates it into multiple strings stored in an array that is returned
  - Checks for and removes any extraneous spaces in line, if " " was the delimeter.

####	====================*/

####	/*======== void redirect_out() ==========
<b>Inputs: </b>
  - char * * arr
  - int initial
  - int size <br>

<b>Returns:</b> None

  - Function for handling ">" redirect and chaining redirects

####	====================*/

####	/*======== void errcheck() ==========
  <b>Inputs:</b> None <br>
	<b>Returns:</b> None
- Prints an errno message when called and resets errno.

####	====================*/
