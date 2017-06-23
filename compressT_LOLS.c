#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <libgen.h>
#include <sys/types.h>

pthread_mutex_t lock1;
pthread_mutex_t lock2;
pthread_mutex_t lock3;

struct input{
	int fNum;
	};

char * CompressName(char * FilePath, int File){
	int i;
	char * FileNum=(char *)malloc(12);
	char* FileName=basename(FilePath);
	for(i=0; i<strlen(FileName) ; i++){
		if(FileName[i]=='.')
			FileName[i]='_';
	}
	snprintf(FileNum, 12, "%d", File);
	char* compName=(char *)malloc(sizeof(char)*(strlen(FileName)+strlen(FileNum)+10));
	compName[0]='\0';
	strcat(compName, FileName);
	strcat(compName, "_LOLS");
	
	if(File==-1){
		return compName;
	}
	
	else{
	strcat(compName, FileNum);
	}
	
	return compName;
}

int k;
char *FName;
int characters;

void* compressT_LOLS(void* params){

	//Open file in worker thread
	pthread_mutex_lock(&lock1);
	char *fileName=FName;
	struct input* thread_num=(struct input*)params;
	int partNum= thread_num->fNum;
	int n=characters;
	pthread_mutex_unlock(&lock1);
	
	FILE *f=fopen(fileName, "r");
		
	
	//User attempts to split file into parts greater than the size of the actual file 	
	if(k > n){
		printf("Error: String length %d is too small to split into %d part(s)\n", n+1, k);
		exit(EXIT_FAILURE);
		}
	
	int size;
	char * toComp;
	int m=n%k;
	
	if(partNum==0){
		//Get size to allocate for substring, rounding up
		size=(((int) floor((double)n/(double)k))+m);
		}
	
	else{
		//Get size to allocate for substring, rounding down
		size=(int) floor((double)n/(double)k);
		}
	
		//Set up component string to compress
		int s=(((int) floor((double)n/(double)k))+m);
		toComp=(char *)malloc(size*sizeof(char)+1);
			
		if(partNum==0){
			char * temp= (char *)malloc((sizeof(char)*size)+1);
			//Copy substring and move forward on input string
			int fd=fileno(f);
			lseek(fd, partNum*size, SEEK_SET);
			fread(temp, 1, size, f);
			temp[size]='\0';	
			//Store Component String to be Compressed
			toComp=temp;
		}
					
		else{
			
			char * temp= (char *)malloc((sizeof(char)*size)+1);
					
			//Copy substring and move forward on input string
			fseek(f, (s+((partNum-1)*size)) , SEEK_SET);
			fread(temp, 1, size, f);
			temp[size]='\0';		
			//Store Componenet String to be Compressed
			toComp=temp;
		}
	
	//Creates array of size toComp, which guarantees enough space albeit a little extra
	char* compressed=(char *)malloc((strlen(toComp)+1)*sizeof(char));
	compressed[0]='\0';
	
	//Loop through uncompressed string, finding repeating groups and compressing them	
	char * ptr=toComp;
	char * tmp;
	int count;
	int compCount;
	
	//Loop through string as long as in bounds
	while(ptr-toComp<strlen(toComp)){
		
		//If first character is alphabetic, do nothing
		if(isalpha(*ptr)!=0){}
		
		//Else loop through until an alphabetic is found, or return error that entire file is non-alphabetic
		else{
			while(isalpha(*ptr)==0){
				ptr++;
				if(ptr-toComp==strlen(toComp)){
					printf("Warning: Section is all non alpha, blank file being outputted\n");
					break;
				}
			}
		}
		char x=*ptr;
		compCount=1;
		count=1;
		int repeat=1;
		tmp=ptr;
		//Loop through as long as characters are repeating (repeat=1)
		while(repeat!=0){
			if(tmp-ptr<=strlen(ptr)){
				tmp=tmp+1;
			}
			else{
				break;
			}
			
			if(*ptr==*tmp){
				count++;
				compCount++;
			}
			else if(isalpha(*tmp)==0){
				count++;
			}
			
			else{
				repeat=0;
			}
		}
	
		//Create a temporary string to compress and move substrings from input
		char * temp=(char *)malloc(10);
		
		//No compression needed
		if(compCount==1){
			temp[0]=x;
			temp[1]='\0';
			strcat(compressed, temp);
		}
		//No compression needed
		else if(compCount==2){
			temp[0]=x;
			temp[1]=x;
			temp[2]='\0';
			strcat(compressed, temp);
		}
		//Compress substring by concatenating compressed + count + *ptr
		else{
			char * temp2=(char *)malloc(30);
			char *temp3=(char *)malloc(30);
			temp3[0]=x;
			temp3[1]='\0';
			snprintf(temp2, 30, "%d", compCount);
			strcat(compressed, temp2);
			temp= strcat(temp, temp3);
			strcat(compressed, temp);
		}
		//Move on to next sequence
		ptr=ptr+count;
	}
	
	char * compName=CompressName(fileName, partNum);
	
	//Check for existance of compressed file
	FILE *g;
	
	//File Already Exists
	if(access(compName, F_OK)!=-1){
		printf("Warning: File already exists, being overwritten\n");
	}
	
	//Write compressed file
		g=fopen(compName, "w");
		fputs(compressed, g);
		fclose(g);
		pthread_exit(0);
}

int main(int argc, char *argv[]){
	//Check if file exists or can be opened
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
	
	//Initialize Global Variables for File Name and Number of Parts
	FName=argv[1];
	
	//Check if number of parts is actually a number
	int len=strlen(argv[2]);
	int i;
	for(i=0;i<len;i++){
		if(isdigit(*(argv[2]+i)) == 0){
			printf("Error: Number of parts is not a digit\n");
			exit(EXIT_FAILURE);
		}
	}
	
	k=atoi(argv[2]);
	if(k<1){
		printf("Error: Number of Parts must be 1 or Greater\n");
		exit(EXIT_FAILURE);
	}
	
	//Create array of threads and attribute
	pthread_t threads[k];
	pthread_attr_t attr;
	int X;
	
	//Set thread to joinable
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	//Create Mutex Locks
	pthread_mutex_init(&lock1, NULL);
	pthread_mutex_init(&lock2, NULL);
	pthread_mutex_init(&lock3, NULL);
	
	struct input inputarray[k];

	FILE *file=fopen(argv[1], "r");
	fseek(file, 0, SEEK_END);
	characters=ftell(file)-1;
	fclose(file);
	
	//Create Threads
	for(i=0;i<k;i++){
		if(k==1){
			inputarray[i].fNum=-1;
		}
		inputarray[i].fNum=i;
		X=pthread_create(&threads[i], &attr, (void *) *compressT_LOLS, &inputarray[i]);
		if(X){
			fprintf(stderr, "Cannot Create Thread\n");
			exit(-1);
		}
	}
	
	//Join Threads
	pthread_attr_destroy(&attr);
	int j;

	for(j=0;j<k;j++){
		X=pthread_join(threads[j], NULL);
		if(X){
			fprintf(stderr, "Cannot Join Thread # %d\n", j); 
		}
	
	}
	pthread_exit(0);  
		

	return 0;
}
