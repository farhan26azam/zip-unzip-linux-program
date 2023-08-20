// Huffman Coding in C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_TREE_HT 50
#define SIZE 20
FILE * zipped;
pthread_mutex_t * locks;
int num_threads = 0;
int sizeOfData;
char zipped_name[30] ="zipped_";
char enc_file[30];

typedef struct _threadarg{
	int start;
	int index;
	int end;
	int sizeOfChunk;
	char* subData;
} threadarg;


// #define BYTE_TO_BINARY_PATTERN "%c"
// #define BYTE_TO_BINARY(byte)  \
//   (byte & 0x01 ? '1' : '0') 

// typedef struct _binary
// {
//     unsigned char first_bit:1;
// }binary;

// typedef struct{
// 	unsigned int x:1;
// } uint1_t;

//*********************************************** HUFFMAN TREE FUNCTIONS *******************************************************
void printArray(int* , int );


struct MinHNode {
  char item;
  unsigned freq;
  struct MinHNode *left, *right;
};

struct MinHeap {
  unsigned size;
  unsigned capacity;
  struct MinHNode **array;
};

// Create nodes
struct MinHNode *newNode(char item, unsigned freq) {
  struct MinHNode *temp = (struct MinHNode *)malloc(sizeof(struct MinHNode));

  temp->left = temp->right = NULL;
  temp->item = item;
  temp->freq = freq;

  return temp;
}

// Create min heap
struct MinHeap *createMinH(unsigned capacity) {
  struct MinHeap *minHeap = (struct MinHeap *)malloc(sizeof(struct MinHeap));

  minHeap->size = 0;

  minHeap->capacity = capacity;

  minHeap->array = (struct MinHNode **)malloc(minHeap->capacity * sizeof(struct MinHNode *));
  return minHeap;
}

// Function to swap
void swapMinHNode(struct MinHNode **a, struct MinHNode **b) {
  struct MinHNode *t = *a;
  *a = *b;
  *b = t;
}

// Heapify
void minHeapify(struct MinHeap *minHeap, int idx) {
  int smallest = idx;
  int left = 2 * idx + 1;
  int right = 2 * idx + 2;

  if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
    smallest = left;

  if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
    smallest = right;

  if (smallest != idx) {
    swapMinHNode(&minHeap->array[smallest], &minHeap->array[idx]);
    minHeapify(minHeap, smallest);
  }
}

// Check if size if 1
int checkSizeOne(struct MinHeap *minHeap) {
  return (minHeap->size == 1);
}

// Extract min
struct MinHNode *extractMin(struct MinHeap *minHeap) {
  struct MinHNode *temp = minHeap->array[0];
  minHeap->array[0] = minHeap->array[minHeap->size - 1];

  --minHeap->size;
  minHeapify(minHeap, 0);

  return temp;
}

// Insertion function
void insertMinHeap(struct MinHeap *minHeap, struct MinHNode *minHeapNode) {
  ++minHeap->size;
  int i = minHeap->size - 1;

  while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
    minHeap->array[i] = minHeap->array[(i - 1) / 2];
    i = (i - 1) / 2;
  }
  minHeap->array[i] = minHeapNode;
}

void buildMinHeap(struct MinHeap *minHeap) {
  int n = minHeap->size - 1;
  int i;

  for (i = (n - 1) / 2; i >= 0; --i)
    minHeapify(minHeap, i);
}

int isLeaf(struct MinHNode *root) {
  return !(root->left) && !(root->right);
}

struct MinHeap *createAndBuildMinHeap(char item[], int freq[], int size) {
  struct MinHeap *minHeap = createMinH(size);

  for (int i = 0; i < size; ++i)
    minHeap->array[i] = newNode(item[i], freq[i]);

  minHeap->size = size;
  buildMinHeap(minHeap);

  return minHeap;
}

struct MinHNode *buildHuffmanTree(char item[], int freq[], int size) {
  struct MinHNode *left, *right, *top;
  struct MinHeap *minHeap = createAndBuildMinHeap(item, freq, size);

  while (!checkSizeOne(minHeap)) {
    left = extractMin(minHeap);
    right = extractMin(minHeap);

    top = newNode('$', left->freq + right->freq);

    top->left = left;
    top->right = right;

    insertMinHeap(minHeap, top);
  }
  return extractMin(minHeap);
}

void printHCodes(struct MinHNode *root, int arr[], int top) {
  if (root->left) {
    arr[top] = 0;
    printHCodes(root->left, arr, top + 1);
  }
  if (root->right) {
    arr[top] = 1;
    printHCodes(root->right, arr, top + 1);
  }
  if (isLeaf(root)) {
    fprintf(zipped, "%c~", root->item);	//printing to file
    printf("%c~",root->item);		//printing to screen
    printArray(arr, top);
  }
}

