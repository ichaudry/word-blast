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
#include "hashtable.h"

#define TOKENIZER_BUFFER_SIZE 64
#define TOKENIZER_DELIMITER " \t\v\n\r,.;:-'\"!?/()[]{}*''""—"


//Thread arguments structure
typedef struct {
    unsigned long start;
    unsigned long end;
    char * filePath;
} threadArgs;



void *processFile(void *arguments);
char * readFile(void * filePointer, char * filePath,long start, long end);

int main(int argc, char ** argv){
    struct timespec startTime;
    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &startTime);

    //Number of threads
    int nThreads;

    //Threads
    pthread_t threads[nThreads];
    int rc, i;


    //File related
    char * filePath;
    FILE * filePointer;


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

    printf("This is the chunk size: %lu\n\n\n\n\n", chunkSize);

    /* spawn the threads */
    for (i=0; i<nThreads; i++)
    {
        //Thread arguments structure
        threadArgs *threadArgs=malloc(sizeof(threadArgs));
        threadArgs->start=i*chunkSize;
        threadArgs->end=(i*chunkSize)+chunkSize;
        threadArgs->filePath= malloc(sizeof(filePath));
        threadArgs->filePath=filePath;
        printf("This is the file path : %s\n",filePath);


        printf("spawning thread %d\n\n\n", i+1);
//        printf("The start of file for this thread is: %lu\n",threadArgs.start);
//        printf("The end of file for this thread is: %lu\n",threadArgs.end);
        //Catch error by checking if rc is equal to zero
        rc = pthread_create(&threads[i], NULL, processFile, (void *) threadArgs);

        if(i==nThreads-1){
            free(threadArgs);
        }
    }

    /* wait for threads to finish */
    for (i=1; i<=nThreads; i++) {
        //catch error by checking if rc is equal to zero
        rc = pthread_join(threads[i], NULL);
    }



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
    char * filePath=args->filePath;
    FILE * filePointer;

    printf("The start of file for this thread is: %lu\n",start);
    printf("The end of file for this thread is: %lu\n",end);
//    printf("This is the file path being recieved by the Thread %s\n\n\n",filePath);





    //Read file to buffer
    char * fileContent= readFile(filePointer,filePath,start,end);


//    printf("Here is the file printing out using threads\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n%s",fileContent);
    printf("the length the file content for thread is = %lu\n",strlen(fileContent));

//    printf("Thread finished printing \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");


    free(fileContent);

//    printf("The start of file for this thread is: %lu\n",args.start);
//    printf("The end of file for this thread is: %lu\n",args.end);
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
char * readFile(void * filePointer, char * filePath,long start, long end){
    char * fileContent = 0;
    filePointer = fopen (filePath, "r");
    long chunkSize= end-start;

//    printf("This is the file path being recieved by the rad file function %s\n",filePath);

    if (filePointer)
    {
        fseek (filePointer, start, SEEK_SET);
        fileContent = malloc (chunkSize);
        if (fileContent)
        {
            fread (fileContent, 1, chunkSize, filePointer);
        }
        fclose (filePointer);
    }
    else{
        printf("Unable to open file\n\n\n");
    }

    if (fileContent)
    {
        printf("The buffer was correctly allocated using what was in the text file\n\n\n");
    }

    return fileContent;
}

