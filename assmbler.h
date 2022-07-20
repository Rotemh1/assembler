#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	MAX_LABELS_NUM		1000
#define BYTE_SIZE	        8
#define MAX_DATA_NUM		1000
#define FIRST_ADDRESS		100 
#define MAX_LINE_LENGTH		100
#define MAX_LABEL_LENGTH	30
#define MEMORY_WORD_LENGTH	15
#define MAX_REGISTER_DIGIT	7
#define MAX_LINES_NUM		1000
#define MAX_CUTLABEL_LINES      500
#define IS_ADDRESS		3
#define IS_WORD			5


typedef enum { NUMBER = 0, LABEL = 1, EXLABEL = 2 ,REGISTER = 3, INVALID = -1 } opType;
typedef enum { ABSOLUTE = 0, EXTENAL = 1, RELOCATABLE = 2 } eraType;
typedef enum { J = 1, I = 2, R = 3} cmdType;
typedef enum { db = 1, dh = 2, dw = 3, asciz = 4} datafType;
typedef enum { EX = 1, EN = 2, CMD = 3, DataF = 4, NIL = 0} directiveType;

typedef struct
{
	char* name;
	int size;
	datafType type;
} dataFuncs;

typedef struct
{
	char* name;
	directiveType type;
	int address;			/* Used for entries, gets entry LABEL address.*/
	int Exsize;				/*size of EXaddress arry*/
	int** ptrExAdd;
	int* EXaddress;			/* Used for externs, an arry of addresses of calls for extern func.*/
}directive;

typedef struct 
{
	char* name;
	int opcode;
	int func : 4;
	int numofparms;
	cmdType type;
} command;

typedef struct
{
	int address;
	char* name;
	int type;				/* gets 1 if cmd, 2 if dataFunc, 3 if extern */
	int isEntry;
} labelInfo;

typedef struct
{
	int value;				/* Value */
	char* str;				/* String */
	opType type;			/* Type */
	int address;			/* The address of the operand in the memory */ 
} operandInfo;

typedef struct
{
	int lineNum;				/* The number of the line in the file */
	int address;				/* The address of the first word in the line */
	char* originalString;		/* The original pointer, allocated by malloc */
	char* lineStr;				/* The text it contains */
	int isError;				/* 0 error or 1 not */
	labelInfo *label;			/* A poniter to labelArr */
	char* commandStr;			/* The string of the command or directive */
	int* dataF;					/* Arry contains data if line is data func*/
	dataFuncs* isDataF;			/* If the line is data func, define which type*/
	directiveType isDirective;	/* Defines directive type or cmd line*/

	/* Command line */
	const command* cmd;			/* A pointer to cmdArr */
	operandInfo op1;			/* The first operand */
	operandInfo op2;			/* The second operand */
	operandInfo op3;			/* The third operand*/
} lineInfo;


/*methods*/

char* allocString(const char* str);

/*labelInfo* getLabelIfCutLabel(char* strLabel);*/
int firstRun(FILE* file, int* IC, int* DC);
void SecRun(FILE* file, int IC, int DC, lineInfo* everylineinfo, int* ErrorsNum, int linecnt);
char* getFirstOperand(char* line, char** endOfOp, int* foundComma);
int isWhiteSpaces(char* str);
void deleteAllSpaces(char** ptStr);
void spaceRemover(char** str);
int getRange(char* strLabel, int isStart);
int isCutLabel(char* labelStr, int printErrors);
int isExistingEntryLabel(char* labelName);
int isLegalLabel(char* labelStr, int printErrors);
int getCmdId(char* cmdName);
int isRegister(char* str, int* value);
int isExistingLabel(char* label);
labelInfo* getLabel(char* labelName);
char* getFirstTok(char* str, char** endOfTok);
int isDirective(char* cmd);
int isLegalStringParam(char** strParam, int lineNum);
int isLegalNum(char* numStr, int numOfBits, int lineNum, int* value);
int isOneWord(char* str);
void Decimal_to_Binary(int number, int bits, int* bin);
void remove_spaces(char* s);
void Two_complement(int bits, int* bin);
void binAdd1(int bits, int* bin);

/*externals for file printing*/

extern directive directivesArr[MAX_LINES_NUM];
extern labelInfo labelArr[];
extern int numberOfLabels;
extern int numberOfDataF;
extern int numberOfDirectives;
extern int* memArr;