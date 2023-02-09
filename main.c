//Antonio Maniscalco
//CS 360 Assignment 1
// Main file

#include"headers.h"

int main(int argc, char **argv) {
    //var to keep track of time for program to run
    double time = 0.0;
    double hashTime = 0.0;
    //start the clock
    clock_t begin = clock();
    //int for specific word count if specified
    int wordPairCountArg = 0;
    int argNum = 1;
    char userIn[256];

    //argument count should be >2 if not print error and exit
    assert(argc >= 2);

    //check if word count number is specified in command line
    if(isdigit(*argv[1])) {
        assert(argc >= 3);
        wordPairCountArg = atoi(argv[1]);
        argNum += 1;
        } 
    //assert(argc > 2);
    
    //loop through command line arguements to deal with files
    while(argNum < argc) {
        clock_t start = clock();
        parseFile(argv[argNum], wordPairCountArg);
        clock_t finish = clock();
        printf("\t<Time elapsed: %f>\n\n", (hashTime+((double)(finish - start)/(CLOCKS_PER_SEC))));
        hashTime = 0.0;
        argNum++;
    }
    //end clock and calculate time elapsed
    clock_t end = clock();
    printf("<Total Time elapsed: %f>\n", (time+((double)(end - begin)/(CLOCKS_PER_SEC))));

    return(1);
}

//compare function
int comp(const void *p, const void *q) {
    return(((*(const struct node**)q)->count - (*(const struct node**)p)->count));
}

//
int parseFile(char *fileName, int wordPairCountArg) {
    int hashSize = 10; //initial table size
    int collisions = 0; //will keep track of number of collisions to determine when hash table needs to be resized
    
    //Open filename assert file pointer not null. exit if NULL
    FILE *fp = fopen(fileName, "r");
    assert(fp != NULL);

   /*Create our structure to keep benchmarks */
    struct information *hashInfo = malloc(sizeof(struct information));
    hashInfo->nodeCount = 0;
    hashInfo->collisions = 0;
    hashInfo->totalCollisions = 0;
    hashInfo->rehashes = 0;
    
    hashEntry *hashTable = hashInit(hashSize); //initialize first hash table
    hashEntry *newHashTable; //for creating new hash tables
    hashEntry *tempTable = hashTable; //pointer for hashtable and new hash tables
    char *word = getNextWord(fp); //grab first word
    char prev[50]; //temp char arrays for grabbing words and concatenating the two strings
    char temp[50]; 
    strcpy(prev, word); //copy previous word to our previous string
    free(word); //free memory allocated for word
    word = getNextWord(fp); 
    //start parsing file in while loop
    while(word != NULL) {
        if(word == NULL) { break; }
        
        //concatenate strings prev and word and copy them into temp
        strcpy(temp, strcat(strcat(prev," "),word)); 
        //add hash function returns a 1 if there is a collision and zero if not
        addHash(tempTable, temp, hashSize, hashInfo); 
        hashInfo->totalCollisions += hashInfo->collisions;
        
        
        if(hashInfo->collisions/hashSize >= 2) {
            //make our new hashtable and copy old to new and triple the size of our hash
            hashInfo->rehashes += 1;
            hashInfo->collisions = 0;
            hashInfo->nodeCount=0;
            hashTable = newHash(hashTable, hashSize, hashInfo); 
            //set our temp as the new hash table with data
            tempTable = hashTable; 
            hashSize = hashSize*3; 
        }
        strcpy(prev, word);
        free(word);
        word = getNextWord(fp);
    }

    //new array of structures size will be our node count + size of the old hash
    int arrCount = hashInfo->nodeCount + hashSize;

    //create new hash table with no chaining
    int i = 0; int j = 0;
    hashEntry *sortArr = malloc(arrCount*(sizeof(sortArr))); 
    //copy old hash to new array 
    while(i< hashSize) {
        //traverse most recent hash
        struct node *tempPut = hashTable[i];
        //traverse linked list in hash and store it into new array
        if(tempPut != NULL) {
            while(tempPut != NULL) {
                sortArr[j] = tempPut; 
                tempPut = tempPut->next;
                j++;
            }
        }
        i++;
    }

    //Sort our array of word pairs
    qsort(sortArr,arrCount,sizeof(struct node*),comp); 
    printf("\n<%s> <READING>\n", fileName);

    //Check if specfic wordpair counts was specified in command line, print those, if not print the whole array
    if(wordPairCountArg == 0) {
        for(int i=0; i < arrCount; i++) {
        //print out new sorted list

            if(sortArr[i]->wordPair != NULL) { 
                printf("%10d %s\n", sortArr[i]->count, sortArr[i]->wordPair); 
            }
        }
    } else {
        for(int i=0; i < wordPairCountArg; i++) {
            if(i == arrCount) break;
            if(sortArr[i]->wordPair != NULL) { 
                printf("%10d %s\n", sortArr[i]->count, sortArr[i]->wordPair); 
            }
        }
    }

    //close file give complete message and return
    printf("<%s> <COMPLETE>\n", fileName);
    printf("Diagnostics\n \t<Total collisions: %ld>\n \t<Re-Hashes: %d>\n", hashInfo->totalCollisions, hashInfo->rehashes);
    //Free our old hash and the sorted one
    freeHash(hashTable, hashSize);
    free(sortArr);
    free(hashInfo);
    fclose(fp);
    
    return(1);
}



