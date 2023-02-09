//Antonio Maniscalco
//CS 360 Assignment 1
//Hash Functions file
//Lines 40-97 provided by Prof. McCamish

#include"headers.h"
#define CRC64_REV_POLY      0x95AC9329AC4BC9B5ULL
#define CRC64_INITIALIZER   0xFFFFFFFFFFFFFFFFULL
#define CRC64_TABLE_SIZE    256

//testing
int globCount = 0;
int lineCount = 0;

//Hash init function
hashEntry *hashInit(int hashSize) {
    int index = 0; 
	hashEntry *hashTable = malloc(hashSize*(sizeof(hashTable))); //allocate memory for hash table
	//printf("SIZE OF HASH TABLE: %lu\n", sizeof(hashTable));
	while(index <= (hashSize-1))
    {
        hashTable[index] = listInit();
        index++; //next location in table array
    }

    return(hashTable);
}

//Init node in hash table
struct node *listInit()
{
    struct node *sent = malloc(sizeof(struct node));
	sent->wordPair = NULL;
	sent->count = 0;
    sent->next = NULL;
    return(sent);
}

//Hash funcion
unsigned long long crc64(char* string, int hashSize) {
    static int initFlag = 0;
    static unsigned long long table[CRC64_TABLE_SIZE];
    
    if (!initFlag) { initFlag++;
        for (int i = 0; i < CRC64_TABLE_SIZE; i++) {
            unsigned long long part = i;
            for (int j = 0; j < 8; j++) {
                if (part & 1)
                    part = (part >> 1) ^ CRC64_REV_POLY;
                else part >>= 1;
            }
            table[i] = part;
        }
    }
    
    unsigned long long crc = CRC64_INITIALIZER;
    while (*string)
        crc = table[(crc ^ *string++) & 0xff] ^ (crc >> 8);
    return(crc%hashSize); //REFACTOR ORIGINAL IS JUST RETURN CRC
	//return(hashSize);
}

//Get next word function
char* getNextWord(FILE* fd) {
	char ch;								/* holds current character */
	char wordBuffer[DICT_MAX_WORD_LEN];		/* buffer for build a word */
	int putChar = 0;						/* current pos in buffer   */

	assert(fd != NULL);		/* the file descriptor better not be NULL */

	/* read characters until we find an alphabetic one (or an EOF) */
	while ((ch = fgetc(fd)) != EOF) {
		if (isalpha(ch)) break;
	}
	if (ch == EOF) return NULL;		/* if we hit an EOF, we're done */

	/* otherwise, we have found the first character of the next word */
	wordBuffer[putChar++] = tolower(ch);

	/* loop, getting more characters (unless there's an EOF) */
	while ((ch = fgetc(fd)) != EOF) {
		/* the word is ended if we encounter whitespace */
		/* or if we run out of room in the buffer       */
		if (isspace(ch) || putChar >= DICT_MAX_WORD_LEN - 1) break;

		/* otherwise, copy the (lowercase) character into the word   */
		/* but only if it is alphanumeric, thus dropping punctuation */
		if (isalnum(ch)) {
			wordBuffer[putChar++] = tolower(ch);
		}
	}

	wordBuffer[putChar] = '\0';		/* terminate the word          */
	return strdup(wordBuffer);		/* re-allocate it off the heap */
}


//Add hash entry function 
//Function will return a 1 if there is a collision for our collision count and a zero if there is no collision
int addHash(hashEntry *hashTable, char *werd, int hashSize, struct information *info) {
	int indexLocation = crc64(werd, hashSize);
	if(hashTable[indexLocation]->wordPair == NULL) {
		hashTable[indexLocation]->wordPair = malloc(strlen(werd)+1);
		strcpy(hashTable[indexLocation]->wordPair, werd);
		//Increment count and set next pointer to null
		hashTable[indexLocation]->count += 1; 
		hashTable[indexLocation]->next = NULL;
		//info->nodeCount += 1;
		return(0);
		}  //if word pair has not been stored yet store string there
	//Check if first element matches word pair
	if (strcmp(hashTable[indexLocation]->wordPair, werd) != 0) {
		hashTable[indexLocation] = collision(hashTable[indexLocation], werd, info); //call collision
		return(1);
	}
	else{
		hashTable[indexLocation]->count += 1;
	} 
	return(0);
}

//collision handler function
struct node *collision(struct node *hashNode, char *word, struct information *info) {
	//info->nodeCount += 1;
	struct node *newNode = listInit(); //create the new node for insertion
	newNode->wordPair = malloc((strlen(word)) + 1); //allocate memory for string
	strcpy(newNode->wordPair, word); //copy string to word pair
	//Temporary node for traversing linked list and temp root node to save the place of the root
	struct node *temp = hashNode;
	struct node *previous = hashNode;
	while(temp != NULL) {
		
		if(strcmp(temp->wordPair, word) == 0) {
			temp->count++; //icrement count in current node if strings match
			free(newNode->wordPair);
			free(newNode->next);
			free(newNode); //free newly created node because string already matches
			//info->nodeCount += 1;
			return(hashNode);
		}
		
		previous = temp;
		temp = temp->next;
		if(temp == NULL) {
			previous->next = newNode;
			newNode->next = NULL;
			newNode->count++;
			info->collisions += 1;
			info->nodeCount += 1;
			return(hashNode);
		}
	}
	return(hashNode);
}

