#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "simple_compiler.h"
#include "postfix.h"

#define REM     "rem"   // comentary
#define LET     "let"   // declare variables
#define INPUT   "input" // get a value
#define PRINT   "print" // print a value
#define GOTO    "goto"  // move to a line in the code
#define IF      "if"    // evaluate condition
#define END     "end"   // end program

#define POS_COUNT           1000
#define LINE_CODE_SIZE      256
#define FILE_NAME_SIZE      100

#define POSIBLE_VAR_COUNT   'z' - 'a' + 1
#define ALPHABET            "abcdefghijklmnopqrstuvwxyz"
#define FULL_ALPHABET       "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

#define VAR_VALUE(varName)   *(vars[varName-'a'].ptrMemPos)
#define   VAR_PTR(varName)     vars[varName-'a'].ptrMemPos
#define  VAR_CHAR(varName)     vars[varName-'a'].letter
#define   VAR_USE(varName)     vars[varName-'a'].using

#define SUCCESSFUL  1
#define FAILED      2           // cambiar mas adelante a -1 si es necesario


typedef struct
{
    char letter;
    int * ptrMemPos;
    int using;
} Variable;

struct codeLine
{
    int lineId;
    char * instruction;
    char * argument;
};
typedef struct codeLine CodeLine;
typedef CodeLine * CodeLinePtr;

struct instructionsList 
{
    struct instructionsList * last;
    CodeLinePtr line;
    struct instructionsList * next;
};
typedef struct instructionsList InstructionsList;
typedef InstructionsList * InstructionsListPtr;

int simpletronMemory[POS_COUNT] = {0};
int *cursorMemory = simpletronMemory;
InstructionsListPtr instructions = NULL;
InstructionsListPtr fisrtLine = NULL;
InstructionsListPtr lastLine = NULL;
int readSuccess = FAILED;
FILE *ptrInputFile;

Variable vars[POSIBLE_VAR_COUNT];


void initializeVarMatrix(void);
void getInstructions(void);
void execInstructions(void);
int isInstruction(char * inst);
int insert(int lineId, char * instruction, char * arg);
int isListEmpty(void);
int isValidVar(char varName);
int isStrNumber(char * num);
void trimWhitespace(char * str);
InstructionsListPtr findLine(int lineId);
int isTrue(char * condition);
int let(char * arg);
int input(char * varName);
int print(const char * varName);
int goTo(InstructionsListPtr *cursor, char * arg, int * jumped);
int ifGoto(InstructionsListPtr *cursor, char * arg, int * jumped);



void initializeVarMatrix(void)
{
    for (int i = 'a' - 'a'; i < POSIBLE_VAR_COUNT; i++)
    {
        vars[i].letter = i+'a';
        vars[i].ptrMemPos = NULL;
        vars[i].using = 0;
    }
}

void getInstructions(void)
{
    char fileInputName[FILE_NAME_SIZE] = {'\0'};
    char lineCode[LINE_CODE_SIZE];
    
    printf("*** Welcome back to Simpletron! ***\n");
    printf("Enter source file name:\n  >> ");
    scanf("%s", fileInputName);
    if ((ptrInputFile = fopen(fileInputName, "r")) == NULL)
    {
        printf("\nError: could not open file \"%s\"\n", fileInputName);
        return;
    }

    while (fgets(lineCode, LINE_CODE_SIZE, ptrInputFile) != NULL)
    {
        int lineId;
        char * lineIdStr;
        char * instruction;
        char * arg;

        trimWhitespace(lineCode);
        if (lineCode[0] == '\0')
            continue;

        lineIdStr = strtok(lineCode, " \t");
        if (lineIdStr == NULL)
            continue;
        lineId = (int)strtod(lineIdStr, NULL);
        
        instruction = strtok(NULL, " \t");
        if (instruction == NULL)
        {
            printf("ERROR: line %d: missing instruction\n", lineId);
            readSuccess = FAILED;
            break;
        }
        trimWhitespace(instruction);

        if (!isInstruction(instruction))
        {
            printf("ERROR: line %d: \'%s\' is not a valid instruction\n", lineId, instruction);
            readSuccess = FAILED;
            break;
        }
        
        arg = strtok(NULL, "");
        if (arg != NULL)
            trimWhitespace(arg);

        if (strcmp(instruction, REM) && insert(lineId, instruction, arg) == SUCCESSFUL)
            readSuccess = SUCCESSFUL;
    }

    fclose(ptrInputFile);
    if (readSuccess) printf("\n*** Instructions read successfully ***\n");
    return;
}

