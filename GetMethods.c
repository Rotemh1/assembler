#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "assmbler.h"

/* arrays and variables for reads files */
extern const command commandsArr[];
extern labelInfo labelArr[];
extern int numberOfLabels;
lineInfo entryLines[MAX_LABELS_NUM];
int numberOfEntry;

/* Methods */

char* allocString(const char* str);
char* getFirstOperand(char* line, char** endOfOp, int* foundComma);


void remove_spaces(char* s) 
{
	char* d = s;
	do {
		while (*d == ' ') {
			++d;
		}
	} while ((*s++ = *d++));
}
/* malloc for the new string returns pointer to the new string*/
char* allocString(const char* str)
{
	char* newStr = (char*)malloc(strlen(str) + 1);
	if (newStr)
	{
		strcpy(newStr, str);
	}

	return newStr;
}

/* Returns a pointer first operand in 'line' and puts '\0' at the end. */
/* Also makes *endOfOp point at the next char after the operand. */
char *getFirstOperand(char *line, char **endOfOp, int *foundComma)
{
	if (!isWhiteSpaces(line))
	{
		/* Find the first comma */
		char *end = strchr(line, ',');
		if (end)
		{
			*foundComma = 1;
			*end = '\0';
			end++;
		}
		else
		{
			*foundComma = 0;
		}

		if (endOfOp)
		{
			if (end)
			{
				*endOfOp = end;
			}
			else
			{
				*endOfOp = strchr(line, '\0');
			}
		}
	}

	deleteAllSpaces(&line);
	return line;
}


/* return pointer to label if it is a cutlabel label 
labelInfo *getLabelIfCutLabel(char *strLabel)
{
	char *label;
	int i = 0;
        char str[MAX_LINE_LENGTH];
        strcpy(str,strLabel);		 copy to avoid from change the original label 
	label=strtok(str,":");		 get label name 
	while(!strcmp(label,labelArr[i].name))
	{
		i++;
	}
	return getLabel(label);
}*/

/* Removes all the spaces from the beginning and the end of the string. */
void deleteAllSpaces(char **str)
{
	char *eos;  /* end of string */

	/* Return if it's NULL or empty string */
	if (!str || **str == '\0')
	{
		return;
	}

	spaceRemover(str);

	/* oes is pointing to the last char in str. */
	eos = *str + strlen(*str) - 1;

	/* Remove spaces from the end */
	while (isspace(*eos) && eos != *str)
	{
		*eos-- = '\0';
	}
}
/* Removes spaces from start of the string */
void spaceRemover(char **str)
{
	/* Return if it's NULL */
	if (!str)
	{
		return;
	}

	/* Get str to the start of the actual text */
	while (isspace(**str))
	{
		++*str;
	}
}



/* Returns a pointer to the start of first token. */
/* Also makes *endOfTok pointer to the last char after the token. */
char *getFirstTok(char *str, char **endOfTok)
{
	char *tokStart = str;
	char *tokEnd = NULL;

	/* skip spaces from strat */
	spaceRemover(&tokStart);

	/* get the end of the first word */
	tokEnd = tokStart;
	while (*tokEnd != '\0' && !isspace(*tokEnd))
	{
		tokEnd++;
	}

	/* Add \0 at the end if needed */
	if (*tokEnd != '\0')
	{
		*tokEnd = '\0';
		tokEnd++;
	}

	/* Make *endOfTok (if it's not NULL) to point at the last char after the token */
	if (endOfTok)
	{
		*endOfTok = tokEnd;
	}
	return tokStart;
}

/* returns the start of the bits range if isStart is 0 or the end if 1 */
int getRange(char *strLabel,int isStart)
{
        char *startRange, *endRange;
        int startRangeInt, endRangeInt;
        char str[MAX_LINE_LENGTH];
        strcpy(str,strLabel);		/* copy to avoid from change the original string */
        startRange=strtok(str,"["); 
	startRange=strtok(NULL,"-");	/* the start of the range */
	endRange=strtok(NULL,"]");	/* the end of the range */
	startRangeInt=atoi(startRange);
	endRangeInt=atoi(endRange);
	if(isStart)
		return startRangeInt;
	else return endRangeInt;    
}

/* Returns the command in cmdArr or -1 if there isn't such command. */
int getCmdId(char* cmdName)
{
	int i = 0;

	while (commandsArr[i].name)
	{
		if (strcmp(cmdName, commandsArr[i].name) == 0)
		{
			return i;
		}

		i++;
	}
	return -1;
}

/* Returns a pointer to the label with 'labelName' name in labelArr or NULL if there isn't such label. */
labelInfo *getLabel(char *labelName)
{
	int i = 0;

	if (labelName)
	{
		for (i = 0; i < numberOfLabels; i++)
		{
			if (strcmp(labelName, labelArr[i].name) == 0)
			{
				return &labelArr[i];
			}
		}
	}
	return NULL;
}

