#ifndef _APP_TASK_H
#define _APP_TASK_H
#include "MT_TFTLCD.H"
#define COLOR_TFTLCD_BACKLIGHT    BLACK //背景色        对应的是TFTLCD背景颜色   黑色
#define COLOR_TFTLCD_FOREGROUND   WHITE //前景色        对应的是TFTLCD字体颜色   白色
#define MENU_FONT_COLOR           WHITE //菜单字体颜色   单独定义的 TFTLCD菜单显示字体颜色  和上相同
#define SYSTEM_PASSWORD_BIT       4 //系统密码位数
#define LCD_Xend           				160 //LCD屏幕宽度
#define LCD_Yend           				128 //LCD屏幕高度
#define MENU_WAIT_OVER_TIME   		20000
//#define BackLight_EXIT            0x02 //背光退出
#define MT_Key_Backlighttime      10000
// 定义一个枚举类型，列出所有的菜单界面
typedef enum
{
    MENU_INIT,                // 0: 一个临时的初始状态
    MENU_DESKTOP,             // 1: 待机桌面 (显示时间等)
    MENU_KEY_OPENLOCK,        // 2: 按键开锁界面 (本课程暂未使用)
    MENU_PASSWORD,            // 3: 密码输入界面
    MENU_MAINMENU,            // 4: 主菜单界面
    MENU_SET_TIME,            // 5: 时间日期设置界面
    MENU_AND_USER,            // 6: 添加新用户界面
    MENU_SET_USER,            // 7: 管理用户信息界面
    MENU_SET_SETTING,         // 8: 系统设置界面
    MENU_HISTORY,             // 9: 开门记录查询界面
} En_Menu_Type;
// 定义UI刷新状态的枚举类型
typedef enum
{
    TFTLCD_UI_NULL,                // UI无需操作
    TFTLCD_UI_INIT,                 // UI需要初始化（完全重绘）
    TFTLCD_UI_RECORER,        // UI需要恢复（部分重绘）
} En_TftlcdUi_Type;
// 定义应用程序变量结构体
typedef struct
{
    unsigned char year_Hbit;   // 年-十位
    unsigned char year_Lbit;   // 年-个位
    unsigned char month_Hbit;  // 月-十位
    unsigned char month_Lbit;  // 月-个位
    unsigned char day_Hbit;    // 日-十位
    unsigned char day_Lbit;    // 日-个位
    unsigned char hour_Hbit;   // 时-十位
    unsigned char hour_Lbit;   // 时-个位
    unsigned char minute_Hbit; // 分-十位
    unsigned char minute_Lbit; // 分-个位
    unsigned char sec_Hbit;    // 秒-十位
    unsigned char sec_Lbit;    // 秒-个位
}str_SystemDisplayer;
typedef struct 
{
    // 【核心变量】当前菜单指针，记录程序当前处于哪个界面状态。
        En_Menu_Type MenuPoint;
    
    // UI刷新标志，告诉界面函数是否需要完全重绘(TFTLCD_UI_INIT)还是无需操作(TFTLCD_UI_NULL)。
        En_TftlcdUi_Type UiDisplay; 
    
    // 用于存储最近一次触摸按键的键值，0xff表示无按键。
        unsigned char TouchKeyNum;
    
    // 通用数据值，常用于存储密码输入或某些数值设置。
        unsigned char pdVal;
    
    // 主光标位置，用于菜单项选择或数据显示定位。
        unsigned char CursorPos;
    
    // 副光标位置，用于需要两个光标的复杂界面（如时间设置中的年/月/日切换）。
        unsigned char CursorPos2;
    
    // 通用刷新标志1，用于触发特定UI元素的局部刷新，避免全屏重绘。
        unsigned char Refresh;
    
    // 通用刷新标志2，用于触发其他UI元素的局部刷新。
        unsigned char Refresh2;
    
    // 子菜单标志，用于在同一主菜单状态下区分不同的操作（如在用户管理中区分“添加”、“删除”子操作）。
        unsigned char submenuTag;
    
    // 显示计时器，用于控制提示信息（如“操作成功”）在屏幕上的显示时长。
        unsigned int DisplayTim;
    
    // 显示数据缓冲区，用于临时存储要在屏幕上显示的字符串（如输入的密码、读取的卡号等）。
        unsigned char DisplayDat[18];
} str_AppMenuVair;
// 创建一个全局的结构体实例，供整个程序使用
extern str_AppMenuVair xdata AppMenuVair;
void APP_Task_Init();//初始化函数
void APP_Task();//主任务函数
void App_Task_MenuToggle(En_Menu_Type type, En_TftlcdUi_Type refresh);//菜单跳转函数
#endif