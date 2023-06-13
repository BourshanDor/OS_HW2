# Shell Program Implementation
## Introduction
The purpose of this project was to gain experience in process management, pipes, signals, and system calls by implementing a simple shell program. The shell program provides various functionalities, including executing commands, executing commands in the background, single piping, and input redirecting.

## Shell Functionality

The implemented shell supports the following operations:

1. **Executing commands:** Users can enter a command, consisting of a program and its arguments, such as ```sleep 10``` . The shell executes the command and waits until it completes before accepting another command.
2. **Executing commands in the background:** Users can enter a command followed by ```&```, for example: ```sleep 10 &```. The shell executes the command but does not wait for its completion before accepting another command.
3. **Single piping:** Users can enter two commands separated by the pipe symbol ```|```, for example: ```cat foo.txt | grep bar```. The shell executes both commands concurrently, piping the standard output of the first command to the standard input of the second command. The shell waits until both commands complete before accepting another command.
4. **Input redirecting:** Users can enter a command and an input file name separated by the redirection symbol ```<```, for example: ```cat < file.txt```. The shell executes the command with its standard input redirected from the input file instead of the default user's terminal. The shell waits for the command to complete before accepting another command.

### Execute 

```gcc -O3 -D_POSIX_C_SOURCE=200809 -Wall -std=c11 shell.c myshell.c -o myshell```