void execInstructions(void)
{
    if (!readSuccess)
    {
        printf("\n*** Execution cancelled ***\n");
        return;
    }

    #define INSTRUCTION cursorInstruction->line->instruction
    #define ARGUMENT    cursorInstruction->line->argument

    initializeVarMatrix();

    InstructionsListPtr cursorInstruction = instructions;
    // ptrOutputFile = fopen(OUTPUT_FILE_NAME, "w+");
    printf("\n*** Executing instructions ***\n\n");

    int lineExec = SUCCESSFUL;
    int jumped = 0;
    while (lineExec == SUCCESSFUL && cursorInstruction != NULL)
    {
        if (!strcmp(INSTRUCTION, END)) break;
        else if (!strcmp(INSTRUCTION, LET))
            lineExec = let(ARGUMENT);
        else if (!strcmp(INSTRUCTION, INPUT))
            lineExec = input(ARGUMENT);
        else if (!strcmp(INSTRUCTION, PRINT))
            lineExec = print(ARGUMENT);
        else if (!strcmp(INSTRUCTION, GOTO))
            lineExec = goTo(&cursorInstruction, ARGUMENT, &jumped);
        else if (!strcmp(INSTRUCTION, IF))
            lineExec = ifGoto(&cursorInstruction, ARGUMENT, &jumped);

        if (!jumped)
            cursorInstruction = cursorInstruction->next;
        jumped = 0;
    }

    if (lineExec == FAILED)
        printf("\n*** Execution finished with error ***\n");
    else
        printf("\n*** Execution finished succefully  ***\n");

    // fclose(ptrOutputFile);
    // printf("*** Results saved in \"%s\" ***\n", OUTPUT_FILE_NAME);
    // printf("*** Goodbye! ***\n");
    return;
}

int isInstruction(char * inst)
{
    return (!strcmp(inst, REM) || !strcmp(inst, LET) || !strcmp(inst, INPUT) || !strcmp(inst, PRINT) ||
            !strcmp(inst, GOTO) || !strcmp(inst, IF) || !strcmp(inst, END));
}

// -------------------------------------------- //
// ------------- INSTRUCTION LIST ------------- //
// -------------------------------------------- //
int insert(int lineId, char * instruction, char * arg)
{
    InstructionsListPtr new = malloc(sizeof(InstructionsList));
    CodeLinePtr newLine = malloc(sizeof(CodeLine));

    if (new == NULL || newLine == NULL)
    {
        printf("ET ERROR: out of memory while inserting instruction\n");
        free(new);
        free(newLine);
        return FAILED;
    }

    newLine->lineId = lineId;
    newLine->instruction = strdup(instruction);
    newLine->argument = (arg != NULL) ? strdup(arg) : NULL;

    if (newLine->instruction == NULL || (arg != NULL && newLine->argument == NULL))
    {
        printf("ET ERROR: out of memory while copying instruction text\n");
        free(newLine->instruction);
        free(newLine->argument);
        free(newLine);
        free(new);
        return FAILED;
    }

    new->last = NULL;
    new->line = newLine;
    new->next = NULL;

    if (isListEmpty())
    {
        instructions = new;
        fisrtLine = instructions;
        lastLine  = instructions;
    }
    else
    {
        if (lastLine->line->lineId > new->line->lineId)
        {
            printf("ERROR: Line %d can't be after line %d, but the limes should be ordered from less to higher", 
                    new->line->lineId, lastLine->line->lineId);
            free(newLine->instruction);
            free(newLine->argument);
            free(newLine);
            free(new);
            return FAILED;
        }

        new->last = lastLine;
        lastLine->next = new;
        lastLine = new;
    }
    return SUCCESSFUL;
}

int isListEmpty(void)
{
    return (instructions == NULL);
}

// -------------------------------------------- //
// ---------------- VALIDATORS ---------------- //
// -------------------------------------------- //
int isValidVar(char varName)
{
    if (!islower(varName))
    {
        printf("ET ERROR: variables identifier must be lowercase\n");
        return FAILED;
    }
    if (!VAR_USE(varName))
    {
        printf("ET ERROR: variable '%c' wasn't declared anywhere\n", varName);
        return FAILED;
    }
    return SUCCESSFUL;
}

int isStrNumber(char * num)
{
    int i = 0;
    int end;

    if (num == NULL)
    {
        printf("ET ERROR: '(null)' is not a valid number\n");
        return FAILED;
    }

    while (num[i] != '\0' && isspace((unsigned char)num[i]))
        i++;

    if (num[i] == '\0')
    {
        printf("ET ERROR: '%s' is not a valid number\n", num);
        return FAILED;
    }

    if (num[i] == '+' || num[i] == '-')
        i++;

    if (num[i] == '\0' || !isdigit((unsigned char)num[i]))
    {
        printf("ET ERROR: '%s' is not a valid number\n", num);
        return FAILED;
    }

    for (; num[i] != '\0' && isdigit((unsigned char)num[i]); i++)
        ;

    end = i;
    while (num[end] != '\0' && isspace((unsigned char)num[end]))
        end++;

    if (num[end] != '\0')
    {
        printf("ET ERROR: '%s' is not a valid number\n", num);
        return FAILED;
    }
    return SUCCESSFUL;
}

