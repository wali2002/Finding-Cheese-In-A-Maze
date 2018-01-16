#ifndef _MAIN_ASM_H
#define _MAIN_ASM_H

#ifdef __cplusplus
    extern "C" { /* our assembly functions have C calling convention */
#endif


void asm_main(void);        /* Make the following available to the C  */
extern const unsigned int StartAddr;
extern       unsigned int SoftXIRQ;
extern       unsigned int intrNum;
void initial(void);
int sensor(void);
void forward(void);
void forward2(void);
int cnt(void);
int cnt2(void);
void turn_left(void);
void turn_right(void);
void mtr_go(void);
void mtr_g(void);
void mtr_spin(void);

void reset_cnt(void);
int right_cnt(void);
void right_adjust(void);
void left_adjust(void);
int sns_r(void);
int sns_l(void);
int sns_f(void);
int wsns_l(void);
int wsns_r(void);
void rst_sns(void);
void reverse(void);
void stop_dead(void);
void straighten(void);
//void asm_main(void);
void wait(int);
//void printPC(char *p);
  /* interface to my assembly main function */

#ifdef __cplusplus
    }
#endif

#endif /* _MAIN_ASM_H */
