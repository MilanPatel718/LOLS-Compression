#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <libgen.h>

//Function that creates appropriate compressed file name
char * CompressName(char * FilePath, char * FileNum){
	int i;
	char* FileName=basename(FilePath);
	for(i=0; i<strlen(FileName) ; i++){
		if(FileName[i]=='.')
			FileName[i]='_';
	}

	char* compName=(char *)malloc(sizeof(char)*(strlen(FileName)+strlen(FileNum)+10));
	compName[0]='\0';
	strcat(compName, FileName);
	strcat(compName, "_LOLS");
	
	if(strcmp("N", FileNum)==0){
		return compName;
	}
	
	else{
	strcat(compName, FileNum);
	}
	
	return compName;
}

int main(int argc, char *argv[]){

	//Extract Vital Information from Parent
	char * fileName=argv[1];
	char * numParts=argv[2];
	char * compPart=argv[3];
	
	FILE *f=fopen(fileName, "r");
	int fd=fileno(f);
	char *toComp;
	
	//Set up appropriate variables provided by parent process
	fseek(f, 0, SEEK_END);
	int n=ftell(f);
	n=n-1;
	rewind(f);
	int k=atoi(numParts);
	int m=n%k;
	int i=atoi(compPart);
	
	//User attempts to split file into parts greater than the size of the actual file 	
	if(k > n){
		printf("Error: String length %d is too small to split into %d part(s)\n", n+1, k);
		exit(EXIT_FAILURE);
		}
	
	int size;
	if(i==0){
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
			
		if(i==0){
			char * temp= (char *)malloc((sizeof(char)*size)+1);
					
			//Copy substring and move forward on input string
			lseek(fd, i*size, SEEK_SET);
			fread(temp, 1, size, f);
			temp[size]='\0';	
			//Store Component String to be Compressed
			toComp=temp;
		}
					
		else{
			
			char * temp= (char *)malloc((sizeof(char)*size)+1);
					
			//Copy substring and move forward on input string
			fseek(f, (s+((i-1)*size)) , SEEK_SET);
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
	
	char * compName=CompressName(fileName, compPart);
	
	//Check for existance of compressed file
	FILE *g;
	
	//File Already Exists
	if(access(compName, F_OK)==0){
		printf("Warning: File already exists, being overwritten\n");
	}
	
	//Write compressed file
		g=fopen(compName, "w");
		//fwrite(compressed, sizeof(compressed), strlen(compressed), g);
		fputs(compressed, g);
		fclose(g);
	fclose(f);
	return 0;
}

