#define _CRT_SECURE_NO_WARNINGS
#include "assmbler.h"
#include <stdlib.h>

directive directivesArr[MAX_LINES_NUM];
labelInfo labelArr[MAX_LABELS_NUM];
int numberOfLabels;
int numberOfDataF;
int numberOfDirectives;


int firstRun(FILE* file, int* IC, int* DC);
int getLine(FILE* file, char* buffer);
void processCmdOperands(lineInfo* line, int* IC, int* DC);
int processLine(lineInfo* line, char* lineStr, int lineNum, int* IC, int* DC);
void processDirective(lineInfo* line, int* IC, int* DC);
void processDataFunc(lineInfo* line, int* IC, int* DC);
void processCommand(lineInfo* line, int* IC, int* DC);
int isDataFunc(lineInfo* line);
labelInfo* addLabelToArr(labelInfo label, lineInfo* line);




char* findLabel(lineInfo* line, int IC)
{
	char* labelEnd = strchr(line->lineStr, ':');
	labelInfo label = { 0 };
	label.address = IC;
	/* Find the label */
	if (!labelEnd)
	{
		return NULL;
	}
	labelEnd[0] = '\0';

	/* Check if the ':' came after the first word */
	if (!isOneWord(line->lineStr))
	{
		labelEnd[0] = ':'; /* Fix the change in line->lineStr */
		return NULL;
	}

	/* if legal, add it to the labelList */
	line->label = addLabelToArr(label, line);
	return labelEnd + 1; /* +1 to make it point at the next char after the \0 */
}

labelInfo* addLabelToArr(labelInfo label, lineInfo* line)
{
	int i = 0;
	/* Check if label is legal */
	if (!isLegalLabel(line->lineStr, 1))
	{
		/* Illegal label name */
		line->isError = 1;
		return NULL;
	}

	/* Check exist */
	if (isExistingLabel(line->lineStr))
	{
		printf("Label already exists. \n");
		line->isError = 1;
		return NULL;
	}

	/* Add the name to the label */
	label.name = line->lineStr;

	/* Add the label to label array and to the lineInfo */
	if (numberOfLabels < MAX_LABELS_NUM)
	{
		for (i = 0; i < numberOfDirectives; i++)
		{
			if (!strcmp(directivesArr[i].name, label.name))
				directivesArr[i].address = label.address;
		}
		labelArr[numberOfLabels] = label;
		return &labelArr[numberOfLabels++];
	}

	/* Too many labels */
	printf("Too many labels \n");
	line->isError = 1;
	return NULL;
}


/*try to get line. if sucsses 1 else return 0*/
int getLine(FILE* file, char* buffer)
{
	char c;
	char* endOfLine;
	int er;

	if (!fgets(buffer, MAX_LINE_LENGTH + 2, file))
	{
		return 0;
	}

	/* Check the line size (no '\n' was present). */
	endOfLine = strchr(buffer, '\n');
	if (endOfLine)
	{
		*endOfLine = '\0';
	}
	else
	{

		er = (feof(file)) ? 1 : 0; /* expected return */

		/* Keep reading chars until you reach the end of the line ('\n') or EOF */
		do
		{
			c = fgetc(file);
		} while (c != '\n' && c != EOF);

		return er;
	}

	return 1;
}

const dataFuncs datafuncArr[] =
{
	{ ".db", 1, db } ,
	{ ".dh", 2, dh } ,
	{ ".dw", 3, dw } ,
	{ ".asciz", 1, asciz } ,
	{NULL}
};


