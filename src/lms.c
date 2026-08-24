#include <stdio.h>
#include "lms.h"

/* ##### LMS INSTRUCTIONS ##### */
#define READ        10      // A1
#define WRITE       11      // A2
#define READ_STR    12      // A3
#define WRITE_STR   13      // A4
#define PUT_NEWLINE 14      // A5
#define LOAD        20      // B1
#define STORE       21      // B2
#define ADDITION    30      // C1
#define SUBSTRACT   31      // C2
#define MULTIPLY    32      // C3
#define DIVIDE      33      // C4
#define MODULE      34      // C5
#define POWER       35      // C5
#define JUMPTO      40      // D1
#define JUMPIFNEG   41      // D2
#define JUMPIFCERO  42      // D3
#define END         50      // FF
/* ############################ */


#define END_INPUT   -99999
#define POS_COUNT   1000

#define OUTPUT_FILE_NAME "output.txt"

FILE *ptrInputFile = NULL;
FILE *ptrOutputFile = NULL;

int read_success = 0;
int simpletron_memory[POS_COUNT] = {0};
int *cursor_memory = simpletron_memory;
int acumulator = 0;
int instructions[POS_COUNT] = {0};
int *cursor_instruction = instructions;

void read(void);
void read_str(void);
void write(void);
void write_str(void);
void put_newline(void); 
void load(void);
void store(void);
void addition(void);
void substraction(void);
void multiply(void);
void divide(void);
void module(void);
void power(void);
void jump(const int pos);
void jump_if_neg(const int pos);
void jump_if_zero(const int pos);

int get_instructions(void)
{
    printf( "*** Welcome back to Simpletron! ***\n");
    char file_input_name[100] = {'\0'};
    printf("\nEnter source file name:\n  >> ");
    scanf("%s", file_input_name);
    if ((ptrInputFile = fopen(file_input_name, "r")) == NULL)
    {
        printf("\nError: could not open file \"%s\"\n", file_input_name);
        return 0;
    }

    while (fscanf(ptrInputFile, "%d", cursor_instruction) != EOF)
    {
        cursor_instruction++;
    }
    fclose(ptrInputFile);
    printf("\n*** Instructions read successfully ***\n");
    read_success = 1;
    return 1;
}

void exec_instructions(void)
{
    if (!read_success)
    {
        printf("\n*** Execution cancelled ***\n");
        return;
    }

    cursor_instruction = instructions;
    ptrOutputFile = fopen(OUTPUT_FILE_NAME, "w+");
    printf("\n*** Executing instructions ***\n\n");
    do
    {
        if (*cursor_instruction == -99999 ) break;
        int instruction = *cursor_instruction / POS_COUNT;
        int mem_pos     = *cursor_instruction % POS_COUNT;
        if (instruction == END && !(cursor_instruction+1)) break;
        switch (instruction)
        {
        case READ:
            jump(mem_pos);
            read();
            break;
        case WRITE:
            jump(mem_pos);
            write();
            break;
        case READ_STR:
            jump(mem_pos);
            read_str();
            break;
        case WRITE_STR:
            jump(mem_pos);
            write_str();
            break;
        case PUT_NEWLINE:
            jump(mem_pos);
            put_newline();
            break;
        case LOAD:
            jump(mem_pos);
            load();
            break;
        case STORE:
            jump(mem_pos);
            store();
            break;
        case ADDITION:
            jump(mem_pos);
            addition();
            break;
        case SUBSTRACT:
            jump(mem_pos);
            substraction();
            break;
        case MULTIPLY:
            jump(mem_pos);
            multiply();
            break;
        case DIVIDE:
            jump(mem_pos);
            divide();
            break;
        case MODULE:
            jump(mem_pos);
            module();
            break;
        case POWER:
            jump(mem_pos);
            power();
            break;
        case JUMPTO:
            jump(mem_pos);
            break;
        case JUMPIFNEG:
            jump_if_neg(mem_pos);
            break;
        case JUMPIFCERO:
            jump_if_zero(mem_pos);
            break;
        case END:
            break;
        default:
            printf( "Invalid instruction:\n"
                    "    +%02d%02d\n"
                    "    ~^^~~\n",
                    instruction, mem_pos);
            fprintf(ptrOutputFile,  "Invalid instruction:\n"
                                    "    +%02d%03d\n"
                                    "    ~^^~~\n",
                                    instruction, mem_pos);
            break;
        }
        cursor_instruction++;
    } while (1);

    fclose(ptrOutputFile);
    printf("\n*** Execution finished ***\n");
    printf("*** Results saved in \"%s\" ***\n", OUTPUT_FILE_NAME);
    printf("*** Goodbye! ***\n");
    return;
}

// ---------------------------- //
// ----- LMS INSTRUCTIONS ----- //
// ---------------------------- //
void read(void)
{
    scanf("%d", (cursor_memory));
}

