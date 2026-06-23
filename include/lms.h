#ifndef LMS_lib
#define LMS_lib

void send_welcome_msg(void);
int get_instructions(void);
void exec_instructions(void);

void read(void);
void write(void);
void load(void);
void store(void);
void addition(void);
void substraction(void);
void multiply(void);
void divide(void);
void jump(const int pos);
void jump_if_neg(const int pos);
void jump_if_zero(const int pos);

#endif
