#include "APP_TASK.H" 
#include "HAL_DS1302.H"
#include "MT_TFTLCD.H"
#include "MT_DS1302.H"
#include "HAL_PWMB_RFID.H"
#include "MT_TK_LED.H"
#include "string.H"
#include "HAL_LOCK.H"
#include "MT_IIC_AT24C128.H"
#include "para.h"
#include "MT_PWMB_RFID.H"
#include "HAL_PWMA_HORN.H"
#include "HAL_WIEGAND_RECEIVE.H"
#include "HAL_WIEGAND.H"
#include "HAL_RS485.H"
#include "MT_MODBUS.H"
MT_TIME_Typedef xdata BackUpDs1302Data; // 
// 创建一个全局的结构体实例，供整个程序使用
str_AppMenuVair xdata AppMenuVair;
extern MT_TIME_Typedef xdata TIME_Task;//声明外部变量
// --- 菜单功能函数声明 ---
static void App_Menu_Desktop(void);
static void App_Menu_Password(void);
static void App_Menu_KeyOpenLock(void);
static void App_Menu_MainMenu(void);
static void App_Menu_DateTimeSeting(void);
static void App_Menu_AddNerUser(void);
static void App_Menu_ManageUserInfo(void);
static void App_Menu_SystemSet(void);
static void App_Menu_History(void);
static void App_Menu_DisPlayRFIDNum(unsigned char x,unsigned char y,unsigned char *p);
static void App_History_Write(unsigned char id,en_historyType type);//记录开锁的时间
void APP_Task_Init()
{
    PartInit();//从EEPROM中读取系统参数
    AppMenuVair.MenuPoint = MENU_DESKTOP;//初始状态为待机界面
    AppMenuVair.UiDisplay = TFTLCD_UI_INIT;//UI需要初始化（完全重绘）
    AppMenuVair.TouchKeyNum = 0xff;//初始化按键值为0xff，表示未按下任何按键

    BackUpDs1302Data.year = 0xff;
    BackUpDs1302Data.month = 0xff;
    BackUpDs1302Data.day = 0xff;
    BackUpDs1302Data.hour = 0xff;
    BackUpDs1302Data.minute = 0xff;
    BackUpDs1302Data.second = 0xff;        
		MT_TFTLCD_Showstring(50,90,"2023-11-13",COLOR_TFTLCD_FOREGROUND,COLOR_TFTLCD_BACKLIGHT,12,0);  //1602
}
void APP_Task()
{
// 1. 使用switch语句，根据当前的状态(MenuPoint)来决定执行哪个功能函数
    switch(AppMenuVair.MenuPoint)
    {
        case MENU_INIT:
                // 初始状态，通常用于执行一次性初始化或直接跳转到主界面
                // 这里可以留空或添加特定逻辑
        break;

        case MENU_DESKTOP:
                App_Menu_Desktop(); // 如果在待机界面，就执行待机界面的处理函数
        break;

        case MENU_KEY_OPENLOCK:
                App_Menu_KeyOpenLock(); // 执行按键开锁界面的处理函数
        break;

        case MENU_PASSWORD:
                App_Menu_Password(); // 如果在密码界面，就执行密码界面的处理函数
        break;

        case MENU_MAINMENU:
                App_Menu_MainMenu(); // 执行主菜单界面的处理函数
        break;

        case MENU_SET_TIME:
                App_Menu_DateTimeSeting(); // 执行时间日期设置界面的处理函数
        break;

        case MENU_AND_USER:
                App_Menu_AddNerUser(); // 执行添加新用户界面的处理函数
        break;

        case MENU_SET_USER:
                App_Menu_ManageUserInfo(); // 执行管理用户信息界面的处理函数
        break;

        case MENU_SET_SETTING:
                App_Menu_SystemSet(); // 执行系统设置界面的处理函数
        break;

        case MENU_HISTORY:
                App_Menu_History(); // 执行历史记录界面的处理函数
        break;
			}
// 2. 实现“无操作自动返回待机界面”的功能
			if(AppMenuVair.MenuPoint != MENU_DESKTOP) // 如果当前不在待机界面
			{
			// 检查按键背光计时器是否已超时 (意味着长时间无按键操作)
					if(Mt_Key_GetBackLightTim() < 30000)
					{
							// 调用菜单切换函数，强制返回到待机桌面
							App_Task_MenuToggle(MENU_DESKTOP, TFTLCD_UI_INIT);        
							MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);//清除屏幕
					}
			}	
}
/**
 * @brief  菜单(状态)跳转函数
 * @param  type: 要跳转的目标菜单
 * @param  refresh: 是否需要刷新UI
 */
void App_Task_MenuToggle(En_Menu_Type type, En_TftlcdUi_Type refresh)
{
    // 1. 更新核心状态变量，指向新的菜单
        AppMenuVair.MenuPoint = type;
    
    // 2. 设置UI刷新标志
        AppMenuVair.UiDisplay = refresh;
    
    // 3. 重置按键值，防止旧的按键影响新界面
        AppMenuVair.TouchKeyNum = 0xff;        
}
//TFTLCD显示屏显示RFID数据
static void App_Menu_DisPlayRFIDNum(unsigned char x,unsigned char y,unsigned char *p)
{
    unsigned long xdata deci;
    deci = (unsigned long)(p[3]);
    deci += ((unsigned long)p[2] << 8);
    deci += ((unsigned long)p[1] << 16);
    deci += ((unsigned long)p[0] << 24);        
    AppMenuVair.DisplayDat[0] = (unsigned char)(deci%10000000000/1000000000 + 0x30);
    AppMenuVair.DisplayDat[1] = (unsigned char)(deci%1000000000/100000000 + 0x30);
    AppMenuVair.DisplayDat[2] = (unsigned char)(deci%100000000/10000000 + 0x30);
    AppMenuVair.DisplayDat[3] = (unsigned char)(deci%10000000/1000000 + 0x30);
    AppMenuVair.DisplayDat[4] = (unsigned char)(deci%1000000/100000 + 0x30);
    AppMenuVair.DisplayDat[5] = (unsigned char)(deci%100000/10000 + 0x30);
    AppMenuVair.DisplayDat[6] = (unsigned char)(deci%10000/1000 + 0x30);
    AppMenuVair.DisplayDat[7] = (unsigned char)(deci%1000/100 + 0x30);
    AppMenuVair.DisplayDat[8] = (unsigned char)(deci%100/10 + 0x30);
    AppMenuVair.DisplayDat[9] = (unsigned char)(deci%10 + 0x30);        
    AppMenuVair.DisplayDat[10] = 0; 
//    MT_TFTLCD_Showstring(x-30,y,"RFID:",WHITE,BLACK,12,0);  
    MT_TFTLCD_Showstring(x,y,&AppMenuVair.DisplayDat,COLOR_TFTLCD_FOREGROUND,COLOR_TFTLCD_BACKLIGHT,16,0); 
}
// --- 待机桌面菜单处理函数 ---
static void App_Menu_Desktop()
{
    static xdata unsigned int DisPlayDelayTim = 0;//RFID数据显示延迟时间
    static xdata unsigned int TIME_Cnt=0;//时间计数器，1s读取一次DS1302时间
    static xdata unsigned int Door_Cnt=0;//门状态计数器，用于显示门状态
		unsigned char userid;              // 存储查询到的用户ID
    unsigned char xdata GetRfidDat[5];//保存读取的RFID数据
    unsigned char idata datDisplay[3];//保存BCD码转换后的字符串
    if(AppMenuVair.UiDisplay == TFTLCD_UI_INIT)//如果UI需要初始化
    {
        AppMenuVair.UiDisplay = TFTLCD_UI_NULL;//初始化UI为无刷新状态
        BackUpDs1302Data.year = 0xff;
        BackUpDs1302Data.month = 0xff;
        BackUpDs1302Data.day = 0xff;
        BackUpDs1302Data.hour = 0xff;
        BackUpDs1302Data.minute = 0xff;
        BackUpDs1302Data.second = 0xff; 
				MT_TFTLCD_Showstring(50,90,"          ",COLOR_TFTLCD_FOREGROUND,COLOR_TFTLCD_BACKLIGHT,12,0);
        MT_TFTLCD_Showstring(50,90,"20  -  -  ",COLOR_TFTLCD_FOREGROUND,COLOR_TFTLCD_BACKLIGHT,12,0);  //1602
        TIME_Cnt=1000;//初始化时间计数器，确保在下一次循环中读取时间更新
        Door_Cnt=0;
        DisPlayDelayTim=0;
    }
    // 显示RFID数据
    if(DisPlayDelayTim)
    {
        DisPlayDelayTim++;
        if(DisPlayDelayTim>=2000)
        {
            DisPlayDelayTim=0;
						MT_TFTLCD_Showstring(10,105,"                 ",WHITE,BLACK,16,0);
						MT_TFTLCD_Showstring(95,0,"                 ",WHITE,BLACK,16,0);
        }
    }
    if(MT_PWMB_GetRfidData(GetRfidDat))
    {
			userid = ParaCheckRfidData(GetRfidDat); // 检查卡号
			if(userid!=0xff)
			{
				DisPlayDelayTim=1;
				Door_Cnt=1;
				Hal_Lock_TriggerOpen();//开锁函数
				App_History_Write(userid+1,OPER_TYPE_OPEN_RFID);//保存开锁的信息
				HAL_WG_Receive_Show();//在屏幕上显示接收到的韦根数据
				MT_Key_SetBacklighttime(MT_Key_Backlighttime);
				
				datDisplay[0] = '0' + (userid +1) / 10;
        datDisplay[1] = '0' + (userid +1) % 10;
        datDisplay[2] = 0;
        MT_TFTLCD_Showstring(10,105,"ID:",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
        MT_TFTLCD_Showstring(35,105,datDisplay,MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
        App_Menu_DisPlayRFIDNum(60,105,GetRfidDat);//显示RFID数据
				
				HAL_PWMA_main(HAL_horn_sound_Di);
				if(PARA_GET_PARA_SYSTEM_WG_SWITCH_STA == SWITCH_OPEN)
        {
          HAL_WG_DataReady(&GetRfidDat[1]);    //韦根26只需要3个字节（偶校验未+24位数据+奇校验位）             
        }
				if(PARA_GET_PARA_SYSTEM_RS485_SWITCH_STA==SWITCH_OPEN)
				{
					mt_modbus_UpdateDat(userid +1,OPER_TYPE_OPEN_RFID,GetRfidDat);
				}
			}
    }
    // 门状态显示时间
    if(Door_Cnt)//
    {
        Door_Cnt++;
        if(Door_Cnt==10)
        {
            MT_TFTLCD_DoorPic(0);
        }
        else if(Door_Cnt==500)
        {
            MT_TFTLCD_DoorPic(1);
        }
        else if(Door_Cnt==2000)
        {
            MT_TFTLCD_DoorPic(2);
            Door_Cnt=0;
        }
    }
		//实时时间显示
		TIME_Cnt++;
		if(TIME_Cnt>=1000)
		{
			TIME_Cnt=0;
			MT_DS1302_Read_Time();//读取DS1302时间
		}
    if(BackUpDs1302Data.hour != TIME_Task.hour)
    {
        BackUpDs1302Data.hour = TIME_Task.hour;        
        MT_DS1302_BCDTOString(BackUpDs1302Data.hour,datDisplay);
		MT_TFTLCD_Showstring (35,40,datDisplay,COLOR_TFTLCD_FOREGROUND,COLOR_TFTLCD_BACKLIGHT,32,0);
    }
    if(BackUpDs1302Data.minute != TIME_Task.minute)
    {
        BackUpDs1302Data.minute = TIME_Task.minute;
        MT_TFTLCD_Showstring(65,37,":",COLOR_TFTLCD_FOREGROUND,COLOR_TFTLCD_BACKLIGHT,32,0);         
        MT_DS1302_BCDTOString(BackUpDs1302Data.minute,datDisplay);
        MT_TFTLCD_Showstring (80,40,datDisplay,COLOR_TFTLCD_FOREGROUND,COLOR_TFTLCD_BACKLIGHT,32,0);
    }
    if(BackUpDs1302Data.second != TIME_Task.second)
    {
        BackUpDs1302Data.second = TIME_Task.second;        
        MT_DS1302_BCDTOString(BackUpDs1302Data.second,datDisplay);
        MT_TFTLCD_Showstring (135,68,datDisplay,COLOR_TFTLCD_FOREGROUND,COLOR_TFTLCD_BACKLIGHT,12,0);
    }
    if(BackUpDs1302Data.year != TIME_Task.year)
    {       
        BackUpDs1302Data.year = TIME_Task.year;        //50  "2023-11-13"
        MT_DS1302_BCDTOString(BackUpDs1302Data.year,datDisplay);                
        MT_TFTLCD_Showstring(62,90,datDisplay,COLOR_TFTLCD_FOREGROUND,COLOR_TFTLCD_BACKLIGHT,12,0);  //1602
    }             
    if(BackUpDs1302Data.month != TIME_Task.month)
    {    
        BackUpDs1302Data.month = TIME_Task.month;        
        MT_DS1302_BCDTOString(BackUpDs1302Data.month,datDisplay);                
        MT_TFTLCD_Showstring(80,90,datDisplay,COLOR_TFTLCD_FOREGROUND,COLOR_TFTLCD_BACKLIGHT,12,0);  //
    }
    if(BackUpDs1302Data.day != TIME_Task.day)
    {     
        BackUpDs1302Data.day = TIME_Task.day;        
        MT_DS1302_BCDTOString(BackUpDs1302Data.day,datDisplay);                
        MT_TFTLCD_Showstring(98,90,datDisplay,COLOR_TFTLCD_FOREGROUND,COLOR_TFTLCD_BACKLIGHT,12,0);  //1602
    }
		//触摸按键键值检测
		AppMenuVair.TouchKeyNum=MT_GetKeyState();
    if(AppMenuVair.TouchKeyNum == 0xff)
      return;
    switch(AppMenuVair.TouchKeyNum)
    {
      case BackLight_Delete:
           MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);//清除屏幕
           App_Task_MenuToggle(MENU_KEY_OPENLOCK,TFTLCD_UI_INIT);
           break;
      case BackLight_ENTER:
           MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);//清除屏幕
           App_Task_MenuToggle(MENU_PASSWORD,TFTLCD_UI_INIT);
           break;
			case BackLight_Ring:
					 HAL_PWMA_main(HAL_horn_sound_DiDong);
					 break;
    }
}
/**
 * @brief  密码开锁菜单处理函数
 */
