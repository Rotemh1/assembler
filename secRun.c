#define _CRT_SECURE_NO_WARNINGS
#include "assmbler.h"
#include <stdio.h>



void fixLabelsArr(lineInfo* everylineinfo);
int entriesFix(lineInfo* line);
int isJcmd(lineInfo* line);
int processJcmd(lineInfo* line, int IC);
int processBcmd(lineInfo* line, int IC);
void CreateMemArr(lineInfo* everylineinfo, int IC, int linecnt);


int* memArr;


int entriesFix(lineInfo* line)
{
	int errNum = 0;
	int i = 0;
	int j = 0;
	int flag = 0;
	while (i<numberOfDirectives)
	{
		if (directivesArr[i].type == EN)
		{
			while (j < numberOfLabels)
			{
				if (!strcmp(labelArr[j].name, directivesArr[i].name))
				{
					labelArr[j].isEntry = 1;
					directivesArr[i].address = labelArr[j].address;
					flag = 1;
				}
				j++;
			}
			if (flag == 0)
			{
				errNum++;
				printf("%s is not a legel entry! \n", directivesArr[i].name);
			}
		}
		j = 0;
		i++;
	}
	return errNum;
}
void fixLabelsArr(lineInfo* everylineinfo)
{
	int i = 0;
	int j = 0;
	while (labelArr[i].address)
	{
		while (1)
		{
			if (labelArr[i].address == everylineinfo[j].address)
			{
				if (everylineinfo[j].isDirective == EX)
					labelArr[i].type = 3;
				if (everylineinfo[j].isDirective == DataF)
					labelArr[i].type = 2;
				if (everylineinfo[j].isDirective == CMD)
					labelArr[i].type = 1;
			}
				j++;
		}

	}
}

void SecRun(FILE* file, int IC, int DC, lineInfo* everylineinfo,int* ErrorsNum, int linecnt)
{
	int i = 0;
	lineInfo* lineptr;
	lineptr = everylineinfo;
	*(ErrorsNum) += entriesFix(everylineinfo);
	for (; i < linecnt; i++)
	{
		if (isJcmd(&everylineinfo[i]))
		{
			*(ErrorsNum) += processJcmd(&everylineinfo[i], IC);
		}
		if (everylineinfo[i].cmd)
		{
			if (everylineinfo[i].cmd->opcode >= 15 && everylineinfo[i].cmd->opcode <= 18)
			{
				*(ErrorsNum) += processBcmd(&(everylineinfo[i]), IC);
			}
		}
	}
	CreateMemArr(lineptr, IC, linecnt);

}

int isJcmd(lineInfo* line)
{
	if (line->cmd)
	{
		if (line->cmd->type == J)
		{
			if (line->cmd->opcode != 62)
				return 1;
		}
	}
	return 0;
}

int processJcmd(lineInfo* line, int IC)
{
	int num = 0;
	int i = 0;
	int flag = 1;
	if (line->cmd->opcode == 63)
		return 0;
	if (line->op1.address == 0)
	{
		while (i<numberOfLabels)
		{
			if (!(strcmp(labelArr[i].name, line->lineStr)))
			{
				line->op1.value = labelArr[i].address;
				line->op1.address = labelArr[i].address;
				line->op1.type = LABEL;
				break;
			}
			i++;
		}
		if (line->op1.value == 0) 
		{
			flag = 0; 
		}
	}
	i = 0;
	while (i < numberOfDirectives)
	{
		if ((!(strcmp(line->lineStr, directivesArr[i].name))) && directivesArr[i].type == EX)
		{
			line->op1.type = EXLABEL;
			line->op1.str = directivesArr[i].name;
			line->op1.value = 0;
			line->op1.address = 0;
			directivesArr[i].Exsize++;
			num = directivesArr[i].Exsize;
			directivesArr[i].EXaddress = realloc(directivesArr[i].EXaddress, sizeof(int) * num);
			directivesArr[i].EXaddress[num - 1] = line->address;
			return 0;
		}
		if ((!(strcmp(line->lineStr, directivesArr[i].name))) && directivesArr[i].type == EN)
		{
			line->op1.type = LABEL;
			line->op1.str = directivesArr[i].name;
			line->op1.value = directivesArr[i].address;
			return 0;
		}
		i++;
	}
	if (flag == 0)
	{
		printf("%s is not a legal label!!!! \n", line->lineStr);
		return 1;
	}
	return 0;
}