const command commandsArr[] = /* represent by name, opcode and number of parameters */
{
	{ "add", 0, 1, 3, R} ,
	{ "sub", 0, 2, 3, R} ,
	{ "and", 0, 3, 3, R} ,
	{ "or", 0, 4, 3, R} ,
	{ "nor", 0, 5, 3, R} ,
	{ "move", 1, 1, 2, R} ,
	{ "mvhi", 1, 2, 2, R} ,
	{ "mvlo", 1, 3, 2, R} ,
	{ "addi", 10, 0, 3, I} ,
	{ "subi", 11, 0, 3, I} ,
	{ "andi", 12, 0, 3, I} ,
	{ "ori", 13, 0, 3, I} ,
	{ "nori", 14, 0, 3, I} ,
	{ "bne", 15, 0, 3, I} , 
	{ "beq", 16, 0, 3, I} , 
	{ "blt", 17, 0, 3, I} , 
	{ "bgt", 18, 0, 3, I} , 
	{ "lb", 19, 0, 3, I} ,
	{ "sb", 20, 0, 3, I} ,
	{ "lw", 21, 0, 3, I} ,
	{ "sw", 22, 0, 3, I} ,
	{ "lh", 23, 0, 3, I} ,
	{ "sh", 24, 0, 3, I} ,
	{ "jmp", 30, 0, 1, J} ,
	{ "la", 31, 0, 1, J} ,
	{ "call", 32, 0, 1, J} ,
	{ "stop", 63, 0, 1, J} ,
	{ NULL }
};

int firstRun(FILE* file, int* IC, int* DC)
{
	int errorsNum = 0;
	int linecnt;
	char linest1[MAX_LINE_LENGTH + 2];
	lineInfo* everylineinfo;
	numberOfLabels = 0;
	numberOfDataF = 0;
	numberOfDirectives = 0;
	linecnt = 0;

	everylineinfo = malloc(sizeof(lineInfo));
	while (!feof(file))
	{
		if (getLine(file, linest1))
		{
			linecnt++;
			errorsNum = processLine(&(everylineinfo[linecnt-1]), linest1, linecnt, IC, DC);
			everylineinfo = realloc(everylineinfo, (linecnt + 1) * sizeof(lineInfo));	
		}
	}
	SecRun(file, *IC, *DC, everylineinfo, &errorsNum, linecnt);
	return 0;
}