static void App_Menu_Password(void)
{
    static unsigned char xdata passWord[SYSTEM_PASSWORD_BIT];        // 用于存储用户输入的4位密码
    static unsigned int xdata errorDisPlayDelayTim = 0;     // 错误提示信息的显示计时器
    static unsigned int xdata DisPlayDelayTim = 0;          // 验证成功后，进入主菜单前的延时计时器
		static unsigned int xdata DisPlayDelayTim_NoUser = 0;   //没有用户显示计时器
	  static unsigned char xdata adminNum;										//可用的空闲ID号
    unsigned char xdata GetRfidDat[4]; // 存储获取到的RFID卡号
    unsigned char datDisplay[3];       // 字符串转换缓冲区
    unsigned char userid;              // 存储查询到的用户ID
    unsigned char pdVal = 0;           // 存储当前按键的数值
		unsigned char HaveAddAdiminNum=0;    //判断系统是否增加了管理员，统计增加的管理员的个数
    if(AppMenuVair.UiDisplay == TFTLCD_UI_INIT)
    {
        AppMenuVair.UiDisplay = TFTLCD_UI_NULL;//初始化UI为无刷新状态
			  HaveAddAdiminNum=ParaGetSumNumber(EN_PARATYPE_ADMINUSER,&adminNum);
				if(HaveAddAdiminNum == 0)
				{//系统没有添加管理员
            DisPlayDelayTim_NoUser = 1000;
            MT_TFTLCD_Showstring(40,10,"No User ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);  // 
            return;
				} 
        MT_TFTLCD_Showstring(30,36,"PASSWORD",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);  //  
				MT_TFTLCD_DrawLine(20,92,40,92,MENU_FONT_COLOR);
        MT_TFTLCD_DrawLine(55,92,75,92,MENU_FONT_COLOR);
        MT_TFTLCD_DrawLine(90,92,110,92,MENU_FONT_COLOR);
        MT_TFTLCD_DrawLine(125,92,145,92,MENU_FONT_COLOR);
				
        AppMenuVair.CursorPos = 0; // 光标位置从0开始
        memset(passWord,0,sizeof(passWord));//初始化密码数组为0
        errorDisPlayDelayTim = 0;
        DisPlayDelayTim = 0;
    }
		//没有用户处理
		if(DisPlayDelayTim_NoUser)
    {
        DisPlayDelayTim_NoUser--;
        if(DisPlayDelayTim_NoUser == 0)
        {
           App_Task_MenuToggle(MENU_AND_USER,TFTLCD_UI_INIT);
					 MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
        }
        return;
    }
        // 错误信息显示处理 (软件定时器)
    if(errorDisPlayDelayTim)
    {
        errorDisPlayDelayTim--; // 倒计时
        if(errorDisPlayDelayTim == 0) // 倒计时结束
        {
            // 清除屏幕上的" error  "信息
            MT_TFTLCD_Showstring(40,65,"        ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
        }
        return; // 在显示错误信息期间，暂停处理其他输入
    }
        // 验证成功延时处理 (软件定时器)
    if(DisPlayDelayTim)
    {
        DisPlayDelayTim--; // 倒计时
        if(DisPlayDelayTim == 0) // 倒计时结束
        {
            MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
                        // 正式跳转到主菜单
            App_Task_MenuToggle(MENU_MAINMENU,TFTLCD_UI_INIT);
        }
        return; // 在延时期间，暂停处理其他输入
    }
         // --- 4. 管理员RFID卡验证 ---
    if(MT_PWMB_GetRfidData(GetRfidDat))
    {
         userid = ParaCheckRfidData(GetRfidDat); // 检查卡号
         if(userid < MAX_ADMIN_USER_NUMBER)
         {
                        // 在屏幕上显示管理员ID和卡号作为提示
           datDisplay[0] = '0' + (userid+1) / 10;
           datDisplay[1] = '0' + (userid+1) % 10;
           datDisplay[2] = 0;
					 MT_TFTLCD_BColorfill(0,64,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
           MT_TFTLCD_Showstring(25,80,"ADMIN_ID:",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
           MT_TFTLCD_Showstring(100,80,datDisplay,MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
           DisPlayDelayTim = 2000; // 启动2秒延时，之后将自动跳转到主菜单
         }
    }    
    AppMenuVair.TouchKeyNum=MT_GetKeyState();
    if(AppMenuVair.TouchKeyNum == 0xff)
    return;
    pdVal=0XFF;//初始化按键值为0xFF，表示未按下任何键
    switch(AppMenuVair.TouchKeyNum)
    {
        case BackLight_NUM1:pdVal=1;
        break; 
        case BackLight_NUM2:pdVal=2;
        break; 
        case BackLight_NUM3:pdVal=3;
        break; 
        case BackLight_NUM4:pdVal=4;
        break; 
        case BackLight_NUM5:pdVal=5;
        break;  
        case BackLight_NUM6:pdVal=6;
        break; 
        case BackLight_NUM7:pdVal=7;
        break; 
        case BackLight_NUM8:pdVal=8;
        break; 
        case BackLight_NUM9:pdVal=9;
        break;
        case BackLight_NUM0:pdVal=0;
        break;
        case BackLight_Delete:
						if(AppMenuVair.CursorPos > 0)
						{
								AppMenuVair.CursorPos--;
								passWord[AppMenuVair.CursorPos] = 0xff;
								MT_TFTLCD_Showstring(24+(35*AppMenuVair.CursorPos),65," ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
						}
        break;
        case BackLight_BACK:
						MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
            App_Task_MenuToggle(MENU_DESKTOP,TFTLCD_UI_INIT);
            break;
        case BackLight_ENTER:
            if((AppMenuVair.CursorPos == SYSTEM_PASSWORD_BIT)&&(ParaCheckPassWordData(passWord))!=0xff)
            {
								MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
                App_Task_MenuToggle(MENU_MAINMENU,TFTLCD_UI_INIT);
            }
						else
						{
								MT_TFTLCD_Showstring(24,65,"                 ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
                MT_TFTLCD_Showstring(40,65," error  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
                errorDisPlayDelayTim = 2000;
                AppMenuVair.CursorPos = 0;
						}
            break;
    }
    if(pdVal<10)
    {
        if(AppMenuVair.CursorPos < SYSTEM_PASSWORD_BIT)
        {
            passWord[AppMenuVair.CursorPos] = pdVal;
            MT_TFTLCD_Showstring(24+(35*AppMenuVair.CursorPos),65,"*",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
            AppMenuVair.CursorPos++;
        }
    }
}
/**160 80
 * @brief  按键开锁菜单处理函数
 */
static void App_Menu_KeyOpenLock(void)
{
    static unsigned char xdata passWord[SYSTEM_PASSWORD_BIT];        // 用于存储用户输入的4位密码
    static unsigned int xdata errorDisPlayDelayTim = 0;     // 错误提示信息的显示计时器
    static unsigned int xdata DisPlayDelayTim = 0;          // 验证成功后，进入主菜单前的延时计时器
    unsigned char xdata GetRfidDat[4]; // 存储获取到的RFID卡号
//    unsigned char datDisplay[3];       // 字符串转换缓冲区
    unsigned char userid,userID;              // 存储查询到的用户ID
    unsigned char pdVal = 0;           // 存储当前按键的数值
    if(AppMenuVair.UiDisplay == TFTLCD_UI_INIT)
    {
        AppMenuVair.UiDisplay = TFTLCD_UI_NULL;//初始化UI为无刷新状态
        MT_TFTLCD_Showstring(30,36,"PASSWORD",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);  // 
				MT_TFTLCD_DrawLine(20,92,40,92,MENU_FONT_COLOR);
        MT_TFTLCD_DrawLine(55,92,75,92,MENU_FONT_COLOR);
        MT_TFTLCD_DrawLine(90,92,110,92,MENU_FONT_COLOR);
        MT_TFTLCD_DrawLine(125,92,145,92,MENU_FONT_COLOR);
				
				AppMenuVair.CursorPos = 0; // 光标位置从0开始
        memset(passWord,0,sizeof(passWord));//初始化密码数组为0
        errorDisPlayDelayTim = 0;
        DisPlayDelayTim = 0;
    }
            // 错误信息显示处理 (软件定时器)
    if(errorDisPlayDelayTim)
    {
        errorDisPlayDelayTim--; // 倒计时
        if(errorDisPlayDelayTim == 0) // 倒计时结束
        {
            // 清除屏幕上的" error  "信息
            MT_TFTLCD_Showstring(40,65,"        ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
        }
        return; // 在显示错误信息期间，暂停处理其他输入
    }
        // 验证成功延时处理 (软件定时器)
    if(DisPlayDelayTim)
    {
        DisPlayDelayTim--; // 倒计时
        if(DisPlayDelayTim == 0) // 倒计时结束
        {
            MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
                        // 正式跳转到主菜单
            App_Task_MenuToggle(MENU_DESKTOP,TFTLCD_UI_INIT);
        }
        return; // 在延时期间，暂停处理其他输入
    }
         // --- 4. 管理员RFID卡验证 ---
    if(MT_PWMB_GetRfidData(GetRfidDat))
    {
      userid = ParaCheckRfidData(GetRfidDat); // 检查RFID卡号是否已录入
			if(userid!=0xff)
			{
				App_History_Write(userid+1,OPER_TYPE_OPEN_RFID);//保存开锁的信息
				Hal_Lock_TriggerOpen();          
//				App_Menu_DisPlayRFIDNum(70,110,GetRfidDat);
				DisPlayDelayTim = 1000; // 启动2秒延时，之后将自动跳转到主菜单
			}
    }    
    AppMenuVair.TouchKeyNum=MT_GetKeyState();
    if(AppMenuVair.TouchKeyNum == 0xff)
    return;
    pdVal=0XFF;//初始化按键值为0xFF，表示未按下任何键
    switch(AppMenuVair.TouchKeyNum)
    {
        case BackLight_NUM1:pdVal=1;
        break; 
        case BackLight_NUM2:pdVal=2;
        break; 
        case BackLight_NUM3:pdVal=3;
        break; 
        case BackLight_NUM4:pdVal=4;
        break; 
        case BackLight_NUM5:pdVal=5;
        break;  
        case BackLight_NUM6:pdVal=6;
        break; 
        case BackLight_NUM7:pdVal=7;
        break; 
        case BackLight_NUM8:pdVal=8;
        break; 
        case BackLight_NUM9:pdVal=9;
        break; 
        case BackLight_NUM0:pdVal=0;
        break; 
        case BackLight_Delete:
            if(AppMenuVair.CursorPos > 0)
            {
							AppMenuVair.CursorPos--;
							passWord[AppMenuVair.CursorPos] = 0xff;
							MT_TFTLCD_Showstring(24+(35*AppMenuVair.CursorPos),65," ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
            }
            break;
        case BackLight_BACK:
						MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
            App_Task_MenuToggle(MENU_DESKTOP,TFTLCD_UI_INIT);
            break;
        case BackLight_ENTER:
						userID=ParaCheckPassWordData(passWord);
            if((AppMenuVair.CursorPos == SYSTEM_PASSWORD_BIT)&&(userID!=0xff))
            {
							App_History_Write(userID+1,OPER_TYPE_OPEN_PASSWORD);//保存开锁的信息
							MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
							App_Task_MenuToggle(MENU_DESKTOP,TFTLCD_UI_INIT);
							Hal_Lock_TriggerOpen();
            }
            else
            {
							MT_TFTLCD_Showstring(24,65,"                 ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
							MT_TFTLCD_Showstring(40,65," error  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
							errorDisPlayDelayTim = 2000;
							AppMenuVair.CursorPos = 0;
            }
            break;
    }
    if(pdVal<10)
    {
        if(AppMenuVair.CursorPos < SYSTEM_PASSWORD_BIT)
        {
            passWord[AppMenuVair.CursorPos] = pdVal;
            MT_TFTLCD_Showstring(24+(35*AppMenuVair.CursorPos),65,"*",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
            AppMenuVair.CursorPos++;
        }
    }
}

/**
 * @brief  主菜单处理函数
 */
static void App_Menu_MainMenu(void)
{
    unsigned char pdVal = 0; // 临时变量，用于存储按键转换后的功能码 
    static unsigned char xdata backupCursorPos = 0;// 使用静态变量备份光标位置，以便从子菜单返回时恢复
    // 此处将添加主菜单的显示与选择逻辑
    if((AppMenuVair.UiDisplay == TFTLCD_UI_INIT) || (AppMenuVair.UiDisplay == TFTLCD_UI_RECORER))
        {
						// 如果是“完全重绘”，则将光标位置重置为0
					if(AppMenuVair.UiDisplay == TFTLCD_UI_INIT)
					{
						backupCursorPos = 0;
					}
					 AppMenuVair.UiDisplay = TFTLCD_UI_NULL; // 清除UI刷新标志
					 AppMenuVair.CursorPos=backupCursorPos;
								// 绘制菜单标题和所有菜单项
					 MT_TFTLCD_Showstring(28,0, "   MENU   ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
					 MT_TFTLCD_Showstring(10,24,"  1.Date&Time     ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
					 MT_TFTLCD_Showstring(10,44,"  2.Add User      ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
					 MT_TFTLCD_Showstring(10,64,"  3.User Manage   ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
					 MT_TFTLCD_Showstring(10,84,"  4.Setting       ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
					 MT_TFTLCD_Showstring(10,104,"  5.History     ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
					 
								// 在当前光标位置绘制 ">" 符号
					 MT_TFTLCD_Showstring(10,(24+AppMenuVair.CursorPos*20),"> ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
        }
        
        AppMenuVair.TouchKeyNum = MT_GetKeyState();
        if(AppMenuVair.TouchKeyNum == 0xff)//如果没有按键按下
        return;
        pdVal=0XFF;//初始化按键值为0xFF，表示未按下任何键
        switch(AppMenuVair.TouchKeyNum)
        {
            case BackLight_UP:
            {
                pdVal=11;
            }
            break; 
            case BackLight_DOWN:
            {
                pdVal=12;
            }
            break; 
            case BackLight_BACK:
            {
								MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
                App_Task_MenuToggle(MENU_DESKTOP,TFTLCD_UI_INIT);        
            }
            break;
            case BackLight_ENTER:
            {
                MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
								backupCursorPos = AppMenuVair.CursorPos; // 在跳转前，备份当前光标位置
                // 利用枚举值的连续性，通过光标位置的偏移量直接跳转到对应的菜单状态
                App_Task_MenuToggle(MENU_SET_TIME + AppMenuVair.CursorPos, TFTLCD_UI_INIT);       
            }
            break;                           
        }
                // --- 3. 光标移动逻辑 ---
        if(pdVal == 11) // 如果是“向上”
        {
            if(AppMenuVair.CursorPos == 0) // 如果光标在第一项
            {
                AppMenuVair.CursorPos = 5; // 循环到最后一项
            }
            AppMenuVair.CursorPos--; // 光标上移
        }
        if(pdVal == 12) // 如果是“向下”
        {
            AppMenuVair.CursorPos++; // 光标下移
            if(AppMenuVair.CursorPos == 5) // 如果光标超过最后一项
            {
                AppMenuVair.CursorPos = 0; // 循环到第一项
            }
        }
				        // --- 4. 光标重绘 ---
        if(pdVal != 0xff) // 如果按下了有效的功能键 (上/下)
        {
          // 先用空格清除所有可能位置的旧光标
          MT_TFTLCD_Showstring(10,24,"  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
          MT_TFTLCD_Showstring(10,44,"  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
          MT_TFTLCD_Showstring(10,64,"  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
          MT_TFTLCD_Showstring(10,84,"  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
          MT_TFTLCD_Showstring(10,104,"  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);

          // 在新的光标位置重新绘制 ">" 符号
          MT_TFTLCD_Showstring(10,(24+AppMenuVair.CursorPos*20),"> ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
        }
}
// 函数:  static void App_Menu_DateTime_Display(unsigned char Cursor, unsigned char state,str_SystemDisplayer timeBuffer)
// 描述:  实现时间闪烁函数
// 参数1: 无
// 参数2: 无
// 返回:  无
//========================================================================
static void App_Menu_DateTime_Display(unsigned char Cursor, unsigned char state,str_SystemDisplayer timeBuffer)
{
        //=========================显示日期
        AppMenuVair.DisplayDat[0] = '0' + timeBuffer.year_Hbit;
        AppMenuVair.DisplayDat[1] = '0' + timeBuffer.year_Lbit;                
        AppMenuVair.DisplayDat[2] = '-';                        
        AppMenuVair.DisplayDat[3] = '0' + timeBuffer.month_Hbit;
        AppMenuVair.DisplayDat[4] = '0' + timeBuffer.month_Lbit;                
        AppMenuVair.DisplayDat[5] = '-';                
        AppMenuVair.DisplayDat[6] = '0' + timeBuffer.day_Hbit;
        AppMenuVair.DisplayDat[7] = '0' + timeBuffer.day_Lbit;
        AppMenuVair.DisplayDat[8] = 0;
        if(state == 0)
        {
                switch(Cursor)
                {
                        case 1:
                                AppMenuVair.DisplayDat[0] = ' ';        
                        break;
                        case 2:
                                AppMenuVair.DisplayDat[1] = ' ';                
                        break;
                        case 3:
                                AppMenuVair.DisplayDat[3] = ' ';                                        
                        break;
                        case 4:
                                AppMenuVair.DisplayDat[4] = ' ';                                        
                        break;
                        case 5:
                                AppMenuVair.DisplayDat[6] = ' ';                                        
                        break;
                        case 6:
                                AppMenuVair.DisplayDat[7] = ' ';                                        
                        break;        
                }        
        }
        MT_TFTLCD_Showstring(58,44,AppMenuVair.DisplayDat,MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0); 
        //=========================显示时间
        AppMenuVair.DisplayDat[0] = '0' + timeBuffer.hour_Hbit;
        AppMenuVair.DisplayDat[1] = '0' + timeBuffer.hour_Lbit;                
        AppMenuVair.DisplayDat[2] = ':';                
        AppMenuVair.DisplayDat[3] = '0' + timeBuffer.minute_Hbit;
        AppMenuVair.DisplayDat[4] = '0' + timeBuffer.minute_Lbit;
        AppMenuVair.DisplayDat[5] = ':';                
        AppMenuVair.DisplayDat[6] = '0' + timeBuffer.sec_Hbit;
        AppMenuVair.DisplayDat[7] = '0' + timeBuffer.sec_Lbit;                
        AppMenuVair.DisplayDat[8] = 0;        
        
        if(state == 0)
        {
					switch(Cursor)
					{
						case 7:
										AppMenuVair.DisplayDat[0] = ' ';        
						break;
						case 8:
										AppMenuVair.DisplayDat[1] = ' ';                
						break;
						case 9:
										AppMenuVair.DisplayDat[3] = ' ';                                        
						break;
						case 10:
										AppMenuVair.DisplayDat[4] = ' ';                                        
						break;
						case 11:
										AppMenuVair.DisplayDat[6] = ' ';                                        
						break;
						case 12:
										AppMenuVair.DisplayDat[7] = ' ';                                        
						break;        
					}        
        }
        MT_TFTLCD_Showstring(58,84,AppMenuVair.DisplayDat,MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
        //清除数字下的白线
        MT_TFTLCD_DrawLine(58,60,65,60,COLOR_TFTLCD_BACKLIGHT);
        MT_TFTLCD_DrawLine(67,60,74,60,COLOR_TFTLCD_BACKLIGHT);
        MT_TFTLCD_DrawLine(81,60,88,60,COLOR_TFTLCD_BACKLIGHT);
        MT_TFTLCD_DrawLine(90,60,97,60,COLOR_TFTLCD_BACKLIGHT);
        MT_TFTLCD_DrawLine(105,60,112,60,COLOR_TFTLCD_BACKLIGHT);
        MT_TFTLCD_DrawLine(114,60,121,60,COLOR_TFTLCD_BACKLIGHT);                
        MT_TFTLCD_DrawLine(58,100,65,100,COLOR_TFTLCD_BACKLIGHT);
        MT_TFTLCD_DrawLine(67,100,74,100,COLOR_TFTLCD_BACKLIGHT);
        MT_TFTLCD_DrawLine(81,100,88,100,COLOR_TFTLCD_BACKLIGHT);
        MT_TFTLCD_DrawLine(90,100,97,100,COLOR_TFTLCD_BACKLIGHT);
        MT_TFTLCD_DrawLine(105,100,112,100,COLOR_TFTLCD_BACKLIGHT);
        MT_TFTLCD_DrawLine(114,100,121,100,COLOR_TFTLCD_BACKLIGHT);        
        if(state)        
        {
					switch(AppMenuVair.CursorPos)
					{
						case 1:
										MT_TFTLCD_DrawLine(58,60,65,60,MENU_FONT_COLOR);                                                        
						break;
						case 2:
										MT_TFTLCD_DrawLine(67,60,74,60,MENU_FONT_COLOR);                                        
						break;                        
						case 3:
										MT_TFTLCD_DrawLine(81,60,88,60,MENU_FONT_COLOR);                                
						break;                        
						case 4:
										MT_TFTLCD_DrawLine(90,60,97,60,MENU_FONT_COLOR);                                                        
						break;
						case 5:
										MT_TFTLCD_DrawLine(105,60,112,60,MENU_FONT_COLOR);                        
						break;                        
						case 6:
										MT_TFTLCD_DrawLine(114,60,121,60,MENU_FONT_COLOR);                                                                
						break;        
						case 7:
										MT_TFTLCD_DrawLine(58,100,65,100,MENU_FONT_COLOR);                        
						break;
						case 8:
										MT_TFTLCD_DrawLine(67,100,74,100,MENU_FONT_COLOR);                        
						break;                        
						case 9:        
										MT_TFTLCD_DrawLine(81,100,88,100,MENU_FONT_COLOR);                        
						break;                        
						case 10:
										MT_TFTLCD_DrawLine(90,100,97,100,MENU_FONT_COLOR);                                
						break;
						case 11:                        
										MT_TFTLCD_DrawLine(105,100,112,100,MENU_FONT_COLOR);                                
						break;                        
						case 12:
										MT_TFTLCD_DrawLine(114,100,121,100,MENU_FONT_COLOR);                                        
						break;        
					}                        
			}
}
// 检测用户输入的时间数据是否有效
static unsigned char App_Menu_DateTime_DetectPutInData(str_SystemDisplayer timeBuffer)
{
        unsigned char dat;
        dat = timeBuffer.month_Lbit + timeBuffer.month_Hbit *10;        
        if((dat > 0) && (dat < 13))
        { ///月份有效取值1-12                
        }
        else
         return 0;                                        

        dat = timeBuffer.day_Lbit + timeBuffer.day_Hbit *10;        
				if((dat > 0) && (dat < 32))
        { ///日1-31          此处还可以进一步优化；
        }
        else
         return 0;        

        dat = timeBuffer.hour_Lbit + timeBuffer.hour_Hbit *10;        
				if(dat < 24)
        { ///小时 0-23
        }
        else
         return 0;
        dat = timeBuffer.minute_Lbit + timeBuffer.minute_Hbit *10;        
				if(dat < 60)
        { ///分钟 0-59
        }
        else
         return 0;         
        dat = timeBuffer.sec_Lbit + timeBuffer.sec_Hbit *10;        
				if(dat < 60)
        { ///秒 0-59
        }
        else
         return 0;        

        return 1;                                         
}
/**
 * @brief  时间日期设置菜单处理函数
 */
static void App_Menu_DateTimeSeting(void)
{
    static str_SystemDisplayer xdata timeBuffer; // 存储用户正在编辑的十进制时间(BCD码转换为十进制)
    static unsigned int xdata BlinkTimer = 0;    // 光标闪烁计时器
    static unsigned char xdata BlinkState = 0;   // 光标闪烁状态 (0:隐藏, 1:显示)

        // --- 临时变量定义 ---
    unsigned char xdata pdVal = 0;           // 存储按键转换后的数值
//    str_Mt_Ds1302Data xdata ds1302_data;   // 用于存储待写入DS1302的BCD码数据
    unsigned char *pdat;               // 用于通过指针操作timeBuffer的指针变量
    if(AppMenuVair.UiDisplay == TFTLCD_UI_INIT)
        {
            AppMenuVair.UiDisplay = TFTLCD_UI_NULL; // 清除UI刷新标志
                // 绘制菜单标题和所有菜单项
            MT_TFTLCD_Showstring(30,0,"Date&Time",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
            MT_TFTLCD_Showstring(10,44,"Date:        ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
            MT_TFTLCD_Showstring(10,84,"Time:        ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //

             // 从DS1302读取当前时间
            MT_DS1302_Read_Time();
             // 将读取到的BCD码时间转换为十进制，并拆分存入timeBuffer
            timeBuffer.year_Hbit   = (TIME_Task.year >> 4) & 0x0f;
            timeBuffer.year_Lbit   = TIME_Task.year & 0x0f;
            timeBuffer.month_Hbit  = (TIME_Task.month >> 4) & 0x0f;
            timeBuffer.month_Lbit  = TIME_Task.month & 0x0f;
            timeBuffer.day_Hbit    = (TIME_Task.day >> 4) & 0x0f;
            timeBuffer.day_Lbit    = TIME_Task.day & 0x0f;
            timeBuffer.hour_Hbit   = (TIME_Task.hour >> 4) & 0x0f;
            timeBuffer.hour_Lbit   = TIME_Task.hour & 0x0f;
            timeBuffer.minute_Hbit = (TIME_Task.minute >> 4) & 0x0f;
            timeBuffer.minute_Lbit = TIME_Task.minute & 0x0f;
            timeBuffer.sec_Hbit    = (TIME_Task.second >> 4) & 0x0f;
            timeBuffer.sec_Lbit    = TIME_Task.second & 0x0f;

             // 初始化光标和闪烁状态
            AppMenuVair.CursorPos = 1; // 光标从第一个位置开始
            BlinkTimer = 1000;
            BlinkState = 1;
            AppMenuVair.DisplayTim = 0; // 清除“设置成功”的延时计时器
        }
        // --- 6. 设置成功后，延时返回主界面 ---
        if(AppMenuVair.DisplayTim)
        {
            AppMenuVair.DisplayTim--; // 倒计时
            if(AppMenuVair.DisplayTim == 0) // 倒计时结束
            {
								MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
               // 返回主菜单，并传递“恢复”标志，以便主菜单记忆上次的光标位置
               App_Task_MenuToggle(MENU_MAINMENU,TFTLCD_UI_RECORER);
            }
            return; // 在延时期间，不处理任何其他操作
        }
        // --- 2. 光标闪烁逻辑 (软件定时器) ---
        BlinkTimer++;
        if(BlinkTimer > 500) // 大约每400ms翻转一次状态
        {
                BlinkTimer = 0;
                BlinkState = !BlinkState; // 翻转显示/隐藏状态
                // 调用显示函数，只更新光标的闪烁效果
                App_Menu_DateTime_Display(AppMenuVair.CursorPos, BlinkState, timeBuffer);
        } 

        AppMenuVair.TouchKeyNum = MT_GetKeyState();
        if(AppMenuVair.TouchKeyNum == 0xff) return; // 无按键则返回
        pdVal = 0xff; // 初始化为无效值
        switch(AppMenuVair.TouchKeyNum)
        {
            case BackLight_UP: pdVal = 11; break; // 设置为“向上”功能码
            case BackLight_DOWN: pdVal = 12; break; // 设置为“向下”功能码
            case BackLight_BACK:
            {
                MT_TFTLCD_Showstring(28,0,  "       ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
                App_Task_MenuToggle(MENU_MAINMENU,TFTLCD_UI_RECORER);        
            }
            break;
            case BackLight_ENTER: // 确认键，用于保存
                        if(App_Menu_DateTime_DetectPutInData(timeBuffer) == 1) // 调用函数检查数据合法性
                        {// 数据合法
																MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
                                MT_TFTLCD_Showstring(10,44," Set Successful ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
                                MT_TFTLCD_Showstring(10,84,"                ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
                                // 将十进制的timeBuffer数据合并为BCD码
                                TIME_Task.year   = timeBuffer.year_Lbit   + (timeBuffer.year_Hbit << 4);
                                TIME_Task.month  = timeBuffer.month_Lbit  + (timeBuffer.month_Hbit << 4);
                                TIME_Task.day    = timeBuffer.day_Lbit    + (timeBuffer.day_Hbit << 4);
                                TIME_Task.hour   = timeBuffer.hour_Lbit   + (timeBuffer.hour_Hbit << 4);
                                TIME_Task.minute = timeBuffer.minute_Lbit + (timeBuffer.minute_Hbit << 4);
                                TIME_Task.second = timeBuffer.sec_Lbit    + (timeBuffer.sec_Hbit << 4);
                                // 将BCD码写入DS1302芯片
                                MT_DS1302_SET_Time();
                        }
                         else
                        {// 数据不合法
																MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
                                MT_TFTLCD_Showstring(10,44," Invalid data   ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
                                MT_TFTLCD_Showstring(10,84,"   Fail         ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
                        }
                        AppMenuVair.DisplayTim = 1500; // 启动1.5秒的提示信息显示计时器
                        return;
                                                                // --- 数字键转换 --- 
                case BackLight_NUM1: pdVal = 1; break;
                case BackLight_NUM2: pdVal = 2; break;
                case BackLight_NUM3: pdVal = 3; break;
                case BackLight_NUM4: pdVal = 4; break;
                case BackLight_NUM5: pdVal = 5; break;
                case BackLight_NUM6: pdVal = 6; break;
                case BackLight_NUM7: pdVal = 7; break;
                case BackLight_NUM8: pdVal = 8; break;
                case BackLight_NUM9: pdVal = 9; break;
                case BackLight_NUM0: pdVal = 0; break;                            
        }
        if(pdVal < 10) // 如果按下了数字键
        {
                pdat = (unsigned char *)(&timeBuffer); // 让指针指向timeBuffer的起始地址
                // 利用指针和光标位置的偏移量，直接修改对应的字节
                pdat[AppMenuVair.CursorPos - 1] = pdVal;
                
                AppMenuVair.CursorPos++; // 自动将光标后移一位
                if(AppMenuVair.CursorPos == 13) // 循环到头
                {
                   AppMenuVair.CursorPos = 1;
                }
        }
        else if((pdVal == 11) || (pdVal == 12)) // 如果按下了上/下键
        {
                if(pdVal == 11) // 向上
                {
                   AppMenuVair.CursorPos--;
                   if(AppMenuVair.CursorPos == 0)
                   {
                      AppMenuVair.CursorPos = 12;
                   }
                }
                else if(pdVal == 12) // 向下
                {
                    AppMenuVair.CursorPos++;
                    if(AppMenuVair.CursorPos == 13)
                    {
                      AppMenuVair.CursorPos = 1;
                    }
                }
        }
}
/**
 * @brief  添加新用户菜单处理函数
 */
static void App_Menu_AddNerUser(void)
{
			static unsigned char xdata passWord[SYSTEM_PASSWORD_BIT]; // 存储正在输入的密码
			static unsigned char xdata GetRfidDat[4];                 // 存储刷入的RFID卡号
			static unsigned int xdata errorDisPlayDelayTim = 0;     	// 临时消息的显示计时器
			static unsigned int xdata OverDleyTim = 0;              	// 整体操作的超时计时器
			static unsigned char xdata adminNum, normalNum;           // 可用的管理员/普通用户ID
			static unsigned char xdata HaveAddAdiminNum, HaveAddNormalNum; // 已存在的管理员/普通用户数量
			static unsigned char xdata UserType; 											// 标记当前选择的用户类型 (0:Admin, 1:Normal
			
			unsigned char xdata i;
			str_AdminUserVair xdata adVair;     // 管理员用户数据结构体
			str_NormalUserVair xdata NoVari;    // 普通用户数据结构体
			unsigned char pdVal = 0;			//保存按键值
			if(AppMenuVair.UiDisplay == TFTLCD_UI_INIT)
			{
					AppMenuVair.UiDisplay = TFTLCD_UI_NULL; // 清除UI刷新标志
					// 绘制菜单标题
					MT_TFTLCD_Showstring(40,0, "Add User",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);  //
					AppMenuVair.Refresh = 1;//1表示刷新屏幕  0 表示不刷新
					AppMenuVair.submenuTag = 0; //0选择用户类型   1 添加RFID  2添加管理员密码 3添加用户成功 4返回主界面
					AppMenuVair.CursorPos = 0;  //光标
					OverDleyTim = MENU_WAIT_OVER_TIME; //操作超时  20秒没有无操作，则返回
					errorDisPlayDelayTim = 0; //提示信息等待延时时间；
					// 获取当前已注册的用户信息
					HaveAddAdiminNum = ParaGetSumNumber(EN_PARATYPE_ADMINUSER, &adminNum);
					HaveAddNormalNum = ParaGetSumNumber(EN_PARATYPE_NORMALUSER, &normalNum);
			}
	
      if(errorDisPlayDelayTim) // 处理临时消息的显示
      {
        errorDisPlayDelayTim --;
        if(errorDisPlayDelayTim == 0)
        {
            if(AppMenuVair.submenuTag == 1)
            {///提示输入RFID 已经学习完成 结束
                MT_TFTLCD_Showstring(20,50,"  Swipe rfid    ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                MT_TFTLCD_Showstring(20,70,"  Waiting...  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                OverDleyTim = MENU_WAIT_OVER_TIME;//更新无操作超时时间；        
            }
            else if(AppMenuVair.submenuTag == 3)
            {//普通用户，添加用户成功，直接退出；
                AppMenuVair.submenuTag = 3; 
                AppMenuVair.Refresh = 1;        
                MT_TFTLCD_Showstring(0,60,"                    ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
            }
         }                
         return;
       }
       if(OverDleyTim) // 操作超时，20秒没有无操作，则返回
       {
					 OverDleyTim--;
					 if(OverDleyTim==0)
					 {
							AppMenuVair.submenuTag = 4; // 强制切换到退出步骤
							AppMenuVair.Refresh = 1;    // 触发UI刷新
					 }
       }
	    if(AppMenuVair.Refresh)
        {
                AppMenuVair.Refresh = 0; // 清除刷新标志
                switch(AppMenuVair.submenuTag)
                {
                        case 0: // 步骤0: 选择要添加的用户类型
                        {
                                MT_TFTLCD_Showstring(10,30,"Add user type:",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //选择增加的用户类型
                                MT_TFTLCD_Showstring(20,50,"> Admin       ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                                MT_TFTLCD_Showstring(20,70,"  Normal      ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                                OverDleyTim = MENU_WAIT_OVER_TIME;
                                AppMenuVair.DisplayDat[0] = HaveAddAdiminNum/10 + '0';
                                AppMenuVair.DisplayDat[1] = HaveAddAdiminNum%10 + '0';
                                AppMenuVair.DisplayDat[2] = '/';
                                AppMenuVair.DisplayDat[3] = MAX_ADMIN_USER_NUMBER + '0';
                                AppMenuVair.DisplayDat[4] = 0;
                                MT_TFTLCD_Showstring(88,50,AppMenuVair.DisplayDat,MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                                
                                AppMenuVair.DisplayDat[0] = HaveAddNormalNum/10 + '0';
                                AppMenuVair.DisplayDat[1] = HaveAddNormalNum%10 + '0';
                                AppMenuVair.DisplayDat[2] = '/';
                                AppMenuVair.DisplayDat[3] = MAX_USER_NUMBER/10 + '0';
                                AppMenuVair.DisplayDat[4] = MAX_USER_NUMBER%10 + '0';
                                AppMenuVair.DisplayDat[5] = 0;
                                MT_TFTLCD_Showstring(88,70,AppMenuVair.DisplayDat,MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //                                
                        }
                        break;
                        case 1: // 步骤1: 提示用户刷入RFID卡
                        {
                                MT_TFTLCD_Showstring(10,30,"Add RFID cards ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //选择增加的用户类型
                                MT_TFTLCD_Showstring(20,50,"  Swipe rfid    ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                                MT_TFTLCD_Showstring(20,70,"  Waiting...  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //

                                OverDleyTim = MENU_WAIT_OVER_TIME;
                        }
                        break;
                        case 2: // 步骤2: 提示用户输入密码 (仅限管理员)
                        {
                                MT_TFTLCD_Showstring(10,30,"Add Password  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //选择增加的用户类型
                                MT_TFTLCD_Showstring(0,60,"                    ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                                MT_TFTLCD_DrawLine(40,90,55,90,MENU_FONT_COLOR);
                                MT_TFTLCD_DrawLine(65,90,80,90,MENU_FONT_COLOR);
                                MT_TFTLCD_DrawLine(90,90,105,90,MENU_FONT_COLOR);
                                MT_TFTLCD_DrawLine(115,90,130,90,MENU_FONT_COLOR);//
                                AppMenuVair.CursorPos  = 0;
                                OverDleyTim = MENU_WAIT_OVER_TIME;
                        }
                        break;
                        case 3: // 步骤3: 显示成功信息并保存数据
                        {
                                MT_TFTLCD_Showstring(10,30,"Add New User    ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //选择增加的用户类型                                
                                MT_TFTLCD_Showstring(10,68,"  success  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);  //选择增加的用户类型
                                errorDisPlayDelayTim = 1000; // 显示1秒成功信息
                                AppMenuVair.submenuTag = 4;        
                                AppMenuVair.Refresh = 1;
																if(UserType == 0)
																{
																	for(i=0;i<4;i++)
																			{
																					adVair.password[i] = passWord[i];
																					adVair.rfidData[i] = GetRfidDat[i];
																			}
																			ParaSaveAdminVair(adminNum,adVair);
																}
                                
                        }
                        break;
                        case 4: // 步骤4: 退出界面
                        {
														MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
														if(!ParaGetSumNumber(EN_PARATYPE_ADMINUSER, &adminNum))//如果没有管理员则直接退出到桌面
														{
															App_Task_MenuToggle(MENU_DESKTOP,TFTLCD_UI_INIT);
														}
														else
                            App_Task_MenuToggle(MENU_MAINMENU,TFTLCD_UI_RECORER);//退出到主菜单        
                        }
                        break;
                }
        } 
        if(AppMenuVair.submenuTag == 1)
        {//增加RFID 卡片
                if(MT_PWMB_GetRfidData(GetRfidDat))
                {        ///获取RFID 数据成功
                        if(ParaCheckRfidData(GetRfidDat) == 0xff)
                        {//RFID 系统中没有被学习，添加RFID卡成功；
                                if(UserType == 1)
                                {//普通用户不需要设置 数字密码  直接提示用户添加成功；
                                        AppMenuVair.submenuTag = 3;                                         
                                        for(i=0;i<4;i++)
                                        {
                                                NoVari.rfidData[i] = GetRfidDat[i];
                                        }        
                                        ParaSaveNormalVair(normalNum,NoVari);
                                }
                                else
                                {//管理员需要添加用户密码
                                    AppMenuVair.submenuTag = 2;
                                }
                                AppMenuVair.Refresh = 1;
                                MT_TFTLCD_Showstring(20,50,"             ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                                MT_TFTLCD_Showstring(20,70,"             ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                                MT_TFTLCD_Showstring(10,60,"RFID ID:    ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                                App_Menu_DisPlayRFIDNum(76,60,GetRfidDat);
                                errorDisPlayDelayTim = 2000;                                
                        }
                        else
                        {///RFID 已经被学习。需要更换新的RFID 卡片
                                MT_TFTLCD_Showstring(20,50,"have been Study",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                                MT_TFTLCD_Showstring(20,70,"    Error...   ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                                errorDisPlayDelayTim = 1000;                
                        }        
                }
        }
        AppMenuVair.TouchKeyNum = MT_GetKeyState();
        if(AppMenuVair.TouchKeyNum == 0xff) return; // 无按键则返回
        OverDleyTim = MENU_WAIT_OVER_TIME;
        pdVal = 0xff; // 初始化为无效值
        switch(AppMenuVair.TouchKeyNum)
        {
                case BackLight_NUM1:
                {
                        pdVal = 1;
                }                        
                break;
                case BackLight_NUM2:
                {
                        pdVal = 2;
                }                        
                break;
                case BackLight_NUM3:
                {
                        pdVal = 3;
                }                        
                break;        
                case BackLight_NUM4:
                {
                        pdVal = 4;
                }                        
                break;                        
                case BackLight_NUM5:        
                {
                        pdVal = 5;
                }
                break;
                case BackLight_NUM6:
                {
                        pdVal = 6;
                }                        
                break;
                case BackLight_NUM7:
                {
                        pdVal = 7;
                }                        
                break;                        
                case BackLight_NUM8:
                {
                        pdVal = 8;
                }                        
                break;
                case BackLight_NUM9:
                {
                        pdVal = 9;
                }                        
                break;                
                case BackLight_NUM0:
                {
                        pdVal = 0;
                }                        
                break;                
                case BackLight_Ring:                        
                break;
                case BackLight_UP:
                {
                        pdVal = 11;
                }        
                break;
                case BackLight_DOWN:
                {
                        pdVal = 12;
                }        
                break;
                case BackLight_Delete:
                {
                        if((AppMenuVair.CursorPos > 0) &&(AppMenuVair.submenuTag == 2))
                        {
                                AppMenuVair.CursorPos --;
                                passWord[AppMenuVair.CursorPos] = 0xff;
                                MT_TFTLCD_Showstring(43+(25*AppMenuVair.CursorPos),68," ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);        
                        }
                }
                break;

                case BackLight_BACK:
                {//返回主菜单
                        AppMenuVair.submenuTag = 4;
                        AppMenuVair.Refresh = 1;
                }
                break;                
                case BackLight_ENTER:///goto password
                {
                        switch(AppMenuVair.submenuTag)
                        {
                                case 0:
                                {        
                                   if((AppMenuVair.CursorPos == 0) && (adminNum == 0xff))//adminNum为0XFF则表示用户已满
                                   {
                                      return;
                                   }
                                   else if((AppMenuVair.CursorPos == 1) && (normalNum == 0xff))
                                   {
                                      return;
                                   }
                                   UserType = AppMenuVair.CursorPos;//添加的用户类型，0:Admin, 1:Normal
                                   AppMenuVair.submenuTag = 1;
                                   AppMenuVair.Refresh = 1;
                                }
                                break;
                                case 2:
                                {
                                        if(AppMenuVair.CursorPos == 4)
                                        {///增加保存数据界面
                                                AppMenuVair.submenuTag = 3;
                                                AppMenuVair.Refresh = 1;                                                
                                        }                
                                }
                                break;                        
                        }                        
                }        
                break;                         
        }
        if((pdVal < 10) && (AppMenuVair.submenuTag == 2))
        {//配置用户密码
                if(AppMenuVair.CursorPos < SYSTEM_PASSWORD_BIT)
                {
                        passWord[AppMenuVair.CursorPos] = pdVal;        
                        MT_TFTLCD_ShowChar(43+(25*AppMenuVair.CursorPos),68,(0x30+pdVal),MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
                        AppMenuVair.CursorPos ++;
                }
        }
        if(((pdVal == 11) || (pdVal == 12)) && (AppMenuVair.submenuTag == 0))
        {
                //选择用户类型    管理员&普通用户
                if(AppMenuVair.CursorPos == 0)        
                {
                        AppMenuVair.CursorPos = 1;        
                }
                else
                {
                        AppMenuVair.CursorPos = 0;
                }
                MT_TFTLCD_Showstring(20,50," ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                MT_TFTLCD_Showstring(20,70," ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                MT_TFTLCD_Showstring(20,50+AppMenuVair.CursorPos*20,">",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
        }
}

/**
 * @brief  管理用户信息菜单处理函数
 */
static void App_Menu_ManageUserInfo(void)
{
    static unsigned char xdata Refresh,Refresh1; // UI刷新标志 (Refresh:静态模板, Refresh1:动态数据)
    static unsigned char xdata RefreshNumId;     // 当前显示的用户在列表中的序号 (从1开始)
    static unsigned char xdata adminNum,normalNum; // 可用的空闲用户ID
    static unsigned char xdata HaveAddAdiminNum,HaveAddNormalNum; // 已添加的管理员和普通用户数量
    static unsigned char xdata dislayUserId;     // 当前显示的用户在全局数组中的实际索引
    unsigned char xdata GetRfidDat[4]; // 用于存储获取到的RFID卡号

    if(AppMenuVair.UiDisplay == TFTLCD_UI_INIT)
        {
                AppMenuVair.UiDisplay = TFTLCD_UI_NULL; // 清除UI刷新标志
                MT_TFTLCD_Showstring(25,0, "USER INFO",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);  
                AppMenuVair.CursorPos = 0;
                // 从参数模块获取当前已存在的用户总数
                HaveAddAdiminNum = ParaGetSumNumber(EN_PARATYPE_ADMINUSER,&adminNum);
                HaveAddNormalNum = ParaGetSumNumber(EN_PARATYPE_NORMALUSER,&normalNum);
                if((HaveAddAdiminNum == 0) && (HaveAddNormalNum == 0))
                {
                   Refresh = 0;
                   Refresh1 = 0;
                   // 如果没有任何用户，则显示提示信息并直接返回
                   MT_TFTLCD_Showstring(10,44,"No user  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
                   AppMenuVair.submenuTag = 2; // 设置为“无用户”状态，锁定大部分按键
                   return;
                }
                AppMenuVair.DisplayTim = 0; // 清除删除成功后的延时计时器
                // 如果存在用户，则初始化所有状态变量，准备显示第一个用户
                RefreshNumId = 1; // 从列表的第一个开始显示，显示第一个用户
                Refresh = 1;      // 触发静态UI模板刷新
                Refresh1 = 1;     // 触发动态用户数据刷新
                AppMenuVair.submenuTag = 0; // 设置为正常的浏览状态
                // 调用辅助函数，通过列表序号(1)获取该用户在全局数组中的实际索引
                dislayUserId = ParaUserInfo(RefreshNumId);
        }
        if(AppMenuVair.DisplayTim) // 处理删除成功后的提示消息
        {
                AppMenuVair.DisplayTim--; // 倒计时
                if(AppMenuVair.DisplayTim == 0) // 倒计时结束
                {
									MT_TFTLCD_Showstring(25,0, "          ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
									MT_TFTLCD_Showstring(10,44,"           ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
                  Refresh = 1;
                  Refresh1 = 1;
                  AppMenuVair.UiDisplay = TFTLCD_UI_INIT;
                }
                return; // 在消息显示期间，不处理其他逻辑
        }
        if(MT_PWMB_GetRfidData(GetRfidDat)) // 轮询RFID刷卡事件
        {
                dislayUserId = ParaCheckRfidData(GetRfidDat); // 检查卡是否存在并获取其在全局数组的索引
                if(dislayUserId != 0xff) // 如果卡存在
                {
                        Refresh1 = 1; // 触发动态数据刷新
                        // 通过卡号反查它在列表中的序号，用于更新 "1/N" 的显示
                        RefreshNumId = ParaCompareRfidId(GetRfidDat);
                }
        }
        // --- 3. UI刷新逻辑 ---
        if(Refresh) // 刷新静态模板 (只在需要时执行)
        {
                Refresh = 0; // 清除刷新标志
                MT_TFTLCD_Showstring(10,24,"ID:         ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
                MT_TFTLCD_Showstring(10,44,"Type:       ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
                MT_TFTLCD_Showstring(10,64,"Password:   ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
                MT_TFTLCD_Showstring(10,84,"RFID:       ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
                MT_TFTLCD_Showstring(10,104,"> Delete User?",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
        }
        if(Refresh1) // 刷新动态用户数据 (只在需要时执行)
        {
                Refresh1 = 0; // 清除刷新标志
                
               //显示用户ID，
                if(dislayUserId <  MAX_ADMIN_USER_NUMBER)
                {                        
                        AppMenuVair.DisplayDat[0] = '0' + PARA_GET_PARA_ADMIN_ID(dislayUserId) / 10;
                        AppMenuVair.DisplayDat[1] = '0' + PARA_GET_PARA_ADMIN_ID(dislayUserId) % 10;
                        AppMenuVair.DisplayDat[2] = 0;                
                }
                else
                {
                        AppMenuVair.DisplayDat[0] = '0' + PARA_GET_PARA_NORMAL_ID(dislayUserId-MAX_ADMIN_USER_NUMBER) / 10;
                        AppMenuVair.DisplayDat[1] = '0' + PARA_GET_PARA_NORMAL_ID(dislayUserId-MAX_ADMIN_USER_NUMBER) % 10;
                        AppMenuVair.DisplayDat[2] = 0;
                }
                MT_TFTLCD_Showstring(60,24,AppMenuVair.DisplayDat,MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
                //显示当前用户在列表中的序号
                AppMenuVair.DisplayDat[0] = '0' + RefreshNumId / 10;
                AppMenuVair.DisplayDat[1] = '0' + RefreshNumId % 10;
                AppMenuVair.DisplayDat[2] = '/';        
                AppMenuVair.DisplayDat[3] = '0' + (HaveAddAdiminNum + HaveAddNormalNum)/ 10;
                AppMenuVair.DisplayDat[4] = '0' + (HaveAddAdiminNum + HaveAddNormalNum) % 10;                
                AppMenuVair.DisplayDat[5] = 0;                
                MT_TFTLCD_Showstring(110,24,AppMenuVair.DisplayDat,MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
                //显示用户类型
                if(dislayUserId < MAX_ADMIN_USER_NUMBER)
                {
                        MT_TFTLCD_Showstring(58,44,"Adminstrator",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                }
                else
                {    
                        MT_TFTLCD_Showstring(58,44,"User        ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
                }
                //=========================显示用户密码
                if(dislayUserId < MAX_ADMIN_USER_NUMBER)
                {
                        AppMenuVair.DisplayDat[0] = '0' + PARA_GET_PARA_ADMIN_PASSWORD(dislayUserId,0) % 10;
                        AppMenuVair.DisplayDat[1] = '0' + PARA_GET_PARA_ADMIN_PASSWORD(dislayUserId,1) % 10;
                        AppMenuVair.DisplayDat[2] = '0' + PARA_GET_PARA_ADMIN_PASSWORD(dislayUserId,2) % 10;
                        AppMenuVair.DisplayDat[3] = '0' + PARA_GET_PARA_ADMIN_PASSWORD(dislayUserId,3) % 10;
                        AppMenuVair.DisplayDat[4] = 0;
                        MT_TFTLCD_Showstring(80,64,AppMenuVair.DisplayDat,MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0); 
                }
                else
                {
                        MT_TFTLCD_Showstring(80,64,"----",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0); 
                }
                //显示用户RFID 
                if(dislayUserId < MAX_ADMIN_USER_NUMBER)
                {
                        GetRfidDat[0] = PARA_GET_PARA_ADMIN_RFIDATA(dislayUserId,0);
                        GetRfidDat[1] = PARA_GET_PARA_ADMIN_RFIDATA(dislayUserId,1); 
                        GetRfidDat[2] = PARA_GET_PARA_ADMIN_RFIDATA(dislayUserId,2); 
                        GetRfidDat[3] = PARA_GET_PARA_ADMIN_RFIDATA(dislayUserId,3);                 
                }
                else
                {
                        GetRfidDat[0] = PARA_GET_PARA_NORMAL_RFIDATA((dislayUserId-MAX_ADMIN_USER_NUMBER),0);
                        GetRfidDat[1] = PARA_GET_PARA_NORMAL_RFIDATA((dislayUserId-MAX_ADMIN_USER_NUMBER),1);
                        GetRfidDat[2] = PARA_GET_PARA_NORMAL_RFIDATA((dislayUserId-MAX_ADMIN_USER_NUMBER),2);
                        GetRfidDat[3] = PARA_GET_PARA_NORMAL_RFIDATA((dislayUserId-MAX_ADMIN_USER_NUMBER),3);
                }
                App_Menu_DisPlayRFIDNum(55,84,GetRfidDat);        
        }
        AppMenuVair.TouchKeyNum = MT_GetKeyState();
        if(AppMenuVair.TouchKeyNum == 0xff) 
        return; // 如果没有按键，直接返回
        AppMenuVair.pdVal = 0xff; // 初始化功能码为无效值
        // 将按键枚举转换为内部功能码
        switch(AppMenuVair.TouchKeyNum)
        {                
                case BackLight_UP: AppMenuVair.pdVal = 11; break; // 向上
                case BackLight_DOWN: AppMenuVair.pdVal = 12; break; // 向下
                case BackLight_Delete: AppMenuVair.pdVal = 13; break; // 删除
                case BackLight_BACK: AppMenuVair.pdVal = 14; break; // 返回
                case BackLight_ENTER: AppMenuVair.pdVal = 15; break; // 确认
        }
        // 根据当前的子流程状态(submenuTag)执行不同的操作
        switch(AppMenuVair.submenuTag)
        {
                case 0: // 状态0: 正常浏览用户界面
                {
                        if(AppMenuVair.pdVal == 11) // 向上按键
                        {
                                RefreshNumId--; // 列表序号减1
                                if(RefreshNumId == 0) RefreshNumId = (HaveAddAdiminNum + HaveAddNormalNum); // 循环到末尾
                                Refresh1 = 1; // 触发数据刷新
                                dislayUserId = ParaUserInfo(RefreshNumId); // 获取新的用户索引
                        }
                        if(AppMenuVair.pdVal == 12) // 向下按键
                        {
                                RefreshNumId++; // 列表序号加1
                                if(RefreshNumId > (HaveAddAdiminNum + HaveAddNormalNum)) RefreshNumId = 1; // 循环到开头
                                dislayUserId = ParaUserInfo(RefreshNumId);
                                Refresh1 = 1;
                        }
                        if(AppMenuVair.pdVal == 13) // 删除按键
                        {
                                AppMenuVair.submenuTag = 1; // 进入“确认删除”状态
                                // 清理屏幕并显示二次确认提示
                                MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
                                MT_TFTLCD_Showstring(10,64,"Want to delete?",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
                        }
                        if(AppMenuVair.pdVal == 14) // 返回按键
                        {
                          // 返回主菜单
													MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
                          App_Task_MenuToggle(MENU_MAINMENU,TFTLCD_UI_RECORER);
                        }
                }
                break;
                case 1: // 状态1: 确认删除界面
                        if(AppMenuVair.pdVal == 14) // 返回按键 (取消删除)
                        {
                           MT_TFTLCD_Showstring(10,64,"                 ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //选择增加的用户类型        
                           AppMenuVair.submenuTag = 0; // 返回浏览状态
                           Refresh1 = 1; // 触发数据刷新
                           Refresh = 1;  // 触发模板刷新
                        }
                        if(AppMenuVair.pdVal == 15) // 确认按键 (执行删除)
                        {
                           AppMenuVair.submenuTag = 0;
                           Refresh1 = 1;
                           Refresh = 1;        
                           AppMenuVair.DisplayTim = 1000;
                           MT_TFTLCD_Showstring(10,64,"                 ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //选择增加的用户类型        
                           MT_TFTLCD_Showstring(10,44,"Update..",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);  //  
                           // 将对应用户的mark标志位置为0，使其在逻辑上被删除
                           if(dislayUserId < MAX_ADMIN_USER_NUMBER) 
                           {
                              PARA_SET_PARA_ADMIN_MARK(dislayUserId,0);
                           }
                           else
                           {
                              PARA_SET_PARA_NORMA_MARK((dislayUserId-MAX_ADMIN_USER_NUMBER),0);
                           }
                           ParaSaveAllVair(); // 将更新后的所有参数重新写入EEPROM
                        }
                break;
                case 2: // 状态2: 无用户界面
                        if(AppMenuVair.pdVal == 14) // 只响应返回键
                        {
                                App_Task_MenuToggle(MENU_MAINMENU,TFTLCD_UI_RECORER);
                        }
                break;
        }

}

/**
 * @brief  系统设置菜单处理函数
 */
static void App_Menu_SystemSet(void)
{
		static unsigned char xdata OperStep,Refresh; // OperStep: 子菜单状态机, Refresh: UI刷新标志
		static unsigned int xdata DisPlayDelayTim = 0; // 用于“Update..”等消息的显示计时器

						// --- 临时变量定义 ---
		unsigned char pdVal = 0xff; // 存储按键转换后的功能码
		if(AppMenuVair.UiDisplay == TFTLCD_UI_INIT)
		{
			AppMenuVair.UiDisplay = TFTLCD_UI_NULL; // 清除UI刷新标志
			// 绘制菜单标题和所有菜单项
			MT_TFTLCD_Showstring(40,0, "SETTING  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
			AppMenuVair.CursorPos = 0; // 主菜单光标默认在第一项
			DisPlayDelayTim = 0;       // 清除计时器
			OperStep = 0;              // 初始化子状态机为0 (主菜单)
			Refresh = 1;               // 触发UI刷新
		}
		if(Refresh)
		{
			Refresh = 0; // 清除刷新标志
			MT_TFTLCD_Showstring(10,24,"  Factory Reset",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
			MT_TFTLCD_Showstring(10,44,"  WG Output    ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
			MT_TFTLCD_Showstring(10,64,"  ModBus Output",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
			// 根据主光标位置绘制 ">"
			MT_TFTLCD_Showstring(10,(24+AppMenuVair.CursorPos*20),"> ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
		}
		if(DisPlayDelayTim)
		{
			DisPlayDelayTim--; // 倒计时
			if(DisPlayDelayTim == 0) // 倒计时结束
			{
				 OperStep = 0; // 返回主菜单状态
				 Refresh = 1;  // 触发主菜单UI刷新
				 // 清除屏幕上的临时消息
				 MT_TFTLCD_Showstring(10,48,"                ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
			}
			return; // 在消息显示期间，不处理其他逻辑
		}
		AppMenuVair.TouchKeyNum = MT_GetKeyState();
		if(AppMenuVair.TouchKeyNum == 0xff)
		return;
		pdVal=0xff;
		switch(AppMenuVair.TouchKeyNum)
		{
			case BackLight_UP:
			{
							pdVal = 11;
			}
			break;
			case BackLight_DOWN:
			{
							pdVal = 12;
			}
			break;
			case BackLight_BACK:
							pdVal = 13;

			break;
			case BackLight_ENTER:///goto password
							pdVal = 14;
			break;                             
		}
		switch(OperStep)
		{
				// --- 状态0: 浏览主设置菜单 ---
		 case 0:
		 {
				if(pdVal == 11) // 向上键
				{
					 if(AppMenuVair.CursorPos == 0) 
							AppMenuVair.CursorPos = 3;
					 AppMenuVair.CursorPos--;
				}
				if(pdVal == 12) // 向下键
				{
					 AppMenuVair.CursorPos++;
					 if(AppMenuVair.CursorPos == 3) 
							AppMenuVair.CursorPos = 0;
				}
				if((pdVal == 13)||(pdVal == 14)) // 返回键或确认键
				{
																	// 清理主菜单UI
					 MT_TFTLCD_Showstring(10,24,"                 ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
					 MT_TFTLCD_Showstring(10,48,"                 ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
					 MT_TFTLCD_Showstring(10,72,"                 ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);

					 if(pdVal == 13) // 如果是返回键
					 {
							App_Task_MenuToggle(MENU_MAINMENU,TFTLCD_UI_RECORER);
					 }
					 else // 如果是确认键
					 {
							OperStep = 1; // 进入子菜单状态
							// 根据主光标位置，绘制不同的子菜单UI
							if(AppMenuVair.CursorPos == 0) // 恢复出厂设置
							{
								 MT_TFTLCD_Showstring(10,48," Restore Settings?",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
							}
							else if(AppMenuVair.CursorPos == 1) // WG 输出设置
							{
								 MT_TFTLCD_Showstring(10,24," WG Ouput?    ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
								 MT_TFTLCD_Showstring(10,48,"   Open         ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
								 MT_TFTLCD_Showstring(10,72,"   Close        ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
									// 根据当前系统参数，初始化子菜单光标位置
								 if(PARA_GET_PARA_SYSTEM_WG_SWITCH_STA)
								 {
										 AppMenuVair.CursorPos2 = 1;
								 }
								 else
								 {
										 AppMenuVair.CursorPos2 = 0;                                                                        
								 }                                                
								 MT_TFTLCD_Showstring(10,(48+AppMenuVair.CursorPos2*20),"> ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
							}
							else if(AppMenuVair.CursorPos == 2) // ModBus 输出设置
							{
									MT_TFTLCD_Showstring(10,24,"ModBus Output?",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //选择增加的用户类型        
									MT_TFTLCD_Showstring(10,48,"  Open       ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
									MT_TFTLCD_Showstring(10,72,"  Close      ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
															
									if(PARA_GET_PARA_SYSTEM_RS485_SWITCH_STA)
									{
										 AppMenuVair.CursorPos2 = 1;
									}
									else
									{
										 AppMenuVair.CursorPos2 = 0;                                                                        
									}
										 MT_TFTLCD_Showstring(10,(48+AppMenuVair.CursorPos2*20),"> ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  // 
							 }
							}
											 return; // 提前返回，避免执行后续的光标重绘
						}
						// 如果按了上/下键，则重绘主菜单光标
						if(pdVal != 0xff)
						{
							 MT_TFTLCD_Showstring(10,24,"  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  // 
							 MT_TFTLCD_Showstring(10,44,"  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
							 MT_TFTLCD_Showstring(10,64,"  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
							 MT_TFTLCD_Showstring(10,84,"  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
							 MT_TFTLCD_Showstring(10,104,"  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //        
							 MT_TFTLCD_Showstring(10,(24+AppMenuVair.CursorPos*20),"> ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  // 
						}
					}
					break;
					// --- 状态1: 在子菜单中进行设置 ---
					case 1:
					switch(AppMenuVair.CursorPos)
					{
						case 0:
						 if((pdVal == 13)||(pdVal == 14))
						 {
								MT_TFTLCD_Showstring(10,24,"            ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);  //选择增加的用户类型
								MT_TFTLCD_Showstring(10,48,"            ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);  //
								MT_TFTLCD_Showstring(10,72,"            ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);  //        
								if(pdVal == 13)
								{///返回设置菜单界面
										OperStep = 0;
										Refresh = 1;                                                        
								}
								else if(pdVal == 14)
								{        //恢复出厂设置        
									 MT_TFTLCD_Showstring(10,48,"Update..",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);  //
									 ParaToHistory();
									 DisPlayDelayTim= 1000;                                                
								}
							}        
						 break;
						 case 1:
						 case 2:
						 {
							if(pdVal == 13)
							{///返回设置菜单界面
								 MT_TFTLCD_Showstring(10,48,"             ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
								 MT_TFTLCD_Showstring(10,72,"             ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //                
								 OperStep = 0;
								 Refresh = 1;                                                        
							}
							else if(pdVal == 14)
							{        //保存数据        
								 MT_TFTLCD_Showstring(10,48,"             ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
								 MT_TFTLCD_Showstring(10,72,"             ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
								 MT_TFTLCD_Showstring(10,48,"Update..",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);  //
								 if(AppMenuVair.CursorPos == 1)
								 {
									 PARA_SET_PARA_SYSTEM_WG_SWITCH_STA(AppMenuVair.CursorPos2);
								 }
								 else if(AppMenuVair.CursorPos == 2)
								 {
									 PARA_SET_PARA_SYSTEM_RS485_SWITCH_STA(AppMenuVair.CursorPos2);
								 }
								 ParaSaveAllVair();
								 DisPlayDelayTim = 1000;                                                
							 }		
							 if((pdVal == 11) || (pdVal == 12))
							 {                
									if(AppMenuVair.CursorPos2 == 0)
									{
										AppMenuVair.CursorPos2 = 1;
									}
									else
									AppMenuVair.CursorPos2 = 0;
									MT_TFTLCD_Showstring(10,48," ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);  //
									MT_TFTLCD_Showstring(10,72," ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);  //                                                                
									MT_TFTLCD_Showstring(10,(48+AppMenuVair.CursorPos2*20),">",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //        
								}
							}
								break;
					}
					break;
		}
}

/**
 * @brief  历史记录菜单处理函数
 */
static void App_Menu_History(void)
{
	static unsigned char historyTotality;    // 存储已保存的历史记录总数
  static unsigned char historyCurrentID;   // 当前正在查看的记录的逻辑序号 (有效值从1开始)

  unsigned char idx;//
	if(AppMenuVair.UiDisplay == TFTLCD_UI_INIT)
		{
			AppMenuVair.UiDisplay = TFTLCD_UI_NULL; // 清除UI刷新标志
			// 绘制菜单标题和所有菜单项
			MT_TFTLCD_Showstring(40,0, "HISTORY",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,24,0);
			 // 从参数模块获取当前已保存的历史记录总数
			historyTotality = PARA_GET_PARA_HISTORY_SUM;
			if(historyTotality == 0)
			{
				// 如果没有任何记录，则显示提示信息并直接返回
				MT_TFTLCD_Showstring(10,44,"No Record  ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
				return;
			}

			// 如果有记录，则绘制UI静态文本标签
			MT_TFTLCD_Showstring(10,24,"ID:           ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
			MT_TFTLCD_Showstring(10,44,"USER:         ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
			MT_TFTLCD_Showstring(10,64,"Type:       ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
			MT_TFTLCD_Showstring(10,84,"Date:       ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
			MT_TFTLCD_Showstring(10,104,"Time:       ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);

			// 初始化状态，准备显示第一条记录
			historyCurrentID = 1; // 从逻辑上的第一条记录开始看
			AppMenuVair.Refresh = 1; // 触发UI刷新
		}
		//UI刷新界面
		if(AppMenuVair.Refresh)
    {
			AppMenuVair.Refresh = 0;
			idx = ParaGetHistoryVari(historyCurrentID);        
			//=========================显示历史记录的序号
			AppMenuVair.DisplayDat[0] = '0' + historyCurrentID / 10;
			AppMenuVair.DisplayDat[1] = '0' + historyCurrentID % 10;                
			AppMenuVair.DisplayDat[2] = '/';                
			AppMenuVair.DisplayDat[3] = '0' + historyTotality / 10;
			AppMenuVair.DisplayDat[4] = '0' + historyTotality % 10;
			AppMenuVair.DisplayDat[5] = 0;
			MT_TFTLCD_Showstring(58,24,AppMenuVair.DisplayDat,MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);
			//=========================显示历史记录的序号
			if(PARA_GET_HISTORYVAIR_USERID(idx) == 0)
			{
				AppMenuVair.DisplayDat[0] = 'I';
				AppMenuVair.DisplayDat[1] = 'D';
				AppMenuVair.DisplayDat[2] = ':';
				AppMenuVair.DisplayDat[3] = '-';
				AppMenuVair.DisplayDat[4] = '-';
				AppMenuVair.DisplayDat[5] = 0;        
			}
			else
			{
				AppMenuVair.DisplayDat[0] = 'I';
				AppMenuVair.DisplayDat[1] = 'D';
				AppMenuVair.DisplayDat[2] = ':';
				AppMenuVair.DisplayDat[3] = '0' + PARA_GET_HISTORYVAIR_USERID(idx) / 10;
				AppMenuVair.DisplayDat[4] = '0' + PARA_GET_HISTORYVAIR_USERID(idx) % 10;
				AppMenuVair.DisplayDat[5] = 0;                
			}
			MT_TFTLCD_Showstring(58,44,AppMenuVair.DisplayDat,MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0); 
			//=========================显示报警类型
			if(PARA_GET_HISTORYVAIR_TYPE(idx) == OPER_TYPE_OPEN_PASSWORD)
			{//密码开锁
				MT_TFTLCD_Showstring(58,64,"Open By Code",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
			}
			else if(PARA_GET_HISTORYVAIR_TYPE(idx) == OPER_TYPE_OPEN_RFID)
			{//RFID开锁
				MT_TFTLCD_Showstring(58,64,"Open By RFID",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
			}
			else if(PARA_GET_HISTORYVAIR_TYPE(idx) == OPER_TYPE_OPEN_KEY)
			{//内部按键
				MT_TFTLCD_Showstring(58,64,"Open By Key ",MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);  //
			}
			//=========================显示日期
			AppMenuVair.DisplayDat[0] = '0' + PARA_GET_HISTORYVAIR_YEAR(idx) / 16;
			AppMenuVair.DisplayDat[1] = '0' + PARA_GET_HISTORYVAIR_YEAR(idx) % 16;                
			AppMenuVair.DisplayDat[2] = '/';                        
			AppMenuVair.DisplayDat[3] = '0' + PARA_GET_HISTORYVAIR_MONTH(idx) / 16;
			AppMenuVair.DisplayDat[4] = '0' + PARA_GET_HISTORYVAIR_MONTH(idx) % 16;                
			AppMenuVair.DisplayDat[5] = '/';                
			AppMenuVair.DisplayDat[6] = '0' + PARA_GET_HISTORYVAIR_DAY(idx) / 16;
			AppMenuVair.DisplayDat[7] = '0' + PARA_GET_HISTORYVAIR_DAY(idx) % 16;
			AppMenuVair.DisplayDat[8] = 0;
			MT_TFTLCD_Showstring(58,84,AppMenuVair.DisplayDat,MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0); 
			//=========================显示时间
			AppMenuVair.DisplayDat[0] = '0' + PARA_GET_HISTORYVAIR_HOUR(idx) / 16;
			AppMenuVair.DisplayDat[1] = '0' + PARA_GET_HISTORYVAIR_HOUR(idx) % 16;                
			AppMenuVair.DisplayDat[2] = ':';                
			AppMenuVair.DisplayDat[3] = '0' + PARA_GET_HISTORYVAIR_MINUTE(idx) / 16;
			AppMenuVair.DisplayDat[4] = '0' + PARA_GET_HISTORYVAIR_MINUTE(idx) % 16;
			AppMenuVair.DisplayDat[5] = ':';                
			AppMenuVair.DisplayDat[6] = '0' + PARA_GET_HISTORYVAIR_SEC(idx)  / 16;
			AppMenuVair.DisplayDat[7] = '0' + PARA_GET_HISTORYVAIR_SEC(idx)  % 16;                
			AppMenuVair.DisplayDat[8] = 0;        
			MT_TFTLCD_Showstring(58,104,AppMenuVair.DisplayDat,MENU_FONT_COLOR,COLOR_TFTLCD_BACKLIGHT,16,0);         
    }
		AppMenuVair.TouchKeyNum = MT_GetKeyState();
		if(AppMenuVair.TouchKeyNum == 0xff) return; // 无按键则返回
    // 将按键枚举转换为内部功能码
    AppMenuVair.pdVal = 0xff;
		switch(AppMenuVair.TouchKeyNum)
		{
			case BackLight_UP: AppMenuVair.pdVal = 11; break;   // 向上
      case BackLight_DOWN: AppMenuVair.pdVal = 12; break; // 向下
			case BackLight_BACK:
			{
				MT_TFTLCD_BColorfill(0,0,LCD_Xend,LCD_Yend,COLOR_TFTLCD_BACKLIGHT);
				App_Task_MenuToggle(MENU_MAINMENU,TFTLCD_UI_RECORER);        
			}
			break;                             
		}
		if(AppMenuVair.pdVal == 11) // 如果是向上翻页
		{
			historyCurrentID--; // 逻辑序号减1
			if(historyCurrentID == 0) // 如果小于1，则循环到最后一条
			{
				historyCurrentID = historyTotality;
			}
			AppMenuVair.Refresh = 1; // 触发UI刷新
		}
		if(AppMenuVair.pdVal == 12) // 如果是向下翻页
		{
			historyCurrentID++; // 逻辑序号加1
			if(historyCurrentID > historyTotality) // 如果大于总数，则循环到第一条
			{
				historyCurrentID = 1;
			}
			AppMenuVair.Refresh = 1; // 触发UI刷新
		}
}
/**
 * @brief  保存开锁的方式
 */
static void App_History_Write(unsigned char id,en_historyType type)
{
    // 1. 定义一个用于存储新记录的临时结构体变量
    str_historyVari write_HistoryVari;
    // 2. 从DS1302芯片获取当前精确的日期和时间
		MT_DS1302_Read_Time();
    // 3. 将所有信息打包到结构体中
    write_HistoryVari.userid = id; // 记录用户ID
    write_HistoryVari.year = TIME_Task.year;   // 记录年
    write_HistoryVari.month = TIME_Task.month; // 记录月
    write_HistoryVari.day = TIME_Task.day;     // 记录日
    write_HistoryVari.hour = TIME_Task.hour;   // 记录时
    write_HistoryVari.minute = TIME_Task.minute; // 记录分
    write_HistoryVari.sec = TIME_Task.second;  // 记录秒
    write_HistoryVari.type = type;                  // 记录操作类型

    // 4. 调用底层的参数保存函数，将这条新记录写入到EEPROM
    ParaSaveHistoryVair(write_HistoryVari);
}