void read_str(void)
{
    getchar();

    if (cursor_memory < simpletron_memory
        || cursor_memory >= simpletron_memory + POS_COUNT)
        return;

    unsigned avail_words = (unsigned)(simpletron_memory + POS_COUNT - cursor_memory);
    unsigned int *mem = (unsigned int *)cursor_memory;

    mem[0] = 0;

    unsigned i = 0;
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        unsigned char uc = (unsigned char)c;

        if (i < 2)
            mem[0] |= (unsigned int)uc << (8 * i);
        else
        {
            unsigned j = i - 2;
            unsigned wi = 1 + j / 4;
            unsigned bi = j % 4;
            if (wi >= avail_words)
                break;
            if (bi == 0)
                mem[wi] = 0;
            mem[wi] |= (unsigned int)uc << (8 * bi);
        }
        i++;
    }

    mem[0] |= (unsigned int)(i & 0xFFFFu) << 16;
}

void write(void)
{
    printf("%d", *(cursor_memory));
    fprintf(ptrOutputFile, "%d", *(cursor_memory));
}

void write_str(void)
{
    if (cursor_memory < simpletron_memory
        || cursor_memory >= simpletron_memory + POS_COUNT)
        return;

    unsigned avail_words = (unsigned)(simpletron_memory + POS_COUNT - cursor_memory);
    const unsigned int *mem = (const unsigned int *)cursor_memory;

    unsigned int w0 = mem[0];
    unsigned n = (w0 >> 16) & 0xFFFFu;
    unsigned max_chars = 2 + (avail_words - 1U) * 4U;
    if (n > max_chars)
        n = max_chars;

    for (unsigned i = 0; i < n; i++)
    {
        unsigned char byte;
        if (i < 2)
            byte = (unsigned char)((w0 >> (8 * i)) & 0xFFu);
        else
        {
            unsigned j = i - 2;
            unsigned widx = 1 + j / 4;
            unsigned bout = j % 4;
            if (widx >= avail_words)
                break;
            byte = (unsigned char)((mem[widx] >> (8 * bout)) & 0xFFu);
        }
        printf("%c", byte);
        fprintf(ptrOutputFile, "%c", byte);
    }
    printf("\n");
    fprintf(ptrOutputFile, "\n");
}

void put_newline(void)
{
    printf("\n");
    fprintf(ptrOutputFile, "\n");
}

void load(void)
{
    acumulator = *(cursor_memory);
}

void store(void)
{
    *(cursor_memory) = acumulator;
}

void addition(void)
{
    acumulator += *(cursor_memory);
}

void substraction(void)
{
    acumulator -= *(cursor_memory);
}

void multiply(void)
{
    acumulator *= *(cursor_memory);
}

void divide(void)
{
    if (*(cursor_memory) == 0)
    {
        printf("Error: Division by zero\n");
        fprintf(ptrOutputFile, "Error: Division by zero\n");
        return;
    }
    acumulator /= *(cursor_memory);
}

void module(void)
{
    int tmp_acumulator = (int)acumulator;
    if (*(cursor_memory) == 0)
    {
        printf("Error: Module by zero\n");
        fprintf(ptrOutputFile, "Error: Module by zero\n");
        return;
    }
    if (acumulator - (int)acumulator != 0 || *(cursor_memory) - (int)*(cursor_memory) != 0)
    {
        printf("Error: Module by non-integer\n");
        fprintf(ptrOutputFile, "Error: Module by non-integer\n");
        return;
    }
    acumulator = tmp_acumulator % (int)*(cursor_memory);
}

void power(void)
{
    int exp = *(cursor_memory);
    int base = acumulator;
    if (exp - (int)exp != 0)
    {
        printf("Error: Power by non-integer\n");
        fprintf(ptrOutputFile, "Error: Power by non-integer\n");
        return;
    }
    if (exp == 0)
    {
        acumulator = 1;
        return;
    }
    while (exp < 0)
    {
        acumulator /= base;
        exp++;
    }
    while (--exp > 0)
    {
        acumulator *= base;
    }
}

void jump(const int pos)
{
    cursor_memory = simpletron_memory + pos;
}

void jump_if_neg(const int pos)
{
    int *cursor_instruction_copy = cursor_instruction;
    while (*cursor_instruction_copy/POS_COUNT != END) cursor_instruction_copy++;
    if (acumulator < 0)
    {
        jump(pos);
        *(cursor_instruction_copy + 1) = 0;
        return;
    }

    cursor_instruction = cursor_instruction_copy+1;
    return;
}

void jump_if_zero(const int pos)
{
    int *cursor_instruction_copy = cursor_instruction;
    while (*cursor_instruction_copy/POS_COUNT != END) cursor_instruction_copy++;
    if (acumulator == 0)
    {
        jump(pos);
        *(cursor_instruction_copy + 1) = 0;
        return;
    }

    cursor_instruction = cursor_instruction_copy+1;
    return;
}