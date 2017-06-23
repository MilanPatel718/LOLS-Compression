# LOLS-Compression
Design:
The goal of this project was to create programs that could carry out LOLS compression using
both processes and threads. As such, the project is robust in the way in which user input is
handled. Right from the start the inputted file and number of parts to split into are checked for
validity. The file is checked for existence and read permissions being set. The number of parts,
k, is checked to actually be a digit (e.g user cannot input 5a). What happens next is where the
deviation between the process and thread implementations begin.

Processes:
The idea behind the Processes implementation was to have the parent process, or
./compressR_LOLS do no work on the input file. This work is left for every worker process
spawned from ./compressR_worker_LOLS. The only job of the parent process is to set up an
array of arguments to be passed into execvp() after the forking occurs. These arguments consist
of the Worker Code executable, the original file path, number of parts k, the part number being
compressed, and a null terminating character. Argument 4, the part number being compressed,
is derived from the values of i in a for loop that forks the child processes. The loop runs k times,
or the number of parts being split. Each time a child process is forked, it runs execvp() with the
appropriate arguments. The parent process then waits on all child processes outside the loop.
In the child process, none of the variables from the parent process can be accessed. This is
where the choice of execvp() came into play, as this version of exec() allows an array of
arguments to be passed as the char *argv[] of the worker process. This information alone
allows each worker process to calculate what index it needs to start reading at using some
formulas based on the size of the input file. The file is first opened and immediately its size is
calculated. Because extra \u2018\n\u2019 characters are added to text files the total size n is actually
calculated by subtracting 1 from the n=ftell() value. This allows for desired results from the rest
of the program. An important realization was that the compression split depended heavily on n
(size of file) %(mod) k(number of parts), or n%k. Whatever remainder, if any, got added to the
floored value of n/k while the remaining files were compressed with just the floored value of
n/k.
Knowing this information, the next part of the compression algorithm is extracting the
appropriately sized string to compress from the input file.
After this is done, the actual compression is carried out as follows:
1)Move ptr until it is on an alphabetic character, or output a warning message saying all
characters in string are non-alpha
2)Set tmp to ptr and move tmp forward, keeping track counts for every character as well as for
repeating characters. Break from loop once characters stop repeating or end of string is
reached.3)Concatenate *ptr to the compressed string if repeating count is 1, *ptr+*ptr to the
compressed string if repeating count is 2, or repeating count+*ptr if count>=3.
4)Move on and complete rest of string.
5)Write compressed string to appropriately named file (made using function and inputted i
value).
Note that these steps occur for every process that runs, so for k processes.

Threads
The main difference in the thread implementation is the use of pthread instead of fork/exec. In
this implementation, an array of k threads is declared. These threads are created using
pthread_create() in a loop, passing in appropriate values for part number being compressed
and a function pointer to the compression method. The compression method is a carbon copy
of the compression algorithm used in the process worker file. After all the threads are created,
another loop then joins these threads. This is so the main thread does not finish execution
before the worker threads are done. Some notable features in this implementation include the
use of mutex locks for two of the global variables. Without these locks, race conditions manifest
and create unpredictable output. Again, much of the code from the process implementation
was ported for this implementation so there is not much to say here.

Running the Code:
First you must compile the code running make using the included Makefile. This code is then
run using command line arguments. In class, Professor Tjang seemed to have disagreements
with Professor Franscisco on how the code should be run. He ultimately said that it could be run
as such if we chose to do so:
./compressR_LOLS Text.txt 5 (Only an example)
./compressT_LOLS Text.txt 5 (Only an example)
This is how the code should be run on the command line and how it will output desired results.
For the text file, the input can be any file path on the computer as long as it is accessible. The
programs will parse the actual file name from the directory and create the appropriate
compressed files in those directories. If the file does not exist the program will output an error
and quit. If the number of parts is not strictly a digit the program will output an error message
and quit.

Some Assumptions:
-The programs will only compress alphabetic characters and ignore all others
-Files already created will be overwritten if they have the same name
