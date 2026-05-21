#ifndef _PARA_
#define _PARA_
#define MAX_ADMIN_USER_NUMBER    5      // 最大管理员用户数量
#define MAX_USER_NUMBER          50     // 最大用户数量
#define SYSTEM_PASSWORD_BIT      4      // 系统密码位数
#define MAX_HOSTORY_NUMBER       99

// 定义管理员用户结构体
typedef struct
{  
    unsigned char mark;           // 用户标识（可用于区分管理员）
    unsigned char id;             // 用户ID
    unsigned char password[4];    // 用户密码（4位）
    unsigned char rfidData[4];    // 用户的RFID数据（4字节）
}str_AdminUserVair;
#define PARA_SET_PARA_ADMIN_MARK(i,var)          (adminUserVair[i].mark = var)  // 设置管理员标识
#define PARA_SET_PARA_ADMIN_ID(i,var)            (adminUserVair[i].id = var)  // 设置管理员标识
#define PARA_SET_PARA_ADMIN_PASSWORD(i,bits,var) (adminUserVair[i].password[bits] = var)  // 设置管理员密码的指定位
#define PARA_SET_PARA_ADMIN_RFIDATA(i,bits,var)  (adminUserVair[i].rfidData[bits] = var)  // 设置管理员RFID数据的指定位

#define PARA_GET_PARA_ADMIN_MARK(i)              (adminUserVair[i].mark)  // 设置管理员标识
#define PARA_GET_PARA_ADMIN_ID(i)                (adminUserVair[i].id)  // 获取管理员ID
#define PARA_GET_PARA_ADMIN_PASSWORD(i,bits)     (adminUserVair[i].password[bits])  // 获取管理员密码的指定位
#define PARA_GET_PARA_ADMIN_RFIDATA(i,bits)      (adminUserVair[i].rfidData[bits])  // 获取管理员RFID数据的指定位
#define SIZE_ADMIN_USER_PARA_SIZE                sizeof(str_AdminUserVair)  // 获取管理员结构体的大小

// 定义普通用户结构体
typedef struct
{
    unsigned char mark;           // 用户标识（可用于区分普通用户）
    unsigned char id;             // 用户ID
    unsigned char rfidData[4];    // 用户的RFID数据（4字节）
}str_NormalUserVair;
#define PARA_SET_PARA_NORMA_MARK(i,var)           (NormalUserVair[i].mark = var)  // 设置普通用户标识
#define PARA_GET_PARA_NORMAL_ID(i)                (NormalUserVair[i].id)  // 获取普通用户ID
#define PARA_GET_PARA_NORMAL_RFIDATA(i,bits)      (NormalUserVair[i].rfidData[bits])  // 获取普通用户RFID数据的指定位
#define SIZE_NORMAL_USER_PARA_SIZE                sizeof(str_NormalUserVair)  // 获取普通用户结构体的大小

// 定义历史开锁类型枚举
typedef enum
{
        OPER_TYPE_OPEN_PASSWORD,
        OPER_TYPE_OPEN_RFID,
        OPER_TYPE_OPEN_KEY,
}en_historyType;
// 定义历史记录结构体
typedef struct
{
        unsigned char mark;
        unsigned char userid;
        unsigned char year;
        unsigned char month;
        unsigned char day;
        unsigned char hour;
        unsigned char minute;
        unsigned char sec;
        en_historyType type;
}str_historyVari;

#define SIZE_HISTORYVARI_PARA_SIZE               sizeof(str_historyVari)
#define PARA_GET_HISTORYVAIR_USERID(i)           (historyVari[i].userid)
#define PARA_GET_HISTORYVAIR_YEAR(i)             (historyVari[i].year)
#define PARA_GET_HISTORYVAIR_DAY(i)              (historyVari[i].day)
#define PARA_GET_HISTORYVAIR_MONTH(i)            (historyVari[i].month)
#define PARA_GET_HISTORYVAIR_HOUR(i)             (historyVari[i].hour)
#define PARA_GET_HISTORYVAIR_MINUTE(i)           (historyVari[i].minute)
#define PARA_GET_HISTORYVAIR_SEC(i)              (historyVari[i].sec)
#define PARA_GET_HISTORYVAIR_TYPE(i)             (historyVari[i].type)

