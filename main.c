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


void *threadTest(void *x)
{
    int tid;
    tid = *((int *) x);
    printf("Hi from thread %d!\n", tid);
    return NULL;
}


int main(int argc, char ** argv){

    struct timespec startTime;
    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &startTime);

    //Number of threads
    int nThreads;

    //Threads
    pthread_t threads[nThreads];

    //Thread arguments
    int thread_args[nThreads];
    int rc, i;

    //Filename
    char * fileName;
    char * temp="./";
    char * filePath;



    //Get command line arguments to know which file to read and how many threads to create
    if(argc == 3){
        fileName=argv[1];
        nThreads=atoi(argv[2]);

        //Needs to freed after use
        filePath = malloc(strlen(fileName) + strlen(temp) + 1);

        strcpy(filePath, temp);
        strcat(filePath, fileName);


        printf("The following arguments were passed into program %s , %d\n",filePath,nThreads);
    }



    else{
        printf("Wrong format for passing arguments. Please follow the following convention \n<name of executable> <filePath> <numberOfThreads>");
        return 0;
    }


    //Depending on the number of threads break up the file into appropriate chunks and pass each chunk to a thread

    //The thread runs a function to read the file word by word and determine whether it satisfies the required constraints


    /* spawn the threads */
    for (i=1; i<=nThreads; ++i)
    {
        thread_args[i] = i;
        printf("spawning thread %d\n", i);
        //Catch error by checking if rc is equal to zero
        rc = pthread_create(&threads[i], NULL, threadTest, (void *) &thread_args[i]);
    }

    /* wait for threads to finish */
    for (i=1; i<=nThreads; ++i) {
        //catch error by checking if rc is equal to zero
        rc = pthread_join(threads[i], NULL);
    }



    /**
     * Frees for dynamically allocated structures
     */
     //Free the file path string array
     free(filePath);






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


