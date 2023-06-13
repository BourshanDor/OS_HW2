#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define PIPE 1
#define BACKGROUND 2
#define REDIRECTION 3

static void handler(int signal){
	if (signal == SIGINT){
		if (write(1,"\n",1) < 0 ) { 
			perror("Error occurs in a signal handler in the shell parent process1: ");
			exit(1);
		}
		return; 
	}
}

// arglist - a list of char* arguments (words) provided by the user
// it contains count+1 items, where the last item (arglist[count]) and *only* the last is NULL
// RETURNS - 1 if should continue, 0 otherwise
int process_arglist(int count, char** arglist){

	int status;
	char *vertical_bar = "|" ; 
	char *ampersand = "&" ;
	char *lass_then = "<" ;
	int functionality = -1; 
	int i; 

	for (i = 0; i < count ; i ++){
		if (strcmp(vertical_bar, arglist[i]) == 0) { 
			functionality = PIPE ; 
			arglist[i] = NULL; 
			break;
		}
		if (strcmp(ampersand, arglist[i]) == 0) { 
			functionality = BACKGROUND ; 
			break;
		}
		if (strcmp(lass_then, arglist[i]) == 0) { 
			functionality = REDIRECTION ; 
			break;
		}
	}

	switch (functionality){
		case PIPE :
			{
				int pipefd[2] ; 
				if (-1 == pipe(pipefd)) {
				// need to check the error section  ****
					perror("Error in pipe: ");
					exit(1);
				}

				pid_t first_child_pid = fork();

				if (first_child_pid < 0 ) {
					// need to check the error section ****
					perror("Error in fork: ");
					close(pipefd[0]);
					close(pipefd[1]);
					exit(1);
				}
				
				if (first_child_pid == 0 ) { 
					// first child 
					// close reader 
					close(pipefd[0]); 
					if(dup2(pipefd[1], 1) < 0){
						perror("Error occur in dup2 section, first child : ");
						close(pipefd[1]); 
						exit(1); 
					}
					// signal(SIGCHLD, SIG_DFL);
					execvp(arglist[0], arglist); 
					perror("Error occur in execvp section, first child : ");
					close(pipefd[1]); 
					exit(1); 
			
				}
				else{
					// parent 
					pid_t second_child_pid = fork();
					if (second_child_pid == 0) {
						// second child 
						// close write 
						close(pipefd[1]); 
						if(dup2(pipefd[0], 0) < 0){
							perror("Error occur in dup2 section, second child, PIPE section : ");
							close(pipefd[0]); 
							exit(1); 
						}
						// signal(SIGCHLD, SIG_DFL);
						execvp(arglist[i + 1], arglist + (i + 1)); 
						perror("Error occur in execvp section, second child, at PIPE section : ");
						close(pipefd[0]); 
						exit(1); 
					}
					else{
						close(pipefd[0]);
						close(pipefd[1]);
						
						waitpid(first_child_pid, &status,0);
						waitpid(second_child_pid, &status,0); 
						if (!(errno == ECHILD || errno == EINTR )){
							perror("Error in waitpid : "); 
							exit(1); 
						}
					}
				}			
				break;
			}

		case BACKGROUND :
		{
			arglist[i] = NULL;
			int pid = fork(); 
			if (pid == 0 ) { 
				// child 
				signal(SIGINT, SIG_IGN); 
				execvp(arglist[0], arglist); 
				perror("The error is : ");
			}
			break;	
		}
		case REDIRECTION :
		{
			int fd = open(arglist[i + 1], O_RDONLY); 
			if (fd < 0 ) {
				perror("Error happend when try open a file : "); 
				exit(1); 
			}
			
			int pid = fork(); 
			if (pid == 0 ) { 
				// child 
				if(dup2(fd, 0) < 0){
					perror("Error occur in dup2 section, second child, PIPE section : ");
					close(fd); 
					exit(1); 
				}
				arglist[i] = NULL; 
				execvp(arglist[0], arglist); 
				perror("Error in REDIRECTION, child, execvp: ");
				exit(1); 
			}
			else{
				// parent 
				
				close(fd); 
				while( -1 != waitpid(pid, &status, 0) ); 
				if (!(errno == ECHILD || errno == EINTR )){
						perror("Error in waitpid : "); 
						exit(1); 
				}
			}
			break;	
		}

		default:
		{
			int pid = fork(); 
			if (pid == 0 ) { 
				// child
				execvp(arglist[0], arglist); 
				perror("The error is : ");
			}
			else{
				while( -1 != waitpid(pid, &status, 0) ); 
				if (!(errno == ECHILD || errno == EINTR )){
						perror("Error in waitpid : "); 
						exit(1); 
				}
			}
			break;	
		}
	}
	return 1; 
}
	
// prepare and finalize calls for initialization and destruction of anything required
int prepare(void){
	
	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask); 
	action.sa_flags = SA_RESTART;
	if (sigaction(SIGINT , &action, NULL) == -1 ) {
		perror("Error in prepare : "); 
		exit(1); 
	} 
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR){
		perror("Error in prepare : "); 
		exit(1); 
	}
	return 0;
}
int finalize(void){
	return 0; 
}