// 定义历史记录管理结构体
typedef struct
{
    unsigned char SumNumber;
    unsigned char startPointer;
}en_historyManageVari;
#define SIZE_HISTORYMANAGE_PARA_SIZE               sizeof(en_historyManageVari)        
#define PARA_GET_PARA_HISTORY_SUM                  (historyManageVari.SumNumber)
#define PARA_GET_PARA_HISTORY_POINTER              (historyManageVari.startPointer)

// 定义系统参数结构体(包含RS485和WG开关状态)
typedef enum
{
        SWITCH_OPEN,
        SWITCH_CLOSE,
}en_SwitchType;
typedef struct
{
    en_SwitchType Switch_Rs485OutPut;
    en_SwitchType Switch_WGOutPut;
}str_SystemVari;
#define SIZE_SYSTEMVARI_PARA_SIZE                  sizeof(str_SystemVari)    
#define PARA_GET_PARA_SYSTEM_RS485_SWITCH_STA      (SystemVari.Switch_Rs485OutPut)
#define PARA_GET_PARA_SYSTEM_WG_SWITCH_STA         (SystemVari.Switch_WGOutPut)
#define PARA_SET_PARA_SYSTEM_RS485_SWITCH_STA(i)   (SystemVari.Switch_Rs485OutPut = i)
#define PARA_SET_PARA_SYSTEM_WG_SWITCH_STA(i)      (SystemVari.Switch_WGOutPut = i)

typedef enum
{
        EN_PARATYPE_ADMINUSER,
        EN_PARATYPE_NORMALUSER,
        EN_PARATYPE_HISTORY,
}en_ParaType;

// 定义EEPROM地址偏移量
#define EEP_ADDR_ADMINUSERPARA_OFFSET      0  
#define EEP_ADDR_NORMALUSERPARA_OFFSET     (SIZE_ADMIN_USER_PARA_SIZE * MAX_ADMIN_USER_NUMBER)
#define EEP_ADDR_SYSTEMPARA_OFFSET         (EEP_ADDR_NORMALUSERPARA_OFFSET+SIZE_NORMAL_USER_PARA_SIZE*MAX_USER_NUMBER)
#define EEP_ADDR_HISTORYMANAGEPARA_OFFSET  (EEP_ADDR_SYSTEMPARA_OFFSET+SIZE_SYSTEMVARI_PARA_SIZE)
#define EEP_ADDR_HISTORYPARA_OFFSET        (EEP_ADDR_HISTORYMANAGEPARA_OFFSET+SIZE_HISTORYMANAGE_PARA_SIZE)

extern str_AdminUserVair xdata adminUserVair[MAX_ADMIN_USER_NUMBER];  //管理员数据数组
extern str_NormalUserVair xdata NormalUserVair[MAX_USER_NUMBER];      //普通用户数据数组
extern str_historyVari xdata historyVari[MAX_HOSTORY_NUMBER];         //历史记录数据数组
extern en_historyManageVari xdata historyManageVari;                  //历史记录管理
extern str_SystemVari xdata SystemVari;                               //系统参数管理

void PartInit(void);
void ParaToHistory(void);//将系统参数写入历史记录
unsigned char ParaCheckRfidData(unsigned char *Rfid_Num);//检查读取的RFID数据是否已经被学习
unsigned char ParaCheckPassWordData(unsigned char *PassWord);//检查密码数据
unsigned char ParaUserInfo(unsigned char CurrentId);//检测当前的用户ID是否已经被学习
unsigned char ParaCompareRfidId(unsigned char *Rfid_Num);//判断识别到的RFID数据是否已经保存到了内存中
unsigned char ParaGetSumNumber(unsigned char en_ParaType,unsigned char *getId);//获取当前的普通用户或管理员，或历史记录的个数
void ParaSaveAdminVair(unsigned char id,str_AdminUserVair vari);//保存管理员数据
void ParaSaveNormalVair(unsigned char id,str_NormalUserVair vari);//保存普通用户数据
void ParaSaveHistoryVair(str_historyVari vari);//保存历史记录数据
unsigned char ParaGetHistoryVari(unsigned char NumId);//获取历史记录数据
void ParaSaveAllVair(void);//将数据保存到EEPROM中
#endif