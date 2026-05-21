#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Init(void);
//void Motor_SetSpeed(int8_t Speed);
void Motor_up(void);
void Motor_Stop(void);
void Motor_Down(void);
void Motor_Left(void);
void Motor_Right(void);
void Motor_Clockwise(void);
void Motor_Anticlockwise(void);
void Enable(void);
void Disenable(void);

#endif
