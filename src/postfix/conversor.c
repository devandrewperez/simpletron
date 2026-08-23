#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "postfix.h"

struct stackNode
{
    char data;
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
static int isOperator(char c);
static int precedence(char operator1, char operator2);
static void push(StackNodePtr *top, char value);
static char pop(StackNodePtr *top);
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

void convertToPostfix(char infix[], char postfix[])
{
    if (infix == NULL) return;

    StackNodePtr tmp = NULL;
    int ic=0, pc=0;

    push(&tmp, '(');
    infix[strlen(infix)] = ')';

    while (!isEmpty(tmp))
    {
        if (infix[ic] == '\0')
            break;

        if (isdigit(infix[ic]) || infix[ic] == ' ' )
        {
            if (infix[ic] == ' ' && pc > 0 && postfix[pc-1] == ' ') 
                push(&tmp, ' ');
            else
                postfix[pc++] = infix[ic];
        }
        else if (infix[ic] == '(')
            push(&tmp, infix[ic]);
        else if (isOperator(infix[ic]))
        {
            while (stackTop(tmp) != '(')
            {
                if (stackTop(tmp) == ' ') 
                    postfix[pc++] = pop(&tmp);
                else if (isOperator(infix[ic]) && precedence(stackTop(tmp), infix[ic]) >= 0)
                    postfix[pc++] = pop(&tmp);
                else
                    break;
            }
            push(&tmp, infix[ic]);
        }
        else if (infix[ic] == ')')
        {
            while (stackTop(tmp) != '(')
            {
                if (stackTop(tmp) == ' ') 
                    postfix[pc++] = pop(&tmp);
                else if (isOperator(stackTop(tmp)))
                    postfix[pc++] = pop(&tmp);
            }
            pop(&tmp);
        }
        ic++;
    }
    postfix[pc] = '\0';
}

static int isOperator(char c)
{
    return (c == PLUS || c == MINUS || c == MULTIPLY || c == DIVIDE || c == REMINDER);
}

static int precedence(char operator1, char operator2)
{
    if (isOperator(operator1) && isOperator(operator2))
    {
        int operator1_value = 0, operator2_value=0;
    
        if (operator1 == PLUS || operator1 == MINUS) operator1_value = 1;
        else operator1_value = 2;
    
        if (operator2 == PLUS || operator2 == MINUS) operator2_value = 1;
        else operator2_value = 2;
    
        return operator1_value - operator2_value;
    }
}

static void push(StackNodePtr *top, char value)
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

static char pop(StackNodePtr *top)
{
    if (*top == NULL) return '\0';
    char item;

    item = (*top)->data;
    *top = (*top)->next; 

    return item;
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
