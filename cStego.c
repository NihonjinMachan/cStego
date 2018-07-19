#include <stdio.h>
#include <stdlib.h>
#include <mem.h>

#define COMMENTMAX 5
#define COMMENTSIZE 39

typedef struct {
    int red,green,blue;
} Pixel;

typedef struct {
    char type[2];
    char comment[COMMENTMAX][COMMENTSIZE];
    int width, height;
    int max;
    Pixel* pixelData;
} PPM;

//method for reading data from PPM file and storing in a struct
PPM * getPPM(FILE * fin)
{
    PPM *img;

    if (fin == NULL) {
        printf("Unable to open file \n");
        exit(1);
    }

    //allocate memory for image
    img = (PPM *)malloc(sizeof(PPM));
    if (!img) {
        printf("Unable to allocate memory\n");
        exit(1);
    }

    //read image type
    fscanf(fin,"%2s\n",img->type);
    if(strcmp(img->type,"P3") != 0){
        printf("Not in P3 format");
        exit(1);
    }

    //reading comments
    int i=0;
    while (i<COMMENTMAX){
        fscanf(fin," #%40[^\n]",img->comment[i]); //pattern matching
        i++;
    }

    //read image size information
    fscanf(fin,"%d %d", &img->width, &img->height);

    //read rgb max value
    fscanf(fin,"%d", &img->max);

    //memory allocation f or pixel data
    int pixCount = img->width * img->height;
    img->pixelData = (Pixel*)malloc(pixCount * sizeof(Pixel));

    if(!img->pixelData){
        printf("Unable to allocate memory \n");
        exit(1);
    }

    //reading pixel data
    int loop=0;
    while(loop<pixCount){
        fscanf(fin,"%d %d %d",&img->pixelData[loop].red,&img->pixelData[loop].green,&img->pixelData[loop].blue);
        loop++;
    }

    fclose(fin);
    return img;
}

//method for printing PPM image.
void showPPM(PPM * i){
    //image format
    printf("P3 \n");

    //comments
    int j=0;
    while(j<COMMENTMAX){
        if(strlen(i->comment[j])<2){
            break;
        }
        printf("#%s \n",i->comment[j]);
        j++;
    }

    //image size
    printf("%d %d \n",i->width,i->height);

    //rgb max value
    printf("%d\n",i->max);

    //pixel data
    int pixCount= i->width * i->height;
    int printer;
    for(printer=0;printer<pixCount;printer++){
        printf("%d %d %d \n",i->pixelData[printer].red,i->pixelData[printer].green,i->pixelData[printer].blue);
    }
}

//comparator function for qsort
int cmpfunc (const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}

//method for encoding text into PPM image
PPM * encode(char * text, PPM * i){
    PPM * output = i;
    int textLength = strlen(text);
    int pixCount = i->height * i->width;
    int count = 0;
    if(textLength > pixCount){
        printf("The image cannot hold your sentence.");
        exit(1);
    }
    int randArray[pixCount]; //creating sorted array of random numbers
    srand(1);
    int j=0;
    int x = rand()%pixCount+1;
    while(j < textLength){
        randArray[j]=x;
        x = rand()%pixCount+1;
        count++;
        j++;
    }
    qsort(randArray,count,sizeof(int),cmpfunc);

    j=0;
    while(j<textLength){
        if(output->pixelData[randArray[j]].red == text[j]){  //if red value and text ASCII are same find another pixel
            randArray[count++]=rand()%pixCount+1;
            qsort(randArray,count,sizeof(int),cmpfunc);
            output->pixelData[randArray[count]].red = text[j];
        }
        else{
            output->pixelData[randArray[j]].red = text[j];
        }
        j++;
    }
    return output;
}

//method to write to a file
void writePPM(PPM *img, FILE * f)
{
    //image format
    fprintf(f,"P3 \n");

    //comments
    int j=0;
    while(j<COMMENTMAX){
        if(strlen(img->comment[j])<1){
            break;
        }
        fprintf(f,"#%s \n",img->comment[j]);
        j++;
    }

    //image size
    fprintf(f, "%d %d\n",img->width,img->height);

    //rgb max value
    fprintf(f, "%d\n",img->max);

    // pixel data
    int pixCount= img->width * img->height;
    int printer;
    for(printer=0;printer<pixCount;printer++){
        fprintf(f,"%d %d %d \n",img->pixelData[printer].red,img->pixelData[printer].green,img->pixelData[printer].blue);
    }

    fclose(f);
}

//method to decode the message by comparing images
char * decode(PPM * original, PPM * input){
    char * result;
    int pixCount = input->height * input->width;
    int * temp = malloc(pixCount * sizeof(int));
    int j = 0;
    int strlength = 0;
    while(j<pixCount){
        if(original->pixelData[j].red != input->pixelData[j].red){
            temp[strlength] = input->pixelData[j].red;
            strlength++;
        }
        j++;
    }
    result = malloc(strlength * sizeof(char));
    j = 0;
    while(j<strlength){
        result[j] = temp[j];
        j++;
    }
    result[j]='\0';
    return result;
}


int main(int argc,char ** argv){

    if(argv[1][0] == 'e'){ //encoding
        PPM *image1;
        FILE *f = fopen(argv[2], "r+");
        image1 = getPPM(f);
        PPM * encoded;
        char input[100];
        printf("Type the message you want to encode: \n");
        fgets (input, 100, stdin);
        encoded = encode(input,image1);
        FILE * fout = fopen(argv[3], "w+");
        writePPM(encoded,fout);
        printf("Displaying encoded PPM image \n");
        showPPM(encoded);
    }

    else if(argv[1][0] == 'd'){ //decoding
        PPM *image2;
        FILE *f = fopen(argv[2], "r+");
        image2 = getPPM(f);
        PPM * input;
        FILE * fin = fopen(argv[3],"r+");
        input = getPPM(fin);
        char * final;
        final = decode(image2,input);
        printf("The hidden message is: %s \n",final);
    }

    else{ //error case
        printf("Wrong input. \n");
        exit(1);
    }
}