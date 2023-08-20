
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SIZE 20

int main(int argc, char * argv[])
{
    if(argc < 2 || argc > 4){
        printf("Insufficient arguments. Format is <zipped file name> <number of threads> <output file name>");
        exit(1);
    }
    printf("\nProcess Started.\n");
    char arg1[70];
    char name[50];
    strcpy(arg1,argv[1]);
    
    int k = 0;
    for(int t = 7; t < 70; t++)
    {
    	if(arg1[t] == '.') {break;}
    	name[k] = arg1[t];
    	k++;

    }
    name[k] = '\0';
    //printf("Filename: %s\n", name);
    
    char encod[50];
    strcpy(encod,name);
    strcat(encod,"_encoding.bin");
    printf("\nReading Encoded file: %s...\n\n", encod);
    FILE * encd= fopen(encod, "rb");	
	if(encd==NULL){
        printf("Can not find encoded file!\n");
        exit(1);
    }
	char elem[50];			//OLD ZIPPING
	int e = 0;
	char code[50][20];
	int fd = 0;
	//printf("\nReading Encode Bin file: %s\n",enc_file);		//reading input file
    
    int added_flag=0;
	if (encd != NULL) {
    		char line [60];
    		while(fgets(line,sizeof line,encd)!= NULL)
    		 {
    		 	elem[e] = line[0]; e++;
    		 	int o = 2;
    		 	for (int p = 0; p < 20; p++)
    		 	{
    		 		if(line[o] == '\0' && line[o] == '\n') {
                        break;
                    }
                    if(line[o]== '\n'){
                        line[o] = 0;
                    }
                    code[fd][p] = line[o];
				    o++;
    		 	}
                fd++;
    		 }
	
    	fclose(encd);
  	}

	// for(int i = 0; i < e; i++)
	// {
	// 	//printf("element: %c  code: %s at index %d\n",elem[i],code[i], i);
	// }
    // int min_code_size=0;
    // for(int n=0; n< 20; n++){
    //     if(code[0][n]!='0' && code[0][n]!='1'){
    //         break;
    //     }
    //     min_code_size++;
    // }
    

    //printf("Min code size: %d\n", min_code_size);
    // now characters are in elem array and codes are in code array
    char * arg3 = argv[1];
    //char ext3[12] = "zipped_";
    //strcat(ext3,arg3);
    printf("Decoding zipped file: %s...\n\n", arg3);

    FILE * zip_read = fopen(arg3, "rb");
    char * zipped_text = (char*) malloc(sizeof(char)* SIZE*10);
    char * unzipped_text = (char *) malloc(sizeof(char)* SIZE*100);
    char iterator='0';
    int mycount=0;
    while(iterator == '0' || iterator == '1'){
        iterator = fgetc(zip_read);
        zipped_text[mycount] = iterator;
        //printf("%c", zipped_text[mycount]);
        mycount++;
        //printf("my count: %d\n", mycount);
    }

    char uzname[50];
    strcpy(uzname, name);
    strcat(uzname, "_unzipped.txt");
    printf("Generating unzipped file...\n\n");
    FILE * unzip = fopen(uzname, "w");
    
    
    int found_counter = 1;

    //     for(int i = 0; i < e; i++)
	// {
	// 	printf("element: %c  at index %d\n",elem[i], i);
	// }

    //char * one_ch= (char*) malloc(sizeof(char));
    for(int i = 0; i< mycount-1; i++){
        int found=0;
        //int all_found= 0;
       
        while(found==0){
            char it_adder[50]={};
            for(int x=0;x<found_counter;x++){
                //printf("i -->: %d\n", i);
                char temp = zipped_text[i+x];
                //printf("temp: %c\n", temp);
                //one_ch[0]= temp;
                //printf("one_ch: %c", one_ch[0]);
                strncat(it_adder,&temp,1);
                //printf("it_adder ---> :%s\n",it_adder);
            }
            for(int j =0; j<e; j++){
                //printf("code[%d]: %s is not same as %s\n",j,  code[j], it_adder);
                int compare=1;

                for(size_t p= 0;p<strlen(code[j]); p++){
                    if(it_adder[p] != code[j][p]){
                        compare = 0;
                        //printf("Not matched\n");
                        break;
                    }else{
                        compare = 1;
                    }
                }
                
                if(compare==1){
                    //printf("ELEMENT: %c\n", elem[j]);
                    //printf("Value of j is: %d\n", j);
                    found = 1;
                    //printf("\nHERE found\n\n");
                    
                    //printf("---------------------------------------------------------\n");
                    fprintf(unzip, "%c", elem[j]);
                    // for(int x=0;x<found_counter;x++){
                    //     zipped_text[i+x]=0;
                    // }
                    i+= found_counter-1;
                    //it_adder[]= {};
                    break;
                }
            }
            //printf("COMES OUT\n");
            if(found==0){
                found_counter++;
            }else{
                found_counter= 1;
            }
        }
    }
    fclose(zip_read);
    fclose(unzip);
    free(zipped_text);
    printf("Process completed. Unzipped file generated: %s\n\n", uzname);
}
    



