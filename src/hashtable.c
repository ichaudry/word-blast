/**************************************************************
* Class: CSC-415-01 Spring 2020
* Name: Ibraheem Chaudry
* Student ID: 917227459
* Project: Assignment 2 – Word Blast
* File: hashtable.c
* Description: Hashtable used to keep track of frequencies
* @author Brian Seymour
* @source https://github.com/engineer-man/youtube/tree/master/077
* **************************************************************/



#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

#define TABLE_SIZE 20000


unsigned int hash(const char *key) {
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    // do several rounds of multiplication
    for (; i < key_len; ++i) {
        value = value * 37 + key[i];
    }

    // make sure value is 0 <= value < TABLE_SIZE
    value = value % TABLE_SIZE;

    return value;
}

entry_t *ht_pair(const char *key, const char *value) {
    // allocate the entry
    entry_t *entry = malloc(sizeof(entry_t) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) + 1);

    // copy the key and value in place
    strcpy(entry->key, key);
    strcpy(entry->value, value);

    // next starts out null but may be set later on
    entry->next = NULL;

    return entry;
}

ht_t *ht_create(void) {
    // allocate table
    ht_t *hashtable = malloc(sizeof(ht_t) * 1);

    // allocate table entries
    hashtable->entries = malloc(sizeof(entry_t*) * TABLE_SIZE);

    // set each to null (needed for proper operation)
    int i = 0;
    for (; i < TABLE_SIZE; ++i) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

void ht_set(ht_t *hashtable, const char *key, const char *value) {
    unsigned int slot = hash(key);

    // try to look up an entry set
    entry_t *entry = hashtable->entries[slot];

    // no entry means slot empty, insert immediately
    if (entry == NULL) {
        hashtable->entries[slot] = ht_pair(key, value);
        return;
    }

    entry_t *prev;

    // walk through each entry until either the end is
    // reached or a matching key is found
    while (entry != NULL) {
        // check key
        if (strcmp(entry->key, key) == 0) {
            // match found, replace value
            free(entry->value);
            entry->value = malloc(strlen(value) + 1);
            strcpy(entry->value, value);
            return;
        }

        // walk to next
        prev = entry;
        entry = prev->next;
    }

    // end of chain reached without a match, add new
    prev->next = ht_pair(key, value);
}

char *ht_get(ht_t *hashtable, const char *key) {
    unsigned int slot = hash(key);

    // try to find a valid slot
    entry_t *entry = hashtable->entries[slot];

    // no slot means no entry
    if (entry == NULL) {
        return NULL;
    }

    // walk through each entry in the slot, which could just be a single thing
    while (entry != NULL) {
        // return value if found
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }

        // proceed to next key if available
        entry = entry->next;
    }

    // reaching here means there were >= 1 entries but no key match
    return NULL;
}

void ht_del(ht_t *hashtable, const char *key) {
    unsigned int bucket = hash(key);

    // try to find a valid bucket
    entry_t *entry = hashtable->entries[bucket];

    // no bucket means no entry
    if (entry == NULL) {
        return;
    }

    entry_t *prev;
    int idx = 0;

    // walk through each entry until either the end is reached or a matching key is found
    while (entry != NULL) {
        // check key
        if (strcmp(entry->key, key) == 0) {
            // first item and no next entry
            if (entry->next == NULL && idx == 0) {
                hashtable->entries[bucket] = NULL;
            }

            // first item with a next entry
            if (entry->next != NULL && idx == 0) {
                hashtable->entries[bucket] = entry->next;
            }

            // last item
            if (entry->next == NULL && idx != 0) {
                prev->next = NULL;
            }

            // middle item
            if (entry->next != NULL && idx != 0) {
                prev->next = entry->next;
            }

            // free the deleted entry
            free(entry->key);
            free(entry->value);
            free(entry);

            return;
        }

        // walk to next
        prev = entry;
        entry = prev->next;

        ++idx;
    }
}



void ht_dump(ht_t *hashtable) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_t *entry = hashtable->entries[i];

        if (entry == NULL) {
            continue;
        }

        printf("slot[%4d]: ", i);

        for(;;) {
            printf("%s=%s ", entry->key, entry->value);

            if (entry->next == NULL) {
                break;
            }

            entry = entry->next;
        }

        printf("\n");
    }
}


// void ht_free(ht_t *hashtable) {
//     for (int i = 0; i < TABLE_SIZE; ++i) {
//         entry_t *entry = hashtable->entries[i];

//         if (entry == NULL) {
//             continue;
//         }

//         ht_del(hashtable, entry->key);
//     }

//     free(hashtable->entries);
//     free(hashtable);



// }

void ht_free(ht_t *hashtable) {
    if(!hashtable) return;

     size_t i = 0;
     if(hashtable->entries)
     {
         //Free all entries
         while(i < TABLE_SIZE)
         {
             if(hashtable->entries[i])
             {
                 if(hashtable->entries[i]->key)
                 {
                     free(hashtable->entries[i]->key);
                     hashtable->entries[i]->key = NULL;
                 }
                 if(hashtable->entries[i]->value)
                 {
                     free(hashtable->entries[i]->value);
                     hashtable->entries[i]->value = NULL;
                 }
                 free(hashtable->entries[i]);
                 hashtable->entries[i] = NULL;
             }
             i++;
         }
         //Free the entry list pointer
         free(hashtable->entries);
         hashtable->entries = NULL;
     }

     //Free the hashtable pointer
     free(hashtable);
     hashtable = NULL;
     return;

}



void get_top_10(ht_t *hashtable, entry_t top10[]){

    int max = 0;

    //Initializing to large value at beginning to get most occurring word in first iteration
    int previousMax = 2000000;

    //
    int frequency;



    entry_t valueToPutInArray;

    for(int j=0; j<10;j++){
        //iterate over hashtable entries
        for(int i = 0; i < TABLE_SIZE; i++){
            entry_t *entry = hashtable ->entries[i];

            if(entry == NULL){
                continue;
            }

            //Frequency of entry
            frequency = atoi(entry->value);


            //Check if greater than current max
            if(frequency > max && frequency < previousMax){
                //Set new max
                max = frequency;

                //Initialize entry to put in array
                valueToPutInArray.key = entry->key;
                valueToPutInArray.value = entry->value;
                valueToPutInArray.next = NULL;
            }
        }

        //Set entry into array
        top10[j] = valueToPutInArray;

        previousMax = max;

        max = 0;

    }
}