void trimWhitespace(char * str)
{
    char * start;
    char * end;
    size_t len;

    if (str == NULL || str[0] == '\0')
        return;

    start = str;
    while (*start != '\0' && isspace((unsigned char)*start))
        start++;

    if (*start == '\0')
    {
        str[0] = '\0';
        return;
    }

    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end))
        end--;
    end[1] = '\0';

    if (start != str)
    {
        len = (size_t)(end - start + 1);
        memmove(str, start, len + 1);
    }
}

// -------------------------------------------- //
// --------------- USEFUL TOOLS --------------- //
// -------------------------------------------- //
InstructionsListPtr findLine(int lineId)
{
    InstructionsListPtr cursor = instructions;

    while (cursor && cursor->line->lineId != lineId)
        cursor = cursor->next;

    return cursor;
}

int isTrue(char * condition)
{
    #define LESS_THAN           "<"
    #define LESS_EQUAL_THAN     "<="
    #define HIGHER_EQUAL_THAN   ">="
    #define HIGHER_THAN         ">"
    #define EQUAL_TO            "=="
    #define DIFF_FROM           "!="

    char *param1    = strtok(condition, " ");
    char *operator  = strtok(NULL, " ");
    char *param2    = strtok(NULL, "\n");

    if (param1 == NULL || operator == NULL || param2 == NULL)
    {
        printf("ET ERROR: condition must have two operands and an operator\n");
        return FAILED;
    }

    int value1, value2;

    if (strpbrk(param1, FULL_ALPHABET))
    {
        if (isValidVar(*param1) == FAILED)
            return FAILED;
        value1 = VAR_VALUE(*param1);
    }
    else if (isStrNumber(param1) == SUCCESSFUL)
    {
        value1 = strtod(param1, NULL);
    }
    else
    {
        printf("ET ERROR: %s not a variable or number\n", param1);
        return FAILED;
    }

    if (param2 && strpbrk(param2, FULL_ALPHABET))
    {
        if (isValidVar(*param2) == FAILED)
            return FAILED;
        value2 = VAR_VALUE(*param2);
    }
    else if (isStrNumber(param2) == SUCCESSFUL)
    {
        value2 = strtod(param2, NULL);
    }
    else
    {
        printf("ET ERROR: %s not a variable or number\n", param2);
        return FAILED;
    }

    if (!strcmp(operator, LESS_THAN))
        return value1 < value2;
    else if (!strcmp(operator, LESS_EQUAL_THAN))
        return value1 <= value2;
    else if (!strcmp(operator, HIGHER_THAN))
        return value1 > value2;
    else if (!strcmp(operator, HIGHER_EQUAL_THAN))
        return value1 >= value2;
    else if (!strcmp(operator, EQUAL_TO))
        return value1 == value2;
    else if (!strcmp(operator, DIFF_FROM))
        return value1 != value2;
    else
    {
        printf("ET ERROR: '%s' is not a valid comparison operator\n", operator);
        return FAILED;
    }

}