//Rehashing function
hashEntry *newHash(hashEntry *hashTable, int hashSize, struct information *info) {
	int i = 0;
	int indexLocation; //for getting th
	int hashTableSize = hashSize;
	hashSize = hashSize*3;
	hashEntry *new = hashInit(hashSize);
	struct node *temp2;
	struct node *current;
	while(i < hashTableSize) {
		if(hashTable[i]->wordPair != NULL) {
			indexLocation = crc64(hashTable[i]->wordPair, hashSize); //get hash function return value of string
			//copy to new hash table
			new[indexLocation] = copyHash(new, indexLocation, hashTable[i]->wordPair, hashTable[i]->count, info);
			//new[indexLocation] = addHash(new, hashTable[i]->wordPair, hashSize);
		}
		if(hashTable[i]->next != NULL) {
			//traverse linked list to add chained word pairs to new hash
				temp2 = hashTable[i]->next; 
				while(temp2 != NULL) {
					if(temp2 == NULL) { break; }
					indexLocation = crc64(temp2->wordPair, hashSize);
					new[indexLocation] = copyHash(new, indexLocation, temp2->wordPair, temp2->count, info);
					temp2 = temp2->next;
				}
			}
		i++;
	}
	freeHash(hashTable, hashTableSize); //free the old hash
	//printf("\nOLD HASH FREED\n");

	return(new);
	
}



/*LATER NEED TO HANDLE COLLISION COUNT HERE */
//Copy hash function
struct node *copyHash(hashEntry *newHash, int index, char *wordPair, int count, struct information *info) {
	struct node *previous = newHash[index];
	struct node *current = newHash[index];
	//if key is empty insert hash there
	if(newHash[index]->wordPair == NULL) {
		//copy string and count then set next to null
		newHash[index]->wordPair = malloc(strlen(wordPair)+1);
		strcpy(newHash[index]->wordPair, wordPair);
		newHash[index]->count = count; 
		newHash[index]->next = NULL;
		return(newHash[index]);
		}

	//check if key is filled and if the word pairs match or not
	else {
		//create the new node for insertion copy info over
		struct node *newNode = listInit(); 
		newNode->wordPair = malloc((strlen(wordPair)) + 1); 
		strcpy(newNode->wordPair, wordPair); //copy string to word pair
		newNode->count = count;
		newNode->next = NULL;
		info->collisions += 1;

		//Find empty node to insert
		while(current != NULL) {
			current = previous->next;
			//check if node is empty
			if(current == NULL) {
				info->nodeCount += 1;
				//set next pointer to new node and current to new node
				previous->next = newNode;
				newNode->next = NULL;
				//current = newNode;
				return(newHash[index]);
			}
			previous = current;
			//current = current->next;
		}
	}
	return(newHash[index]);
}


/* Function to get last node of linked list */
struct node *findNode(struct node *sentinal) {
	struct node *current = sentinal;
	struct node *previous = sentinal;

	//traverse list
	while(current != NULL) {
		//if current is NULL return previous node
		if(current == NULL) {
			return(previous);
		}
		//copy previous to current and current to next
		previous = current;
		current = current->next;
	}
	return(previous);
}

//Free hashtable function
void freeHash(hashEntry *hashTable, int hashSize) {
    int index = 0;
    while(index < hashSize) {
    	if(hashTable[index] == NULL) break;
		//if there is a collision free the linked list
		if(hashTable[index]->next != NULL) {
			freeList(hashTable[index]->next);
		}
        free(hashTable[index]->wordPair);
        //free(hashTable[index]->next);
        free(hashTable[index]);
        index++;
    }
    free(hashTable);
}

//Free linked list function
void freeList(struct node *list) {
	struct node *temp;
	struct node *current = list;
	while(current != NULL) {
		if (current == NULL) { return; } //end of list return
		temp = current;
		current = current->next; //get next node
		free(temp->wordPair);
		free(temp);
	}
}


//get size of array for the new sorted array
int getCount(hashEntry *newHashTable, int hashSize) {
	int i = 0;
	int count = 0;
	struct node *temp; //temp node to traverse linked list
	if(newHashTable == NULL) {
		return(0);
	}
	while(i < hashSize) {
		if(newHashTable[i]->next != NULL) {
			//if there are more nodes traverse list to get count
			temp = newHashTable[i]->next;
			while(temp != NULL) {
				
				if(temp == NULL) { break; } //dont need this but you know
				count = count+1;
				temp = temp->next;
			}
		}
		i++;
	}
	printf("<%5d> <hash size %d\n", count, hashSize);
	return(count+hashSize); //return size of the array
}
