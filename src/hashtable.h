/**************************************************************
* Class: CSC-415-01 Spring 2020
* Name: Ibraheem Chaudry
* Student ID: 917227459
* Project: Assignment 2 – Word Blast
* File: hashtable.h
* Description: Header file for the hashtable.c
* @author Ibraheem Chaudry
* **************************************************************/

#ifndef WORD_BLAST_HASHTABLE1_H
#define WORD_BLAST_HASHTABLE1_H


//typedefs
typedef struct entry_t {
    char *key;
    char *value;
    struct entry_t *next;
} entry_t;

typedef struct {
    entry_t **entries;
} ht_t;


//Function declarations
unsigned int hash(const char *key);
entry_t *ht_pair(const char *key, const char *value);
ht_t *ht_create(void);
void ht_set(ht_t *hashtable, const char *key, const char *value);
char *ht_get(ht_t *hashtable, const char *key);
void ht_del(ht_t *hashtable, const char *key);
void ht_dump(ht_t *hashtable);
void findTop10(ht_t *hashtable, entry_t top10[]);
void ht_free(ht_t *hashtable);


#endif //WORD_BLAST_HASHTABLE1_H
