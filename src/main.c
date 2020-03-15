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
#define TOKENIZER_DELIMITER " ,.;:!?\t\n"
#define INT2POINTER(a) ((char*)(intptr_t)(a))

char *readFile(void * filePointer, char * filePath );
char **tokenizeFileContents(char *fileContent);
void *threadTest(void *x);
ht_t * initHashTable(char ** tokens);


/**
 * Here is the plan on how I am going to do it.
 * Break the file buffer into no of parts based on how many threads it needs to run on
 *
 * Start the threads with the parameters
 *
 *
 */


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

    //File related
    char * filePath;
    char * fileContent = 0;
    char **tokens;
    long fileSize;
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

    //Read file and allocate a buffer
    fileContent= readFile(filePointer,filePath);

//    printf("%s\nThe above output is coming out of main.\n",fileContent);


    tokens= tokenizeFileContents(fileContent);

//
//    ht_t *ht = ht_create();
//
////    int index=0;
////
////    char *token="hello";
////
////    printf("The token is: %s\n",token);
//
//    ht_set(ht,"hello",'1');




//    while(1){
//        if(tokens[index]==NULL){
//            break;
//        }
//        char *token=tokens[index];
//
//        printf("The token is: %s\n",token);
//
//        ht_set(ht,token,1);
//
//
//
//        index++;
//    }



    /**
    * Testing Hashmap
    */
    ht_t *ht = initHashTable(tokens);



    /* spawn the threads */
    for (i=1; i<=nThreads; i++)
    {
        thread_args[i] = i;
        printf("spawning thread %d\n", i);
        //Catch error by checking if rc is equal to zero
        rc = pthread_create(&threads[i], NULL, threadTest, (void *) &thread_args[i]);
    }

    /* wait for threads to finish */
    for (i=1; i<=nThreads; i++) {
        //catch error by checking if rc is equal to zero
        rc = pthread_join(threads[i], NULL);
    }




    /**
     * Frees for dynamically allocated structures
     */
    free(fileContent);
    free(tokens);
    //Test if the freeing of hash table is completing successfully
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
 * @param x
 * @return
 */
void *threadTest(void *x)
{
    int tid;
    tid = *((int *) x);
    printf("Hi from thread %d!\n", tid);
    return NULL;
}



char **tokenizeFileContents(char *fileContent)
{
    int bufferSize = TOKENIZER_BUFFER_SIZE, position = 0;
    char **tokens = malloc(bufferSize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(fileContent, TOKENIZER_DELIMITER);
    while (token != NULL) {
//        printf("The token is: %s and the string length is: %lu\n",token,strlen(token));

        if(strlen(token)>5){
//            printf("The token is %s",token);
//            //Covert the string to lowercase
//            for(int i = 0; strlen(token); i++){
//                token[i] = tolower(token[i]);
//            }
//            printf("The token is %s",token);
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

        token = strtok(NULL, TOKENIZER_DELIMITER);
    }

    tokens[position] = NULL;

//    for(int i=0;i<position;i++){
//        printf("The tokens in the array are: %s\n",tokens[i]);
//    }

    return tokens;
}







ht_t * initHashTable(char ** tokens){
    ht_t *ht = ht_create();

    ht_set(ht, "name1", "em");
    ht_set(ht, "name2", "russian");
    ht_set(ht, "name3", "pizza");
    ht_set(ht, "name4", "doge");
    ht_set(ht, "name5", "pyro");
    ht_set(ht, "name6", "joost");
    ht_set(ht, "name7", "kalix");

    ht_dump(ht);

    int index=0;

    while(1){
        if(tokens[index]==NULL){
            break;
        }
        char * token=tokens[index];

        if(ht_get(ht,token)==NULL){
            ht_set(ht,token,"1");
//            ht_dump(ht);
        }
        else{
            int count=atoi(ht_get(ht,token))+1;
            printf("the new count is: %d\n", count);
            char  buf[30];
            snprintf(buf, sizeof(buf), "%d", count);
            printf  ("buf is now: %s\n", buf);


//            sprintf(value,"%d",count);
            ht_set(ht,token,&buf);
        }
        index++;
    }

    printf("The frequency of Pierre is : %s\n", ht_get(ht,"Prince"));



//    ht_dump(ht);

    return ht;
}

//
//if(ht_get(ht,token)==NULL){
//ht_set(ht,token,1);
//ht_dump(ht);
//}
//else{
//int count=atoi(ht_get(ht,token))+1;
//ht_set(ht,token,count);
//ht_dump(ht);
//}


/**
 * This function reads a file and assigns a buffer on the heap for the file content
 * Some part of this code has been obtained from: https://stackoverflow.com/a/174552
 * @param filePointer
 * @param filePath
 * @param fileContent
 * @return fileSize
 */
char * readFile(void * filePointer, char * filePath){
    filePointer = fopen (filePath, "r");
    long fileSize;
    char * fileContent = 0;

    if (filePointer)
    {
        fseek (filePointer, 0, SEEK_END);
        fileSize = ftell (filePointer);
        fseek (filePointer, 0, SEEK_SET);
        fileContent = malloc (fileSize);
        if (fileContent)
        {
            fread (fileContent, 1, fileSize, filePointer);
        }
        fclose (filePointer);
    }

    if (fileContent)
    {
        printf("The buffer was correctly allocated using what was in the text file\n");
//        printf("The size of the file is %lu\n",strlen(fileContent));
//        printf("%s",fileContent);
    }

    return fileContent;
}