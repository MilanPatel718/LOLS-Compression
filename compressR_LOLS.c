#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>


int main(int argc, char *argv[]){
	
	if(access(argv[1], F_OK)!=-1 && access(argv[1], R_OK)!=-1){}
	else{
		if(access(argv[1], F_OK)==-1){
			printf("Error: File does not exist\n");
			exit(0);
		}
		else{
			printf("Error: File has no read permission\n");
			exit(0);
		}
	}
	//Check if number of parts is actually a digit
	int len=strlen(argv[2]);
	int i;
	for(i=0;i<len;i++){
		if(isdigit(*(argv[2]+i)) == 0){
			printf("Error: Number of parts is not a digit\n");
			exit(EXIT_FAILURE);
		}
	}
				
	int k=atoi(argv[2]);
	if(k<1){
		printf("Error: Number of Parts must be 1 or {Greater\n");
		exit(EXIT_FAILURE);
	}
	
	/* Malloc appropriately sized array that will store the following arguments for execvp():
	1)Worker Process Code Executable
	2)Original File Path
	3)Number of Parts
	4)Part Number Being Compressed
	5)Null Teriminating String */
	
	char ** strings=(char **)malloc(5*sizeof(char *));
	
	//Store Worker Process Code Executable
	strings[0]="./compressR_worker_LOLS";
	
	//Store Original File Name
	strings[1]=argv[1];
	
			
				
	//Store k, number of parts to compress
	strings[2]=(char *)malloc(k*sizeof(char));
	int a =sizeof(strings[2]);
	snprintf(strings[2], a, "%d", k);
	
	pid_t pid;		
	//Use loop to create array of substrings that will be passed on to child processes
	for(i=0 ; i<k ; i++){
				
		//Store Part Number Being Compressed
				
		strings[3]=(char *)malloc(i*sizeof(char));
		int x=0;
		if(k==1){
		strings[3]="N";
		}
				
		else{
		x=sizeof(strings[3]);
		snprintf(strings[3], x , "%d", i);
		}
				
		//Store Null Terminator to argument array for execvp
		strings[4]='\0';
				
		//Fork Worker Process 
		//int status;	
		pid=fork(); 
				
		/*Parent Process
		if(pid>0){
       			if ((pid = wait(&status)) == -1)
                    	perror("wait error");
        		else {                
           			if (WIFSIGNALED(status) != 0)
              				printf("Child process ended because of signal %d\n",
                      			WTERMSIG(status));
                      				
           			else if(WIFEXITED(status) != 0){}
		
                     				 
           			else
              				printf("Child process did not end normally\n");
              					
       			}
     
		}*/
				
		//Child Process
		if(pid==0){
			execvp("./compressR_worker_LOLS", strings);
		}
		
		//Parent Process			
		else if(pid>0){}
				
		
		//Forking Failed
			else{
				printf("Error: Forking Failed");
				exit(EXIT_FAILURE);	
			}			
		}
		
		//Wait on all forked processes to finish
		while((pid = waitpid(-1, NULL, 0))) {
   			if (errno == ECHILD) {
      			break;
	  	 }
}
	return 0;
}

	
