#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "assmbler.h"

char hex_int_to_char(int hex);
void createObjectFile(const char* name, int IC, int DC);
void createEXfile(const char* name);
void createENfile(const char* name);
void Decimal_to_Binary(int number, int bits, int* bin);
char Bin_To_Hex(int* bin);
void printmemArr(FILE* file, int IC);
void Two_complement(int bits, int* bin);
void binAdd1(int bits, int* bin);
FILE* openFile(const char* name, char* ending, const char* mode);

/*creates extern file*/
void createEXfile(const char* name)
{
    int i, j;
    int extcount = 0;
    FILE* file;
    for (i = 0; i < numberOfDirectives; i++)
        if (directivesArr[i].type == EX)
            extcount++;
    if (extcount > 0)
    {
        file = openFile(name, ".ext", "w");
        for (i = 0; i < numberOfDirectives; i++)
        {
            if (directivesArr[i].type == EX)
                for (j = 0; j < directivesArr[i].Exsize; j++)
                {
                    fprintf(file, "%s \t %d \n", directivesArr[i].name, directivesArr[i].EXaddress[j]);
                }
        }
        fclose(file);
    }
}

/*creates extern file*/
void createENfile(const char* name)
{
    int i;
    FILE* file;
    int entcount = 0;
    for (i = 0; i < numberOfDirectives; i++)
        if (directivesArr[i].type == EN)
            entcount++;
    if (entcount > 0)
    {
        file = openFile(name, ".ent", "w");
        for (i = 0; i < numberOfDirectives; i++)
        {
            if (directivesArr[i].type == EN)
                fprintf(file, "%s \t %d \n", directivesArr[i].name, directivesArr[i].address);
        }
        fclose(file);
    }
}

void binAdd1(int bits, int* bin)
{
    int b;
    int sum;
    int i = 0;
    sum = 0;
    for (i = 0; i < bits; i++)
    {
        b = 0;
        if (bin[i] == 1)
            b = pow(2, i);
        sum += b;
    }
    sum++;
    Decimal_to_Binary(sum, bits, bin);
}

void Two_complement(int bits, int* bin)
{
    int i = 0;
    for (i = 0; i < bits; i++)
    {
        if (bin[i] == 1)
            bin[i] = 0;
        else
        {
            if(bin[i] == 0)
            bin[i] = 1;
        }
    }
    binAdd1(bits, bin);
}

/*prints line to OB file*/
void printline(FILE* file, int* line)
{
    int j;
    char c;
    for (j = 0; j < 4; j++)
    {

        c = Bin_To_Hex(&line[(j * 8) + 4]);
        fprintf(file, "%c", c);
        c = Bin_To_Hex(&line[(j * 8)]);
        fprintf(file, "%c", c);
        fputc(' ', file);
    }

}

/* prints lines of OB file*/
void printmemArr(FILE* file,int IC)
{
    int lineIC;
    int i = 0;
    lineIC = 100;
    while ((i + 32) < ((IC - 100) * 8))
    {
        fputc('\n', file);
        fprintf(file, "%d \t", lineIC);
        printline(file, &memArr[i]);
        lineIC += 4;
        if ((IC - lineIC) < 4)
            break;
        i += 32;
    }
}

/* Creates a file (for writing) from a given name and ending, and returns a pointer to it. */
FILE* openFile(const char* name, char* ending, const char* mode)
{
    FILE* file;
    char* mallocStr = (char*)malloc(strlen(name) + strlen(ending) + 1), * fileName = mallocStr;
    sprintf(fileName, "%s%s", name, ending);

    file = fopen(fileName, mode);
    free(mallocStr);

    return file;
}

/*creates and writes .OB file*/
void createObjectFile(const char* name, int IC, int DC)
{
    FILE* file;
    file = openFile(name, ".ob", "w");
    /* Print IC and DC */
    fprintf(file, "%d", (IC - DC - 96));
    fprintf(file, "\t\t");
    fprintf(file, "%d", DC);
    printmemArr(file, IC);
    fclose(file);
}

/* converts dec number to binary code of bits number of digits and pushes it to an arry */
void Decimal_to_Binary(int number, int bits, int* bin)
{
    int i;
    int j;
    for (i = (bits -1); i >= 0 ; i--)
    {
        j = number >> i;

        if (j & 1)
        {
            bin[i] = 1;
        }
        else
        {
            bin[i] = 0;
        }
    }

}

/* converts 4 bits of bin code to hex number*/
char Bin_To_Hex(int* bin)
{
    int i;
    char chrhex;
    double hex = 0;
    int b = 0;
        for (i = 0; i < 4; i++)
        {
            b = 0;
            if (bin[i] == 1)
            {
                b = pow(2, i);
            }
            hex = hex + b;
        }
        chrhex = hex_int_to_char(hex);

    return chrhex;
}

/* gets int betweeen 0-15 and convert it to hex*/
char hex_int_to_char(int hex)
{
        char ch = 0;
        if (hex < 10)
                ch = hex + 48;
        if (hex >= 10)
                ch = hex + 55;
        return ch;
}

int isAsfile(const char* ch, char* name)
{
    int len = strlen(ch);
    int i;
    name = (char*)realloc(name, sizeof(char) * len);
    for(i=0;i<len - 1;i++)
        if (!strcmp(".as", &ch[i]))
        {
            strcpy(name, ch);
            name[i] = 0;
            return 1;
        }
    return 0;
}

int main(int argc, const char** argv)
{
    int i = 1;
    int IC = 100;
    int DC = 0;
    char* name = (char*)malloc(sizeof(char));
    FILE* f;
    while (i < argc)
    {
        if (isAsfile(argv[i], name))
        {
            f = openFile(name, ".as", "r");
            firstRun(f, &IC, &DC);
            createObjectFile(name, IC, DC);
            createENfile(name);
            createEXfile(name);
            IC = 100;
            DC = 0;
        }
        i++;
    }
    free(name);
    return 0;
}
