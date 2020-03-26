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

#define TOKENIZER_BUFFER_SIZE 64
#define TOKENIZER_DELIMITER " \t\v\n\r,.;:-'\"!?/()[]{}*''""—"


/**
 * Struct for passing in arguments to threads
 */
typedef struct {
    unsigned long start;
    unsigned long end;
} threadArgs;


/**
 * Global variable for file path accessible by all threads
 */
char * filePath;
pthread_mutex_t hashTable_mutex;
ht_t *ht;

//Function declarations
void *processFile(void *arguments);
char * readFile(void * filePointer,long start, long end);
char **tokenizeFileContents(char *fileContent);


int main(int argc, char ** argv){
    struct timespec startTime;
    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &startTime);

    if (pthread_mutex_init(&hashTable_mutex, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    //Number of threads
    int nThreads;

    //Threads
    pthread_t threads[nThreads];
    int rc, i;

    //File Pointer
    FILE * filePointer;

    //Hash Table that stores final frequencies
    ht= ht_create();
//    ht_set(ht,"hello world","1");
//    ht_dump(ht);


    //Get command line arguments to know which file to read and how many threads to create
    if(argc == 3){
        filePath=argv[1];
        nThreads=atoi(argv[2]);
//        printf("The following arguments were passed into program %s , %d\n",filePath,nThreads);
    }
    else{
        printf("Wrong format for passing arguments. Please follow the following convention \n<name of executable> <filePath> <numberOfThreads>");
        return 0;
    }


    //Getting the file size and calculating chunk size to pass to individual threads
    long fileSize;
    long chunkSize;

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

    printf("This the size of the file %lu\n",fileSize);

    //Initialize the chunk size using the file size and the number of threads the program runs
    chunkSize=fileSize/nThreads;

    printf("This is the chunk size: %lu\n\n\n", chunkSize);

//    threadArgs *threadArgs=malloc(sizeof(threadArgs));
    threadArgs *threadArgs;
    /* spawn the threads */
    for (i=0; i<nThreads; i++)
    {
        //Thread arguments structure
        threadArgs=malloc(sizeof(threadArgs));
        threadArgs->start=i*chunkSize;
        threadArgs->end=(i*chunkSize)+chunkSize;


//        printf("spawning thread %d\n", i+1);
//        printf("The start of file for this thread is: %lu\n",threadArgs->start);
//        printf("The end of file for this thread is: %lu\n",threadArgs->end);
        //Catch error by checking if rc is equal to zero
        rc = pthread_create(&threads[i], NULL, processFile, (void *) threadArgs);
        if(rc!=0){
            perror("Error creating thread");
        }

        //Call to free threadArgs after creation of last thread
//        if(i==nThreads-1){
//            free(threadArgs);
//            threadArgs=NULL;
//        }
    }





//    threadArgs=NULL;

    int threadTerminated=0;

//    while(threadTerminated<nThreads){
    /* wait for threads to finish */
    for (i=0; i<nThreads; i++) {
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

    printf("Number of Threads Terminated: %d\n",threadTerminated);
//    }

    pthread_mutex_destroy(&hashTable_mutex);


    printf("The frequency of Prince is : %s\n", ht_get(ht,"prince"));

    //To-do check if the hashtable has been successfully freed
    ht_free(ht);


    free(threadArgs);



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
 * @param x
 * @return
 */
void *processFile(void *arguments)
{

    threadArgs * args= (threadArgs * )arguments;
    unsigned long start= args->start;
    unsigned long end= args->end;
    FILE * filePointer;
    int index=0;


    printf("Start: %lu\n",start);
    printf("End: %lu\n\n",end);


//    printf("Step 1\n");
    //Read file to buffer
    char * fileContent= readFile(filePointer,start,end);

//    printf("This is the size of file buffer: %lu\n",strlen(fileContent));

//    printf("Step 2\n");
    //Tokenize file contents
    char **tokens=tokenizeFileContents(fileContent);

    //Loop to iterate over all tokens
    while(1){
        if(tokens[index]==NULL){
            break;
        }
        char * token=tokens[index];

        pthread_mutex_lock(&hashTable_mutex);
        if(ht_get(ht,token)==NULL){
            ht_set(ht,token,"1");
        }
        else{
            int count=atoi(ht_get(ht,token))+1;
            char  *buf=malloc(32);
            snprintf(buf, sizeof(buf), "%d", count);
            ht_set(ht,token,buf);
            free(buf);
        }
        pthread_mutex_unlock(&hashTable_mutex);

        index++;
    }


//    printf("This is the number of words greater than 6: %d\n",index);

    //Free the file content buffer
    free(fileContent);
    free(tokens);


    return NULL;
}






/**
 * This function reads a file and assigns a buffer on the heap for the file content
 * Some part of this code has been obtained from: https://stackoverflow.com/a/174552
 * @param filePointer
 * @param filePath
 * @param fileContent
 * @return fileSize
 */
char * readFile(void * filePointer,long start, long end){
//    printf("The Read File function received:\nStart:%lu\nEnd:%lu\n-------------\n\n",start,end);


    char * fileContent = 0;
    filePointer = fopen (filePath, "r");
    long chunkSize= end-start;
//    printf("This is the chunk size calculated by the readFile func: %ld\n",chunkSize);

    if (filePointer)
    {
        fseek (filePointer, start, SEEK_SET);
        fileContent = malloc (chunkSize);
        if (fileContent)
        {
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

    if (fileContent)
    {
//        printf("The buffer was correctly allocated using what was in the text file\n");
    }

    return fileContent;
}


char **tokenizeFileContents(char *fileContent)
{
    int bufferSize = TOKENIZER_BUFFER_SIZE, position = 0;
    char **tokens = malloc(bufferSize * sizeof(char*));
    char *token;
    char *savePtr;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok_r(fileContent, TOKENIZER_DELIMITER,&savePtr);
    while (token != NULL) {
//        printf("The token is: %s and the string length is: %lu\n",token,strlen(token));

        if(strlen(token)>5){
//            printf("The token is : %s and the string length is : %lu\n",token,strlen(token));
            //Covert the string to lowercase
            for(int i = 0; i< strlen(token); i++){
                token[i] = tolower(token[i]);
            }
//            printf("The token after conversion is : %s\n",token);
            tokens[position] = token;
            position++;
        }

        if (position >= bufferSize) {
            bufferSize += TOKENIZER_BUFFER_SIZE;
            tokens = realloc(tokens, bufferSize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok_r(NULL, TOKENIZER_DELIMITER,&savePtr);
    }

    tokens[position] = NULL;

    return tokens;
}
