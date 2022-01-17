#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define HASH_MAX_BUCKETS 59388
#define NUMBER_OF_MOD 59387

#define SUCCESS 1
#define NOT_SUCCESS 0

//Implementação da fila.

typedef struct lista{
	
	char palavra[50];
	int hash;
	struct lista *proximo;
}lista;

void insertList(lista **topo, char *palavra, int numberOfHash){
	
	lista *novo = (lista*) malloc(sizeof(lista));
	strcpy(novo->palavra, palavra);
	novo->hash = numberOfHash;
	
	if(*topo == NULL){
		
		novo->proximo = NULL;
		*topo = novo;
	}else{
		
		novo->proximo = *topo;
		*topo = novo;
	}
}

void removeList(lista **topo, FILE *archive){
	
	unsigned int i = 0;
	
	if(*topo == NULL){
		
		fprintf(archive, "\nNum. Ocorrencia - Palavra\n");
		fprintf(archive, "-----------------------------------------\n");
	}else{
		
		lista *aux = *topo;
		
		fprintf(archive, "\nNum. Ocorrencia - Palavra\n");
		fprintf(archive, "-----------------------------------------\n");
		
		for(i = 1; aux != NULL; aux = aux->proximo, i++){
			
			fprintf(archive, "%d - %s", i, aux->palavra);
		}
	}
}

struct Node{
	
	char palavra[50];
	struct Node *next;
};

typedef struct Node* hashTable[HASH_MAX_BUCKETS];

void initHashTable(hashTable table){
	
	unsigned int i = 0;
	
	for(i = 0; i < HASH_MAX_BUCKETS; i++) table[i] = NULL;
}

void insertElement(hashTable table, char *word, int hash){
	
	if(table[hash] == NULL){
		
		table[hash] = (struct Node*) malloc(sizeof(struct Node));
		strcpy(table[hash]->palavra, word);
		table[hash]->next = NULL;
	}else{
		
		struct Node* newNode = table[hash];
		while(newNode->next != NULL) newNode = newNode->next;
		
		newNode->next = (struct Node*) malloc(sizeof(struct Node));
		strcpy(newNode->next->palavra, word);
		newNode->next->next = NULL;
	}
}

unsigned int hashFunction(char *word, int maxSize){
	
	unsigned int i = 0, hash = 0;
	
	for(i = 0; i < maxSize; i++){
		
		hash += word[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	
	hash += (hash << 3);
  	hash ^= (hash >> 11);
  	hash += (hash << 15);
  	
  	return (hash % NUMBER_OF_MOD);
}

unsigned int searchHashTable(hashTable table, int hash, char* palavra){
	
	struct Node* newNode = table[hash];
	
	if(newNode != NULL){
		
		while(newNode != NULL && (unsigned int) strcmp(newNode->palavra, palavra)){
			
			newNode = newNode->next;
		}
		
		if(newNode == NULL){
			
			return NOT_SUCCESS;
		}else{
			
			return SUCCESS;
		}
		
	}else{
		
		return NOT_SUCCESS;
	}
}

int main(void) {
	
	FILE *dictionary;
	dictionary = fopen("dicionario.txt", "rt");
	
	lista *no = NULL;
	char palavra[50] = {};
	unsigned int hashValue = 0, countWords = 0, faliedHashWord = 0;
	
	time_t first = 0, last = 0;
	double resultTime = 0;
	
	hashTable hash;
	initHashTable(hash);
	
	while(!feof(dictionary)){
		
		fgets(palavra, 50, dictionary);
		insertElement(hash, palavra, hashValue = hashFunction(palavra, strlen(palavra)));
	}
	
	fclose(dictionary);
	
	dictionary = fopen("entrada.txt", "r");
	
	first = clock();
	
	while(!feof(dictionary)){
		
		fgets(palavra, 50, dictionary);
		hashValue = hashFunction(palavra, strlen(palavra));
		
		if(!searchHashTable(hash, hashValue, palavra)){
			
			faliedHashWord++;
			insertList(&no, palavra, hashValue);
		}
		
		countWords++;
	}
	
	last = clock();
	
	resultTime = (((last - first) * 1000) / CLOCKS_PER_SEC);
	fclose(dictionary);
	
	dictionary = fopen("results.txt", "w");
	
	fprintf(dictionary, "Numero total de palavras do texto: %d\n", countWords);
	fprintf(dictionary, "Tempo total da verificação: %dms\n", (int) resultTime);
	fprintf(dictionary, "Numero de palavras que falharam no spell check: %d\n", faliedHashWord);
	fprintf(dictionary, "Lista de palavras que falharam no spell check:\n");
	
	removeList(&no, dictionary);
	
	fclose(dictionary);

	return 0;
}