// -------------------------------------------- //
// --------------- INSTRUCTIONS --------------- //
// -------------------------------------------- //
int let(char * arg)
{
    int n;
    char * opsFrom;
    char * eq;
    char * rhs;
    char varName;

    if (arg == NULL)
    {
        printf("ET ERROR: let requires an assignment\n");
        return FAILED;
    }

    eq = strchr(arg, '=');
    if (eq == NULL)
    {
        printf("ET ERROR: let requires '=' in assignment\n");
        return FAILED;
    }

    /* buscar la variable destino antes del '=' */
    varName = '\0';
    for (char *p = arg; p < eq; p++)
    {
        if (isalpha((unsigned char)*p))
        {
            if (varName != '\0')
            {
                printf("ET ERROR: let destination must be a single variable\n");
                return FAILED;
            }
            if (!islower((unsigned char)*p))
            {
                printf("ET ERROR: variables identifier must be lowercase\n");
                return FAILED;
            }
            varName = *p;
        }
        else if (!isspace((unsigned char)*p))
        {
            printf("ET ERROR: invalid character before '=' in let\n");
            return FAILED;
        }
    }
    if (varName == '\0')
    {
        printf("ET ERROR: let missing destination variable\n");
        return FAILED;
    }

    rhs = eq + 1;
    while (*rhs != '\0' && isspace((unsigned char)*rhs))
        rhs++;

    if (VAR_PTR(varName) == NULL)
        VAR_PTR(varName) = cursorMemory++;

    opsFrom = rhs;
    if (rhs[0] == '+' || rhs[0] == '-')
        opsFrom = rhs + 1;

    if (!strpbrk(rhs, FULL_ALPHABET) && !strpbrk(opsFrom, "()+-*/%"))
    {
        if (isStrNumber(rhs) == FAILED)
            return FAILED;
        VAR_VALUE(varName) = (int)strtod(rhs, NULL);
    }
    else if (!strpbrk(rhs, "()+-*/%"))
    {
        if (isValidVar(*rhs) == FAILED)
            return FAILED;
        VAR_VALUE(varName) = VAR_VALUE(*rhs);
    }
    else
    {
        char * var = rhs;
        int size = strlen(rhs);

        while ((var = strpbrk(var, FULL_ALPHABET)))
        {
            if (isValidVar(*var) == FAILED)
                return FAILED;

            n = VAR_VALUE(*var);
            size--;
            if (n == 0)
                size++;

            while (n)
            {
                n /= 10;
                size++;
            }

            var++;
        }

        size += 2;
        char temp[size];
        char tempPostfix[size];
        memset(temp, 0, (size_t)size);
        memset(tempPostfix, 0, (size_t)size);

        for (int j = 0, i = 0; rhs[i] != '\0'; i++)
        {
            if (!isalpha(*(rhs+i)))
            {
                sprintf(temp+j, "%c", *(rhs+i));
                j++;
            }
            else
            {
                if (isValidVar(*(rhs+i)) == FAILED)
                    return FAILED;

                int value = VAR_VALUE(*(rhs+i));
                n = value;
                int digits = (value == 0) ? 1 : 0;

                while (n)
                {
                    n /= 10;
                    digits++;
                }
                sprintf(temp+j, "%d", value);
                j += digits;
            }
        }

        convertToPostfix(temp, tempPostfix);
        VAR_VALUE(varName) = evaluatePostfixExpression(tempPostfix);
    }
    VAR_CHAR(varName) = varName;
    VAR_USE(varName) = 1;
  
    return SUCCESSFUL;
}

int input(char * varName)
{
    while (varName && isspace(*varName)) varName++;

    if (*(varName + 1) != '\0' && !isspace((unsigned char)*(varName + 1)))
    {
        printf("ET ERROR: variable's name must be a single char\n");
        return FAILED;
    }
    char var = *varName;
    if (!islower(var))
    {
        printf("ET ERROR: variables identifier must be lowercase\n");
        return FAILED;
    } 

    if (VAR_PTR(var) == NULL)
        VAR_PTR(var) = cursorMemory++;
    printf("  > ");
    if (scanf("%d", VAR_PTR(var)) != 1)
    {
        printf("ET ERROR: failed to read integer from input\n");
        return FAILED;
    }
    printf("\n");
    VAR_USE(var) = 1;
    return SUCCESSFUL;
}

int print(const char * varName)
{
    while (varName && isspace(*varName)) varName++;

    if (*(varName + 1) != '\0' && !isspace((unsigned char)*(varName + 1)))
    {
        printf("ET ERROR: variable name must be a single character\n");
        return FAILED;
    }

    char var = *varName;
    if (isValidVar(var) == FAILED)
        return FAILED;

    printf("%d\n", VAR_VALUE(var));
    return SUCCESSFUL;
}

int goTo(InstructionsListPtr *cursor, char * arg, int * jumped)
{
    if (arg != NULL)
        trimWhitespace(arg);

    if (isStrNumber(arg) == FAILED)
        return FAILED;

    int destLine = strtod(arg, NULL);
    InstructionsListPtr actual_instruction = *cursor;

    *cursor = findLine(destLine);

    if (*cursor)
    {
        *jumped = 1;
        return SUCCESSFUL;
    }
    else
    {
        printf("ET ERROR: line %d not found\n", destLine);
        *cursor = actual_instruction;
        return FAILED;
    }
}

int ifGoto(InstructionsListPtr *cursor, char * arg, int * jumped)
{
    char * gotoInstruction = strstr(arg, "goto");

    if (gotoInstruction == NULL)
    {
        printf("ET ERROR: if statement is missing 'goto'\n");
        return FAILED;
    }

    int condLen = gotoInstruction - arg;
    char condition[condLen + 1];
    strncpy(condition, arg, condLen);
    condition[condLen] = '\0';
    trimWhitespace(condition);
    
    char * posToJump;
    int condResult;

    posToJump = gotoInstruction + 4;
    while (*posToJump && isspace((unsigned char)*posToJump))
        posToJump++;
    trimWhitespace(posToJump);

    condResult = isTrue(condition);
    
    if (condResult == FAILED)
        return FAILED;
    if (!condResult)
        return SUCCESSFUL; /* condición falsa: no salta, pero no es error */
    
    if (isStrNumber(posToJump) == FAILED)
        return FAILED;
    if (goTo(cursor, posToJump, jumped) == FAILED)
        return FAILED;

    return SUCCESSFUL;
}