// Wrapper function
void HuffmanCodes(char item[], int freq[], int size) {

  struct MinHNode *root = buildHuffmanTree(item, freq, size);

  int arr[MAX_TREE_HT], top = 0;

  printHCodes(root, arr, top);
}

// Print the array
void printArray(int arr[], int n) {
  int i;
  for (i = 0; i < n; ++i)
  {
    fprintf(zipped, "%d", arr[i]);	//printing to file
    printf("%d", arr[i]);		//printing to screen
  }

  fprintf(zipped, "\n");
  printf("\n");
}
// HUFFMAN TREE CODE ENDS



//*********************************************** ENCODING/ZIP FUNCITON *******************************************************

void * Encode(void * args)
{

	threadarg* arg = (threadarg*) args;
	int sizeOfUniq = arg->sizeOfChunk;
	char* data1 = arg->subData;
	

	pthread_mutex_lock(&locks[arg->index]);

	// int store[8]={0,0,0,0,0,0,0,0};
	// char store_char[8]={};
	// struct binary num;
	// binary * bit_store = (binary*) malloc(sizeof(binary)*4);

	// char numbers[6]={};
	// unsigned int num_store=0;
	// int leading_zero_bool=0;
	// int store_count=0;
	FILE* ip=fopen(zipped_name,"ab+");					//opening zipped file
	for(int u = 0; u < sizeOfUniq; u++)
	{	
		FILE* encd = fopen(enc_file,"rb");
		char line [50];
    		while(fgets(line,sizeof line,encd)!= NULL)
    		 {
    		 	if(line[0] == data1[u])
    		 	{
    		 		int w = 2;
					
    		 		while(line[w] != '\n' && line[w] != '\0')
    		 		{
						// store[store_count]=((int)line[w])-48;
						// store_count++;
						// if(store_count==7){
						// 	for(int g= 0; g<8; g++){
						// 		store_char[g] = store[g];
						// 	}
						// 	fprintf()
						// }

						// numbers[store_count] = line[w];
						// store_count++;
						// printf("index : %d stores %c\n", store_count, line[w]);
						// if(store_count >= 6 || (line[w+1]=='\n' && line[w+1] =='\0')){

						// 	printf("In condition.\n");

						// 	for(int k=0; k < store_count; k++){
						// 		if(numbers[k]=='0' && leading_zero_bool==0){
						// 			fprintf(ip,"%c", numbers[k]);
						// 			printf("Leading zero : %c value of k: %d\n", numbers[k], k);


						// 		}else{
						// 			leading_zero_bool = 1;
						// 			num_store = atoi(numbers);
						// 			fprintf(ip,"%d", num_store);
						// 			printf("others : %d\n", num_store);
						// 			for(int f=k;f<store_count;f++){
						// 				numbers[f] = 0;
						// 			}
						// 			leading_zero_bool = 0;
						// 			k = store_count;
						// 			store_count=0;
						// 		}
						// 	}
						// }
						//printf("size : %lu\n",sizeof(unsigned int));
						// bit_store[0].first_bit = ((int)line[w])-48;
						// //printf("Size of num.x : %lu\n", sizeof(struct binary));
						// printf("%u\n",bit_store[0].first_bit);

						fprintf(ip,"%c", line[w]);

						// fwrite(&bit_store, sizeof(binary), 1, ip);


    		 			//fprintf(ip,"%u",num.x);

						//int buffer = (int)line[w]-48;
    		 			//fwrite(&buffer,sizeof(buffer),1,ip);
    		 			w++;
    		 		}
    		 	}
    		 }
    		 fclose(encd);
	}
	fclose(ip);
	
	
	if (arg->index<num_threads-1){
		pthread_mutex_unlock(&locks[arg->index+1]);
	}	
}