/* process the operands in a command line. */
void processCmdOperands(lineInfo* line, int* IC, int* DC)
{
	int flag = 0;
	int k = 0;
	int i = 0;
	int j = 0;
	int opNum = 0;
	char* startOfNextPart = line->lineStr;
	char data[10] = { 0 };
	/* Reset the op types */
	line->op1.type = INVALID;
	line->op2.type = INVALID;
	line->op3.type = INVALID;
	line->op1.value = 0;
	line->op2.value = 0;
	line->op3.value = 0;

	if (line->cmd->type == R)
	{
		for (opNum = 0; opNum < 3; opNum++)
		{
			if (line->cmd->opcode == 1 && opNum == 1)
			{
				line->op2.address = 0;
				opNum++;
			}
			while (line->lineStr[i] != ',')
			{
				if (line->lineStr[i] == '\n' || line->lineStr[i] == '\0')
				{
					break;
				}
				if (line->lineStr[i] == '$')
				{
					flag = 1;
				}
				if (line->lineStr[i] >= '0' && line->lineStr[i] <= '9' && flag == 1)
				{
					data[j] = line->lineStr[i];
					j++;
				}
				i++;
			}
			if (flag == 0)
			{
				if (i != 3 && line->cmd->opcode != 1)
				{
					line->isError = 1;
					printf("This command should get 3 registers as operands \n");
				}
			}
			flag = 0;
			j = 0;
			if (opNum == 0)
			{
				line->op1.address = atoi(data);
			}

			if (opNum == 1)
			{
				line->op2.address = atoi(data);
			}

			if (opNum == 2)
			{
					line->op3.address = atoi(data);
			}
			for (k = 0; k < 10; k++)
				data[k] = 0;
			if (line->lineStr[i] == '\n' || line->lineStr[i] == '\0' || line->lineStr[i] == EOF)
			{
				break;
			}
			if (line->lineStr[i] == ',')
			i++;
		}
	}
	if (line->cmd->type == I)
	{
		if (line->cmd->opcode >= 15 && line->cmd->opcode <= 18)
		{
			for (opNum = 0; opNum < 3; opNum++)
			{
				while (line->lineStr[i] != ',')
				{
					if (opNum == 2)
						break;
					if (line->lineStr[i] == '\n' || line->lineStr[i] == '\0')
					{
						break;
					}
					if (line->lineStr[i] == '$')
					{
						flag = 1;
					}

					if (((((int)(line->lineStr[i]) >= '0' && (int)(line->lineStr[i]) <= '9')) || (int)(line->lineStr[i]) == '-') && flag == 1)
					{
						data[j] = line->lineStr[i];
						j++;
					}
					i++;
				}
				if (opNum == 2)
				{
					flag = 1;
					startOfNextPart += i;
				}
				if (flag == 0)
				{
					line->isError = 1;
					printf("Error reading operands func %s \n", line->commandStr);		
				}
				flag = 0;
				j = 0;
				if (opNum == 0)
				{
					line->op1.address = atoi(data);
				}

				if (opNum == 1)
				{
					line->op3.address = atoi(data);
				}

				if (opNum == 2)
				{
					line->op2.type = LABEL;
					line->op2.str = (char*)malloc(sizeof(char) * strlen(startOfNextPart));
					strcpy(line->op2.str, startOfNextPart);
					remove_spaces(line->op2.str);
					for(j = 0 ; j < numberOfLabels ; j++)
					{
						
						if (!(strcmp(line->lineStr, labelArr[j].name)))
						{
							line->op2.address = (labelArr[j].address - line->address);
							line->op2.str = labelArr[j].name;
							return;
						}
					}
				}
				for (k = 0; k < 10; k++)
					data[k] = 0;
				if (line->lineStr[i] == '\n' || line->lineStr[i] == '\0')
				{
					break;
				}
				if(line->lineStr[i] == ',')
				i++;
			}
		}
		else
		{
			for (opNum = 0; opNum < 3; opNum++)
			{
				while (line->lineStr[i] != ',')
				{
					if (line->lineStr[i] == '\n' || line->lineStr[i] == '\0')
					{
						break;
					}
					if (opNum != 1)
					{
						if (line->lineStr[i] == '$')
						{
							flag = 1;
						}
					}
					if (opNum == 1)
						flag = 1;
					if ((((int)(line->lineStr[i]) >= '0' && (int)(line->lineStr[i]) <= '9') || (int)(line->lineStr[i]) == '-') && flag == 1)
					{
						data[j] = line->lineStr[i];
						j++;
					}
					i++;
				}
				if (opNum == 2)
				{
					flag = 1;
					startOfNextPart += i;
				}
				if (flag == 0)
				{
					line->isError = 1;
					printf("Error reading operands func %s \n", line->commandStr);
				}
				flag = 0;
				j = 0;
				if (opNum == 0)
				{
					line->op1.address = atoi(data);
				}

				if (opNum == 1)
				{
					line->op2.address = atoi(data);
				}

				if (opNum == 2)
				{
					line->op3.address = atoi(data);
				}
				for (k = 0; k < 10; k++)
					data[k] = 0;
				if (line->lineStr[i] == '\n' || line->lineStr[i] == '\0')
				{
					break;
				}
				if (line->lineStr[i] == ',')
					i++;
			}
		}
	}


	if (line->cmd->type == J)
	{
		i = 0;
		j = 0;
		if (line->cmd->opcode == 30)/*jmp*/
		{
			while (line->lineStr[i] != '\n' && line->lineStr[i] != '\0')
			{
				if (flag)
				{
					data[j] = line->lineStr[i];
					j++;
				}
				if (line->lineStr[i] == '$')
				{
					flag = 1;
					line->op1.type = REGISTER;
				}
				i++;
			}
			line->op1.address = atoi(data);
			if (!flag)
			{
				line->op1.address = 0;
				line->op1.type = LABEL;
				line->op1.str = (char*)malloc(sizeof(char) * strlen(startOfNextPart));
				strcpy(line->op1.str, startOfNextPart);
				remove_spaces(line->op1.str);
				for (j = 0; j < numberOfLabels; j++)
				{
					if (!(strcmp(line->lineStr, labelArr[j].name)))
					{
						line->op1.address = (labelArr[j].address - *(IC));
						line->op1.str = labelArr[j].name;
						return;
					}
				}

			}

		}
		if (line->cmd->opcode == 31 || line->cmd->opcode == 32)/*la , call*/
		{
			line->op1.address = 0;
			line->op1.type = LABEL;
			line->op1.str = (char*)malloc(sizeof(char) * strlen(startOfNextPart));
			strcpy(line->op1.str, startOfNextPart);
			remove_spaces(line->op1.str);
			for (j = 0; j < numberOfLabels; j++)
			{
				if (!(strcmp(line->lineStr, labelArr[j].name)))
				{
					line->op1.address = (labelArr[j].address - *(IC));
					line->op1.str = labelArr[j].name;
					return;
				}
			}
		}


		if (line->cmd->opcode == 63)/*stop*/
		{
			line->op1.type = LABEL;
			line->op1.address = 0;
			remove_spaces(line->lineStr);
			if (line->lineStr[0] != 0 && line->lineStr[0] != '\n')
			{
				line->isError = 1;
				printf("Error: Operands after stop \n");
			}
		}
	}
}

