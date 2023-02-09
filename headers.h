//Antonio Maniscalco
//CS360 HW1 header file
//Lines 11-35 provided by Prof. McCamish
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<ctype.h>
#include<time.h>
#include<unistd.h>

//From getWord.h
#ifndef GETWORD_H
#define GETWORD_H
#define DICT_MAX_WORD_LEN	256		/* maximum length of a word (+1) */
/* Reads characters from fd until a single word is assembled */
/* and returns a copy of the word allocated from the heap.   */
/* NULL is returned at EOF.                                  */

/* Words are defined to be separated by whitespace and start */
/* with an alphabetic character.  All alphabetic characters  */
/* translated to lower case and punctuation is removed.      */


char* getNextWord(FILE* fd);
#endif

//From crc64.h
#ifndef CRC64_H
#define CRC64_H


/* interface to CRC 64 module */

/* crc64 takes a string argument and computes a 64-bit hash based on */
/* cyclic redundancy code computation.                               */

unsigned long long crc64(char* string, int hashSize);
#endif

struct information {
    int collisions;
    int nodeCount;
    long int totalCollisions;
    int rehashes;
}; typedef struct info *myInfo;

//Linked list structure
struct node {
    char *wordPair;
    int count;
    struct node *next;
}; typedef struct node *hashEntry;

unsigned long long crc64(char* string, int hashSize);
char* getNextWord(FILE* fd);
hashEntry *hashInit(int hashSize);
struct node *listInit();
void freeHash(hashEntry *hashTable, int hashSize);
int addHash(hashEntry *hashTable, char *werd, int hashSize, struct information *info); 
void printHash(hashEntry *hashTable, int hashSize);
struct node *collision(struct node *hashNode, char *word, struct information *info);
void printList(struct node *list, int index);
void freeList(struct node *list);
hashEntry *newHash(hashEntry *hashTable, int hashSize, struct information *info);
int getCount(hashEntry *newHashTable, int hashSize);
int comp(const void *p, const void *q);
struct node *copyHash(hashEntry *newHash, int index, char *wordPair, int count, struct information *info);
int parseFile(char *fileName, int wordPairCountArg);
struct node *findNode(struct node *sentinal);