int processBcmd(lineInfo* line, int IC)
{
	int i = 0;
	int flag = 1;
	if (line->op2.value == 0)
	{
		while (labelArr[i].name)
		{
			if (!(strcmp(labelArr[i].name, line->op2.str)))
			{
				line->op2.address = (labelArr[i].address - line->address);
				line->op2.value = 1;
				break;
			}
			i++;
		}
		if (line->op2.value == 0)
		{
			flag = 0;
		}
	}

	i = 0;
	while (i< numberOfDirectives)
	{
		if ((!(strcmp(line->op2.str, directivesArr[i].name))) && directivesArr[i].type == EX)
		{
			printf("Error!!! : %s is an extern label \n", directivesArr[i].name);
			return 1;
		}
		if (!(strcmp(line->op2.str, directivesArr[i].name) && directivesArr[i].type == EN))
		{
			line->op2.type = LABEL;
			line->op2.str = directivesArr[i].name;
			line->op2.value = (directivesArr[i].address - line->address);
			return 0;
		}
		i++;
	}
	if (flag == 0)
	{
		printf("%s is not a legal label!!!! \n", line->op2.str);
		return 1;
	}
	return 0;
}

void CreateMemArr(lineInfo* everylineinfo, int IC, int linecnt)
{
	int i, j;
	int* pointmem;
	memArr = malloc(sizeof(int) * (((IC - 100) * 8) + 1));
	pointmem = memArr;
	for (i = 0; i < linecnt; i++)
	{
		if (everylineinfo[i].isDirective == CMD)
		{

			if (everylineinfo[i].cmd->type == R)
			{
				Decimal_to_Binary(0, 6, memArr);
				memArr += 6;
				Decimal_to_Binary(everylineinfo[i].cmd->func, 5, memArr);
				memArr += 5;
				Decimal_to_Binary(everylineinfo[i].op3.address, 5, memArr);
				memArr += 5;
				Decimal_to_Binary(everylineinfo[i].op2.address, 5, memArr);
				memArr += 5;
				Decimal_to_Binary(everylineinfo[i].op1.address, 5, memArr);
				memArr += 5;

			}
			if (everylineinfo[i].cmd->type == I)
			{
					Decimal_to_Binary(everylineinfo[i].op2.address, 16, memArr);
					memArr += 16;
					Decimal_to_Binary(everylineinfo[i].op3.address, 5, memArr);
					memArr += 5;
					Decimal_to_Binary(everylineinfo[i].op1.address, 5, memArr);
					memArr += 5;


			}
			if (everylineinfo[i].cmd->type == J)
			{
				if (everylineinfo[i].op1.type == LABEL || everylineinfo[i].op1.type == EXLABEL)
				{
					Decimal_to_Binary(everylineinfo[i].op1.address, 25, memArr);
					memArr += 25;
					Decimal_to_Binary(0, 1, memArr);
					memArr++;
				}
				if (everylineinfo[i].op1.type == REGISTER)
				{
					Decimal_to_Binary(everylineinfo[i].op1.address, 25, memArr);
					memArr += 25;
					Decimal_to_Binary(1, 1, memArr);
					memArr++;
				}
			}
			Decimal_to_Binary(everylineinfo[i].cmd->opcode, 6, memArr);
			memArr += 6;
		}
		if (everylineinfo[i].isDirective == DataF)
		{
			if (everylineinfo[i].isDataF->type == 1)
			{
				for (j=0;j < everylineinfo[i].isDataF->size;j++)
				{
					Decimal_to_Binary(everylineinfo[i].dataF[j], 8, memArr);
					memArr += 8;
				}
			}
			if (everylineinfo[i].isDataF->type == 2)
			{
				for (j = 0; j < everylineinfo[i].isDataF->size; j++)
				{
					Decimal_to_Binary(everylineinfo[i].dataF[j], 16, memArr);
					memArr += 16;
				}
			}
			if (everylineinfo[i].isDataF->type == 3)
			{
				for (j = 0; j < everylineinfo[i].isDataF->size; j++)
				{
					Decimal_to_Binary(everylineinfo[i].dataF[j], 32, memArr);
					memArr += 32;
				}
			}
			if (everylineinfo[i].isDataF->type == 4)
			{
				for (j = 0; j < everylineinfo[i].isDataF->size; j++)
				{
					Decimal_to_Binary(everylineinfo[i].dataF[j], 8, memArr);
					memArr += 8;
				}
			}
		}
	}
	memArr = pointmem;
}