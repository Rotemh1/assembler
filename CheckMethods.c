#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "assmbler.h"

/* arrays and variables for reads files */
extern const command commandsArr[];
extern labelInfo labelArr[];
extern int numberOfLabels;
extern lineInfo entryLines[];
extern int numberOfEntry;

/* Methods */

/* one word check */
int isOneWord(char *str)
{
	spaceRemover(&str);				/* Skip the spaces from the beginning*/
	while (!isspace(*str) && *str) { str++; }	/* skip the text from middle */
	/* Return if it's the end of the text */
	return isWhiteSpaces(str);
}

/* Returns if str contains only white spaces. */
int isWhiteSpaces(char *str)
{
	while (*str)
	{
		if (!isspace(*str++))
		{
			return 0;
		}
	}
	return 1;
}

/* return pointer to label if it is a cutlabel label */
labelInfo *getLabelIfCutLabel(char *strLabel)
{
	char *label;
	/*int i;*/
        char str[MAX_LINE_LENGTH];
        strcpy(str,strLabel);		/* copy to avoid from change the original label */
	label=strtok(str,"[");		/* get label name */
	/*while(!strcmp(label,labelArr[i].name))
	{
		i++;
	}*/
	return getLabel(label);
}

/* check if label is cutlabel return 0 if it is */
int isCutLabel(char *labelStr, int printErrors)
{
        char *label;
        char *range;
        char *startRange, *endRange;
        int startRangeInt, endRangeInt;
        char *str;
        str=allocString(labelStr);	/* copy to avoid from change the original string */
        label=strtok(str,"[");			/* take the label */
	if(label!=NULL)
	{
		if(isLegalLabel(label,0))
		{
		       range=strtok(NULL,"]");
			if(range==NULL)
				return 0;
		       startRange=strtok(range,"-");
		       endRange=strtok(NULL,"\0");
		       startRangeInt=atoi(startRange);
		       endRangeInt=atoi(endRange);
		       if(startRangeInt>=0 && endRangeInt<=14 && 
		          startRangeInt<endRangeInt &&
		          endRangeInt-startRangeInt<=13) /* check if is legal range */
		         return 1;
		}
	}
        return 0;      
}

/* Returns if the label is already exist in entry array */
int isExistingEntryLabel(char *labelName)
{
	int i = 0;

	if (labelName)
	{
		for (i = 0; i < numberOfEntry; i++)
		{
			if (strcmp(labelName, entryLines[i].lineStr) == 0)
			{
				return 1;
			}
		}
	}
	return 0;
}

/* Returns if labelStr is a legal label name. */
int isLegalLabel(char *labelStr, int printErrors)
{
	int labelLength = strlen(labelStr), i;

	/* Check for size */
	if (strlen(labelStr) > MAX_LABEL_LENGTH)
	{
		if (printErrors) printf("Label is too long.");
		return 0;
	}

	/* Check if the label isn't an empty string */
	if (*labelStr == '\0')
	{
		if (printErrors) printf("Label name is empty.");
		return 0;
	}

	/* Check if the first char is a letter. */
	if (isspace(*labelStr))
	{
		if (printErrors) printf("Label must start at the start of the line.");
		return 0;
	}

	/* Check if it contain chars only. */
	for (i = 1; i < labelLength; i++)
	{
		if (!isalnum(labelStr[i]))
		{
			if (printErrors) printf("label can contain letters and numbers only.");
			return 0;
		}
	}

	/* Check if the 1st char is a letter. */
	if (!isalpha(*labelStr))
	{
		if (printErrors) printf("first char of label must be a letter.");
		return 0;
	}

	/* Check if it's not a name of a register */
	if (isRegister(labelStr, NULL))
	{
		if (printErrors) printf("labal can not be a register.");
		return 0;
	}

	/* Check if it's not a name of a command */
	if (getCmdId(labelStr) != -1)
	{
		if (printErrors) printf("labal can not be a command.");
		return 0;
	}

	return 1;
}

/* check if it is register */
int isRegister(char *str, int *value)
{
	if (str[0] == '$'  && str[1] >= '0' && str[1] - '0' <= 7 && str[2] == '\0') 
	{
		/* Update value if it's not NULL */
		if (value)
		{
			*value = str[1] - '0'; /* -'0' To get the actual number the char represents */
		}
		return 1;
	}

	return 0;
}


/* Returns if the label already exists. */
int isExistingLabel(char *label)
{
	if (getLabel(label))
	{
		return 1;
	}

	return 0;
}

/* Returns 1 if the cmd is a directive. */
int isDirective(char *cmd)
{
	if (cmd[0] == '.'){
		return 1;
	}
	return 0;
}

/* Returns if the strParam enclosed in quotes, and remove them. */
int isLegalStringParam(char **strParam, int lineNum)
{
	/* check if the string param is enclosed in quotes */
	if ((*strParam)[0] == '"' && (*strParam)[strlen(*strParam) - 1] == '"')
	{
		/* remove the quotes */
		(*strParam)[strlen(*strParam) - 1] = '\0';
		++*strParam;
		return 1;
	}

	if (**strParam == '\0')
	{
		printf("No parameter.");
	}
	else
	{
		printf("string must start and end with quotes");
	}
	return 0;
}

/* Returns if the num is a legal number param, and save it's value in *value. */
int isLegalNum(char* numStr, int numOfBits, int lineNum, int* value)
{
	char* endOfNum;
	/* maxNum is the max number you can represent with (MAX_LABEL_LENGTH - 1) bits
	 (-1 for the negative/positive bit) */
	int maxNum = (1 << numOfBits) - 1;

	if (isWhiteSpaces(numStr))
	{
		printf("Empty parameter.");
		return 0;
	}

	*value = strtol(numStr, &endOfNum, 0);

	/* if endOfNum is at the end of the string: isn't valid num */
	if (*endOfNum)
	{
		printf("isn't a valid number.");
		return 0;
	}

	/* Check for size */
	if (*value > maxNum || *value < -maxNum)
	{
		printf("number must be between %d and %d.", -maxNum, maxNum);
		return 0;
	}

	return 1;
}


