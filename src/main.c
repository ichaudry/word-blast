/**************************************************************
* Class: CSC-415-01 Spring 2020
* Name: Ibraheem Chaudry
* Student ID: 917227459
* Project: Assignment 2 – Word Blast
* File: main.c
* Description: Multi-threaded program that counts words that are a certain length in a text file and reports the top ten largest found.
* **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include "hashtable.h"

//Buffer to store tokens
#define TOKENS_BUFFER 64
#define TOKENS_DELIMITER " \t\v\n\r,.;:-'\"!?/()[]{}*''""—"


/**
 * Struct for passing in arguments to threads
 */
typedef struct {
    //Start point of file chunk to read
    unsigned long start;

    //End point of file chunk to read
    unsigned long end;
} threadArgs;


/**
 * Global variables
 */
 //File Path
char * filePath;

//Mutex to lock hash table to protect against race conditions
pthread_mutex_t hashTable_mutex;

pthread_mutex_t hashTable_write_mutex;


//Hash Table to store frequencies
ht_t *ht;

/**Function declarations
 *
 */
void *processFile(void *arguments);
char * readFile(void * filePointer,long start, long end);
char **tokenizeFileContents(char *fileContent);
char * test(char * temp, char * buf);


int main(int argc, char ** argv){
    //Initializing a clock to calculate program processing time
    struct timespec startTime;
    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &startTime);


    //Initializing mutex lock
    if (pthread_mutex_init(&hashTable_mutex, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    if (pthread_mutex_init(&hashTable_write_mutex, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    //Number of threads
    int nThreads;

    //Pointer to track threads
    pthread_t * threads = malloc(sizeof(pthread_t)* nThreads *20);

    //Return value from thread creation and joining
    int rc;

    //File Pointer
    FILE * filePointer;

    //Hash Table that stores final frequencies
    ht= ht_create();



    //Get command line arguments to initialize file path and number of threads
    if(argc == 3){
        filePath=argv[1];
        nThreads=atoi(argv[2]);
    }

    else{
        printf("Wrong format for passing arguments. Please follow the following convention \n<name of executable> <filePath> <numberOfThreads>");
        return 0;
    }

    //Total File Size
    long fileSize;

    //Chunk Size for each thread
    long chunkSize;

    //Open file to initialize file size
    filePointer = fopen (filePath, "r");

    //Check if file was successfully opened
    if (filePointer) {
        fseek(filePointer, 0, SEEK_END);
        fileSize = ftell(filePointer);
        fclose(filePointer);
    }
    else{
        perror("Error");
    }

    //Initialize chunk size
    chunkSize=fileSize/nThreads;

    printf("This the size of the file %lu\n",fileSize);

    printf("This is the chunk size: %lu\n\n", chunkSize);


    //Thread argument structure
    threadArgs *threadArgs;
    
    /* spawn the threads */
    for (int i=0; i<nThreads; i++)
    {
        //Thread arguments structure
        threadArgs=malloc(sizeof(threadArgs));
        threadArgs->start=i*chunkSize;
        threadArgs->end=(i*chunkSize)+chunkSize;

        rc = pthread_create(&threads[i], NULL, processFile, (void *) threadArgs);
        if(rc!=0){
            perror("Error creating thread");
        }

    }

    //Counter to check how many threads were terminated
    int threadTerminated=0;

    /* wait for threads to finish */
    for (int i=0; i<nThreads; i++) {
        //catch error by checking if rc is equal to zero
        rc = pthread_join(threads[i], NULL);
        if(rc!=0){
            printf("Error joining thread %d\n",i);
            printf("Error code: %d\n",rc);
        }
        else{
            threadTerminated++;
        }
    }

    printf("Number of Threads Terminated: %d\n\n",threadTerminated);

    /*Destroy mutexes*/
    pthread_mutex_destroy(&hashTable_mutex);
    pthread_mutex_destroy(&hashTable_write_mutex);


    //Print Top 10 occurring words
    entry_t  top10[10];

    get_top_10(ht,top10);

    for(int i=0 ; i<10;i++){
        printf("%s   -   %d\n",top10[i].key,atoi(top10[i].value));
    }



    /**
    *Free memory allocated for the hashtable. 
    *Function in hashtable.c
    */
    ht_free(ht);


    /**
     * Calculates the time taken to execute the program. Should be
     * */
    clock_gettime(CLOCK_REALTIME, &endTime);
    time_t sec = endTime.tv_sec - startTime.tv_sec;
    long n_sec = endTime.tv_nsec - startTime.tv_nsec;

    if (endTime.tv_nsec < startTime.tv_nsec)
        {
        --sec;
        n_sec = n_sec + 1000000000L;
        }

    printf("Total Time was %ld.%09ld seconds\n", sec, n_sec);

    return 0;

}



/**
 *
 * @param arguments
 * @return
 */
void *processFile(void *arguments)
{

    threadArgs * args= (threadArgs * )arguments;
    unsigned long start= args->start;
    unsigned long end= args->end;
    FILE * filePointer;
    int index=0;

    //Read file to buffer
    char * fileContent= readFile(filePointer,start,end);

    //Tokenize file contents
    char **tokens=tokenizeFileContents(fileContent);

    //Loop to iterate over all tokens and store frequencies
    while(1){
        if(tokens[index]==NULL){
            break;
        }

        //Get token
        char * token=tokens[index];

        //Lock the read operation
        pthread_mutex_lock(&hashTable_mutex);
        if(ht_get(ht,token)==NULL){

            ht_set(ht,token,"1");

            //unlock read
            pthread_mutex_unlock(&hashTable_mutex);
        }

        else{
            //Unlock read
            pthread_mutex_unlock(&hashTable_mutex);

            //Lock the write operation
            pthread_mutex_lock(&hashTable_write_mutex);

            int count=atoi(ht_get(ht,token))+1;

            char buf[32];

            snprintf(buf, sizeof(buf), "%d", count);

            //Set new frequency
            ht_set(ht, token, buf);

            pthread_mutex_unlock(&hashTable_write_mutex);
        }
        index++;
    }

    //Free buffers
    free(fileContent);
    free(tokens);
    free(args);

    return NULL;
}


/**
 * This function reads a file and assigns a buffer on the heap for the file content
 * Some part of this code has been obtained from: https://stackoverflow.com/a/174552
 * @param filePointer
 * @param start
 * @param end
 * @return fileSize
 */
char * readFile(void * filePointer,long start, long end){
    char * fileContent = 0;
    filePointer = fopen (filePath, "r");
    long chunkSize= end-start;

    if (filePointer)
    {
        //Set file pointer to start of chunk thread needs to read
        fseek (filePointer, start, SEEK_SET);

        //Allocate buffer to store file content
        fileContent = malloc (chunkSize);
        if (fileContent)
        {
            //Load the buffer up with threads desginated portion of file
            fread (fileContent, 1, chunkSize, filePointer);
        }
        else{
            perror("Error");
        }
        fclose (filePointer);
    }
    else{
        printf("Unable to open file\n\n");
    }

    return fileContent;
}

/**
 *
 * @param fileContent
 * @return
 */
char **tokenizeFileContents(char *fileContent)
{
    int bufferSize = TOKENS_BUFFER, position = 0;
    char **tokens = malloc(bufferSize * sizeof(char*));
    char *token;
    char *savePtr;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while ((token = strtok_r(fileContent, TOKENS_DELIMITER,&savePtr))) {
        if(strlen(token)>5){
            //Covert the string to lowercase
            for(int i = 0; i< strlen(token); i++){
                token[i] = tolower(token[i]);
            }

            tokens[position] = token;
            position++;

        }

        if (position >= bufferSize) {
            bufferSize += TOKENS_BUFFER;
            tokens = realloc(tokens, bufferSize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        fileContent=savePtr;
    }

    tokens[position] = NULL;

    return tokens;
}
