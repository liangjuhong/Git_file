#include "stm32f10x.h"

void delay_us(uint32_t n);

uint32_t d;      //计算出的距离

//超声波模块引脚
#define	TRIG_PORT      GPIOB		       
#define	ECHO_PORT      GPIOB		
#define	TRIG_PIN       GPIO_Pin_7   	//TRIG-发送引脚PB7       
#define	ECHO_PIN       GPIO_Pin_15		//ECHO-接收引脚PB15   

//超声波初始化函数
void sr04_init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
		 	       
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//时钟使能
    
	GPIO_InitStructure.GPIO_Pin = TRIG_PIN;		//PB7接TRIG
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	      //速度
	GPIO_Init(TRIG_PORT, &GPIO_InitStructure);	         //初始化GPIO 

	GPIO_InitStructure.GPIO_Pin = ECHO_PIN;				 //PB15接ECH0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(ECHO_PORT,&GPIO_InitStructure);		//初始化GPIO 

	GPIO_ResetBits(GPIOB,GPIO_Pin_15);//PC8初始状态为低电平，看时序图	
}

/*获取距离*/
int32_t sr04_get_distance(void)
{
	uint32_t t;	
		
	//PC8高电平
	GPIO_SetBits(TRIG_PORT,TRIG_PIN);
	delay_us(15);	//持续10us以上
	GPIO_ResetBits(TRIG_PORT,TRIG_PIN);//PC8低电平
	
	while(!GPIO_ReadInputDataBit(ECHO_PORT,ECHO_PIN));	//等待高电平
	
	//等待PC9出现高电平
	t=0;
	while(ECHO_PIN==0)
	{	
		//超时处理
		t++;
		delay_us(1);
		
		//如果超时，就返回一个错误码
		if(t >= 300)
			return -1;
	
	}

	//测量高电平的时间
	t=0;
	while(GPIO_ReadInputDataBit(ECHO_PORT,ECHO_PIN))
	{	
		t++;
		delay_us(9);	//9us == 3ms
		
		//如果超时，就返回一个错误码
		if(t >= 200)
			return 45;
	}
		
	//由于测量的时间，就是超声波从发射到返回的时间	
	d = t*0.375;
	
	return d;
	
}	

void delay_us(uint32_t n)
{
	SysTick->CTRL = 0; 			// Disable SysTick，关闭系统定时器
	SysTick->LOAD = (168*n)-1; // 配置计数值(168*n)-1 ~ 0
	SysTick->VAL  = 0; 		// Clear current value as well as count flag
	SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
	while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	SysTick->CTRL = 0; 		// Disable SysTick	
}