/* process the command */
void processCommand(lineInfo* line, int* IC, int* DC)
{
	int cmdId = getCmdId(line->commandStr);

	if (cmdId == -1)
	{
		line->cmd = NULL;
		if (*line->commandStr == '\0')
		{
			/* The command is empty, but the line isn't empty. must be label. */
			printf("Can't write a label to an empty line. \n");
		}
		else
		{
			/* Illegal command. */
			printf("No such command. \n");
		}
		line->isError = 1;
		return;
	}
	line->cmd = &commandsArr[cmdId];
	processCmdOperands(line, IC, DC);
}


/* process the directive */
void processDirective(lineInfo* line, int* IC, int* DC)
{
	int flag = 0;
	int i = 0;
	if (!strcmp(line->commandStr, ".extern"))
	{
		i = 0;
		while (i < numberOfDirectives)
		{
			if (!strcmp(line->lineStr, directivesArr[i].name))
				flag = 1;
			i++;
		}
		if (flag == 0)
		{
			directivesArr[numberOfDirectives].name = (char*)malloc(sizeof(char) * strlen(line->lineStr));
			strcpy(directivesArr[numberOfDirectives].name, line->lineStr);
			remove_spaces(directivesArr[numberOfDirectives].name);
			directivesArr[numberOfDirectives].EXaddress = (int*)malloc(sizeof(int));
			directivesArr[numberOfDirectives].ptrExAdd = &(directivesArr[numberOfDirectives].EXaddress);
			directivesArr[numberOfDirectives].type = EX;
			directivesArr[numberOfDirectives].Exsize = 0;
			numberOfDirectives++;
		}
		
	}
	else
	{
		if (!(strcmp(line->commandStr, ".entry")))
		{
			line->isDirective = EN;

			i = 0;
			while (i < numberOfDirectives)
			{
				if (!strcmp(line->lineStr, directivesArr[i].name))
					flag = 1;
				i++;
			}
			if (flag == 0)
			{
				directivesArr[numberOfDirectives].name = (char*)malloc(sizeof(char) * strlen(line->lineStr));
				strcpy(directivesArr[numberOfDirectives].name, line->lineStr);
				remove_spaces(directivesArr[numberOfDirectives].name);
				directivesArr[numberOfDirectives].address = *(IC);
				directivesArr[numberOfDirectives].type = EN;
				numberOfDirectives++;
			}			/*we need to add here check if exist or add it to directivearr*/
		}
		else
		{
			printf("No such directive \n");
			line->isError = 1;
		}
	}
}

