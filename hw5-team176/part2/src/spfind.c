/*
* Sorts the output of pfind
* Author: Stephen Connelly, Benjamin Alexander 
* ID: Sjc2235, Baa2165
*An executable spfind that sorts the output from pfind
*
*
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h> 

#define BUFSIZE 1024

int main(int argc, char *argv[]){

    int pfind_to_sort[2], sort_to_spfind[2];
    pipe(pfind_to_sort);
    pipe(sort_to_spfind);
	    
    pid_t pid[2];
    
    //pfind
    if((pid[0] = fork())==0){
        close(pfind_to_sort[0]); 
        dup2(pfind_to_sort[1], STDOUT_FILENO); 
        close(pfind_to_sort[1]);  
	    //unrelated
        close(sort_to_spfind[0]);
        close(sort_to_spfind[1]);

        if (execv("pfind", argv) == -1) {
            fprintf(stderr, "Error: pfind failed.");
            return EXIT_FAILURE;
        }
	    // execv("pfind", argv);
	    perror("execv");
        exit(EXIT_FAILURE);
    }
    waitpid(pid[0], NULL, 0);
    
    //sort
    if((pid[1] = fork())==0){
        close(pfind_to_sort[1]); 
        dup2(pfind_to_sort[0], STDIN_FILENO); 
        close(pfind_to_sort[0]); 

        close(sort_to_spfind[0]);  
        dup2(sort_to_spfind[1], STDOUT_FILENO);
        close(sort_to_spfind[1]); 

        if (execlp("sort", "sort", NULL) == -1) {
            fprintf(stderr, "Error: sort failed");
        }
        perror("execlp");
        exit(EXIT_FAILURE);
    }
    close(pfind_to_sort[0]);
    close(pfind_to_sort[1]); 
    close(sort_to_spfind[1]);

    int br;
    char buf[BUFSIZE];
   
    while ((br = read(sort_to_spfind[0], buf, sizeof(buf))) > 0) {
        if (write(STDOUT_FILENO, buf, br) == -1) {
            fprintf(stderr, "Write failed: %s\n", strerror(errno));
        }
    }
    
    close(sort_to_spfind[0]);
   
 
}   
