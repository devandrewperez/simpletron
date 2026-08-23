#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "postfix.h"

struct stackNode
{
    int data;
    struct stackNode *next;
};

#define SIZE        100

#define PLUS        '+'
#define MINUS       '-'
#define MULTIPLY    '*'
#define DIVIDE      '/'
#define REMINDER    '%'


typedef struct stackNode StackNode;
typedef StackNode *StackNodePtr;

static void get_str(char * str);
static int calculate(int op1, int op2, char operator);
static int isOperator(char c);
static void push(StackNodePtr *top, int value);
static int pop(StackNodePtr *top);
static char stackTop(StackNodePtr top);
static int isEmpty(StackNodePtr top);
static void printStack(StackNodePtr top);

static void get_str(char * str)
{
    char c;
    int i=0;
    do
    {
        c = getchar();
        if (c == '\n') break;

        if (isdigit(c) || isOperator(c) || c == ' ' || c == '(' || c == ')')
        {
            *(str+i) = c;
            i++;
        }
    }while(1);
}

int evaluatePostfixExpression(char *expr)
{
    StackNodePtr tmp = NULL;
    int i = 0;
    int number = 0;
    int in_number = 0;

    while (expr[i] != '\0')
    {
        if (isdigit((unsigned char)expr[i]))
        {
            number = number * 10 + (expr[i] - '0');
            in_number = 1;
        }
        else
        {
            if (in_number)
            {
                push(&tmp, number);
                number = 0;
                in_number = 0;
            }
            if (isOperator(expr[i]))
            {
                int y = pop(&tmp);
                int x = pop(&tmp);
                int result = calculate(x, y, expr[i]);
                push(&tmp, result);
            }
        }
        i++;
    }
    if (in_number)
        push(&tmp, number);

    return pop(&tmp);
}

static int calculate(int op1, int op2, char operator)
{
    switch(operator)
    {
        case PLUS:
            return op1 + op2;
        case MINUS:
            return op1 - op2;
        case MULTIPLY:
            return op1 * op2;
        case DIVIDE:
            if (op2 != 0) return op1 / op2;
        case REMINDER:
            if (op2 != 0) return op1 % op2;
    }
}

static int isOperator(char c)
{
    return (c == PLUS || c == MINUS || c == MULTIPLY || c == DIVIDE || c == REMINDER);
}

static void push(StackNodePtr *top, int value)
{
    StackNodePtr newItem = malloc(sizeof(StackNode));

    if (newItem == NULL) return;

    newItem->data = value;
    newItem->next = NULL;

    if (*top == NULL)
    {
        *top = newItem; 
    }
    else
    {
        newItem->next = *top;
        *top = newItem;
    }
}

static int pop(StackNodePtr *top)
{
    if (*top == NULL) return '\0';
    int value;

    value = (*top)->data;
    *top = (*top)->next; 

    return value;
}

static char stackTop(StackNodePtr top)
{
    if (top != NULL) return top->data;
    return '\0';
}

static int isEmpty(StackNodePtr top)
{
    return (top == NULL);
}

static void printStack(StackNodePtr currentPtr)
{
    if (currentPtr == NULL)
    {  
        printf("Stack is empty.\n");
    }
    else
    {
        printf("The stack is:\n\n");
        while (currentPtr != NULL) {
            printf( "%d\n", currentPtr->data);
            currentPtr = currentPtr->next;
        }
    }
    printf("\n");
}