void processDataFunc(lineInfo* line, int* IC, int* DC)
{
	int k;
	char ch[10] = { 0 };
	int isErr = 0;
	int geresh = 0;
	int i = 0;
	int j = 0;
	int* dataInt;
	dataInt = (int*)malloc(sizeof(int) * 8); 
	if (line->isDataF->type != asciz)
	{
		while (line->lineStr[i] != '\n' && line->lineStr[i] != '\0')
		{
			k = 0;
			if (line->lineStr[i] == ',')
			{
				i++;
				continue;
			}
			while (line->lineStr[i] != ',' && line->lineStr[i] != '\n' && line->lineStr[i] != '\0')
			{

				ch[k] = line->lineStr[i];
				i++;
				k++;
			}
			
			dataInt = realloc(dataInt, sizeof(int) * (j+1));
			dataInt[j] = atoi(ch);
			j++;
			
			for (k = 0; k < 10; k++)
				ch[k] = 0;

			if (line->isDataF->type == db)
			{
				*(IC) = *(IC)+1;
				*(DC) = *(DC)+1;
			}
			if (line->isDataF->type == dh)
			{
				*(IC) = *(IC)+2;
				*(DC) = *(DC)+2;
			}
			if (line->isDataF->type == dw)
			{
				*(IC) = *(IC)+4;
				*(DC) = *(DC)+4;
			}

		}
		line->isDataF->size = j;
		line->dataF = dataInt;

	}

	if (line->isDataF->type == asciz)
	{
		while (line->lineStr[i] != '\n' && line->lineStr[i] != '\0')
		{
			if (geresh == 1)
			{
				dataInt = realloc(dataInt, sizeof(int) * (j + 1));
				dataInt[j] = (int)line->lineStr[i];
				*(IC) += 1;
				*(DC) += 1;
				j++;

			}
			if (line->lineStr[i] == '"')
				geresh++;
			i++;
			if (geresh == 2)
			{
				dataInt[j - 1] = 0;
				line->dataF = dataInt;
				line->isDataF->size = j;
			}

		}
		if (isErr)
		{
			line->isError = 1;
		}
	}
	if(!isErr)
	numberOfDataF++;
}

int isDataFunc(lineInfo* line)
{
	int i = 0;
	while(i < 4)
	{
		if (!(strcmp(line->commandStr, datafuncArr[i].name)))
		{
			line->isDataF = (dataFuncs*)malloc(sizeof(dataFuncs));
			if (datafuncArr[i].type == 1)
				line->isDataF->type = db;
			if (datafuncArr[i].type == 2)
				line->isDataF->type = dh;
			if (datafuncArr[i].type == 3)
				line->isDataF->type = dw;
			if (datafuncArr[i].type == 4)
				line->isDataF->type = asciz;
			return 1;
		}
		
		i++;
	}
	return 0;
}

/* process a line, and print errors. */
int processLine(lineInfo* line, char* lineStr, int lineNum, int* IC, int* DC)
{
	char* startOfText;
	char* startOfNextPart = lineStr;

	line->lineNum = lineNum;
	line->address = *IC;
	line->originalString = allocString(lineStr);
	line->lineStr = line->originalString;
	line->isError = 0;
	line->label = NULL;
	line->commandStr = NULL;
	line->cmd = NULL;
	line->isDataF = NULL;
	line->isDirective = NIL;

	
	if (!line->originalString)
	{
		printf("there is not enough memory \n");
		return 1;
	}


	startOfText = line->lineStr; /* to avoid damage line->lineStr */

	if (*(line->lineStr) == ';')
	{
		/* Comment */
		return 0;
	}

	spaceRemover(&startOfText);
	if (*startOfText == '\0')
	{
		/* Empty line */
		return 0;
	}
	if (*startOfText == ';')
	{
		/* error if ';' isn't at the start of the line 3 */
		printf("Comments must start with ';' \n");
		line->isError = 1;
		return 1;
	}



	/* Find label and add it to the label list */
	startOfNextPart = findLabel(line, *IC); /*4*/

	if (line->isError)
	{
		return 1;
	}

	/* Update the line if startOfNextPart isn't NULL */
	if (startOfNextPart)
	{
		line->lineStr = startOfNextPart;
	}

	/* Find the command token */
	line->commandStr = getFirstTok(line->lineStr, &startOfNextPart);
	line->lineStr = startOfNextPart;

	/* process the command or directive */
	if (isDirective(line->commandStr))/* 6 - 9 */
	{
		if (isDataFunc(line))
		{
			line->isDirective = DataF;
			processDataFunc(line, IC, DC);
		}
		else
			processDirective(line, IC, DC);
	}
	else
	{
		line->isDirective = CMD;
		processCommand(line, IC, DC);
		*(IC) = *(IC)+4;
	}
	if (line->isError)
	{
		return 1;
	}
	return 1;
}