//********************************************************* MAIN *************************************************************
int main(int argc, char *argv[]) {

// Checking if arguments do not satisfy given format
if(argc<2 || argc > 4)
{
	printf("Error: Incorrect arguments\n %s \n Syntax: <input-file> <number of threads> <output file name> \n",argv[0]);
	return -1;
}


if(argc==4)
{
  num_threads = atoi(argv[2]);
  char input_name[50];
  strcpy(input_name,argv[1]);
  strcat(zipped_name,input_name); 		         //zipped_name is the compressed/encoded file
}
char ch;FILE *fp;
fp=fopen(argv[1],"rb");					//opening input file
if(fp==NULL)
{
	printf("[!]Input file cannot be opened.\n");
	return -1;
}

char* unique;
int* frequency;
int uns = 50;
unique = (char*) malloc(sizeof(char) * uns);
int v = 0;
char * data = (char*) malloc(sizeof(char)* SIZE*10);
int i = 0; 
printf("\nProcess Started.\n");
printf("\nReading input file: %s\n",argv[1]);		//reading input file
do{				
	ch = fgetc(fp);
	data[i] = ch;
	if(i == 0)
	{
		if(v>= uns){
			unique = realloc(unique, uns*10);
		}
		unique[v] = ch; v++; 
	}
	else if (data[i] != data[i-1])				//if not similar to previous, check all previous to see uniqueness
	{	char temp = data[i]; int flag = 0;
		for(int u = 0; u < i-2; u++) {
			if(temp == data[u]) { flag = 1; break;  }	
		}
		if(flag == 0) { unique[v] = ch; v++;}

	}
	i++;
	if(i>=SIZE*10){
		data = realloc(data, sizeof(char) * i*10);
	}
   } while (ch != EOF);
sizeOfData = i-2;	
int uniq_size = v-2;
frequency = (int*) malloc(sizeof(int) * uniq_size);

for(int  i = 0; i < uniq_size; i++)  //initialize count to 0
{
	frequency[i] = 0;
}

//******************************************** FREQUENCY CALCULATION *************************************************************
printf("\nCalculating Frequency...\n");
for(int n = 0; n < uniq_size; n++) 		//counting unique characters
{
	char temp = unique[n];
	for(int i = 0; i < sizeOfData; i++)
	{
		if(data[i] == temp)
		{
			frequency[n] = frequency[n] +1;
		}
	}
	
	//printf("Unique: %c occurs: %d\n",unique[n], frequency[n]);		//printing frequency array

}
/*for(int j = 0; j < sizeOfUniq; j++)
{
	printf("DATA: %c \n",data[j]);
}*/
//***************************************** HUFFMAN TREE CREATION ****************************************************************
	printf("\nGenerating Codes using Huffman Tree\n");
	// adding extension to encoding file
	char ext[]= "_encoding.bin";
	strcpy(enc_file, argv[3]);
	strcat(enc_file, ext);
	printf("\n--------------------------------------------------------------\n");
	printf("Huffman codes written to output file: %s", enc_file);
	printf("\n--------------------------------------------------------------\n");
	zipped = fopen(enc_file,"w");
	HuffmanCodes(unique, frequency, uniq_size);
	fclose(zipped);
	
//****************************************** ENCODE USING THREADING ***************************************************************

	printf("\nGenerating zip file...\n");
	FILE* ip1=fopen(zipped_name,"w");
	fclose(ip1);
	threadarg* marg = (threadarg*) malloc(sizeof(threadarg) * num_threads);	
	pthread_t* threads =(pthread_t*) malloc(sizeof(pthread_t) * num_threads);	
	

	
	locks = malloc(sizeof(pthread_mutex_t)*num_threads);
	int rc;
	//initializing all locks and locking them
	for(int i =0; i< num_threads; i++){
		rc = pthread_mutex_init(&locks[i], NULL);
		assert(rc==0);
		pthread_mutex_lock(&locks[i]);
	}

	pthread_mutex_unlock(&locks[0]);
	
	
	int chunk = ceil((float)sizeOfData/num_threads);
	//printf("Number of Threads: %d\n", num_threads);	
	//printf("Data size: %d\n", sizeOfData);
	//printf("Chunk size: %d\n", chunk);
	
	for(int i =0;i < sizeOfData; i = i + chunk){
		int index = (int)i/chunk;
		//printf("Index: %d\n", index);
		marg[index].start = i;
		marg[index].end = i + chunk < sizeOfData ? i + chunk : sizeOfData;
		marg[index].index = index;
		//printf("start %d, end %d\n", marg[index].start, marg[index].end);
	}
	
	for(int i =0;i < num_threads; i++){
		int tempsize = 0;
		char* partOfData = &(data[marg[i].start]);
		tempsize = marg[i].end - marg[i].start;		
		marg[i].sizeOfChunk = tempsize;
		marg[i].subData = partOfData;
		if(tempsize == 0) {break;}		
		//printf("\nSize: %d\nData array chunk: ",marg[i].sizeOfChunk);
		//for(int k = 0; k < tempsize; k++) {printf("%c  ",partOfData[k]);}
		pthread_create(&threads[i], NULL, Encode, &marg[i]);	
	}
	
	for (int i =0; i< num_threads; i++){
		pthread_join(threads[i], NULL);
	}
	

	printf("\nProcess completed. Zipped file generated: %s\n\n", zipped_name);
	free(threads);
	free(unique);
	free(data);
	free(frequency);
	
}








