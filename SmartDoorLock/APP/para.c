#include "para.h"
#include "MT_IIC_AT24C128.H"
str_AdminUserVair xdata adminUserVair[MAX_ADMIN_USER_NUMBER];  // 管理员用户数组
str_NormalUserVair xdata NormalUserVair[MAX_USER_NUMBER];      // 普通用户数组
str_historyVari xdata historyVari[MAX_HOSTORY_NUMBER];//保存历史记录数组
en_historyManageVari xdata historyManageVari;  //历史记录管理
str_SystemVari xdata SystemVari;                        //系统参数管理

static unsigned char ParaCheck(void);
// 初始化参数
void PartInit(void)
{
    MT_IICReaddata(EEP_ADDR_ADMINUSERPARA_OFFSET,(unsigned char*)(&adminUserVair),sizeof(adminUserVair));        
    MT_IICReaddata(EEP_ADDR_NORMALUSERPARA_OFFSET,(unsigned char*)(&NormalUserVair),sizeof(NormalUserVair));  
    MT_IICReaddata(EEP_ADDR_SYSTEMPARA_OFFSET,(unsigned char*)(&SystemVari),sizeof(SystemVari));
    MT_IICReaddata(EEP_ADDR_HISTORYMANAGEPARA_OFFSET,(unsigned char*)(&historyManageVari),sizeof(historyManageVari));
    MT_IICReaddata(EEP_ADDR_HISTORYPARA_OFFSET,(unsigned char*)(&historyVari),sizeof(historyVari));
    
    if(ParaCheck() == 0)
    {
        ParaToHistory();
        MT_IICReaddata(EEP_ADDR_ADMINUSERPARA_OFFSET,(unsigned char*)(&adminUserVair),sizeof(adminUserVair));        
        MT_IICReaddata(EEP_ADDR_NORMALUSERPARA_OFFSET,(unsigned char*)(&NormalUserVair),sizeof(NormalUserVair));
        MT_IICReaddata(EEP_ADDR_SYSTEMPARA_OFFSET,(unsigned char*)(&SystemVari),sizeof(SystemVari));
        MT_IICReaddata(EEP_ADDR_HISTORYMANAGEPARA_OFFSET,(unsigned char*)(&historyManageVari),sizeof(historyManageVari));
        MT_IICReaddata(EEP_ADDR_HISTORYPARA_OFFSET,(unsigned char*)(&historyVari),sizeof(historyVari));        
    }
}
// 检查是否有无效的数据
static unsigned char ParaCheck(void)
{
     unsigned char xdata i;
     for(i=0;i<MAX_ADMIN_USER_NUMBER;i++)
     {
       if((adminUserVair[i].mark) > 1)
       {
          return 0;
       }
     }        
     for(i=0;i<MAX_USER_NUMBER;i++)
     {
        if((NormalUserVair[i].mark) > 1)
        {
          return 0;
        }
     }        
     return 1;        
}
//恢复出厂设置函数功能
void ParaToHistory(void)
{
     unsigned char xdata i,j;
     for(i=0;i<MAX_ADMIN_USER_NUMBER;i++)
     {
        adminUserVair[i].mark = 0;
        adminUserVair[i].id = 0xff;
        for(j=0;j<4;j++)
        {
           adminUserVair[i].password[j] = 0xff;
           adminUserVair[i].rfidData[j] = 0xff;        
        }
     }        
     for(i=0;i<MAX_USER_NUMBER;i++)
     {
        NormalUserVair[i].mark = 0;
        NormalUserVair[i].id = 0xff;
        for(j=0;j<4;j++)
        {
          NormalUserVair[i].rfidData[j]  = 0xff;        
        }
     }
     historyManageVari.SumNumber = 0;
     historyManageVari.startPointer = 0;
     SystemVari.Switch_Rs485OutPut = SWITCH_OPEN;
     SystemVari.Switch_WGOutPut = SWITCH_OPEN;
     MT_IICWritePage(EEP_ADDR_ADMINUSERPARA_OFFSET,(unsigned char*)(&adminUserVair),sizeof(adminUserVair));        
     MT_IICWritePage(EEP_ADDR_NORMALUSERPARA_OFFSET,(unsigned char*)(&NormalUserVair),sizeof(NormalUserVair));
     MT_IICWritePage(EEP_ADDR_SYSTEMPARA_OFFSET,(unsigned char*)(&SystemVari),sizeof(SystemVari));        
     MT_IICWritePage(EEP_ADDR_HISTORYMANAGEPARA_OFFSET,(unsigned char*)(&historyManageVari),sizeof(historyManageVari));
     MT_IICWritePage(EEP_ADDR_HISTORYPARA_OFFSET,(unsigned char*)(&historyVari),sizeof(historyVari));                        
}
//检查读取的RFID数据是否已经被学习
unsigned char ParaCheckRfidData(unsigned char *Rfid_Num)
{
     unsigned char xdata i;
     for(i=0;i<MAX_ADMIN_USER_NUMBER;i++)
     {
       if((adminUserVair[i].mark == 1) &&
        (adminUserVair[i].rfidData[0] == Rfid_Num[0]) &&
        (adminUserVair[i].rfidData[1] == Rfid_Num[1]) &&
        (adminUserVair[i].rfidData[2] == Rfid_Num[2]) &&
        (adminUserVair[i].rfidData[3] == Rfid_Num[3]))
       {
          return i;  // 返回管理员的索引
       }
     }
     for(i=0;i<MAX_USER_NUMBER;i++)
     {
       if((NormalUserVair[i].mark == 1) &&
         (NormalUserVair[i].rfidData[0] == Rfid_Num[0]) &&
         (NormalUserVair[i].rfidData[1] == Rfid_Num[1]) &&
         (NormalUserVair[i].rfidData[2] == Rfid_Num[2]) &&
         (NormalUserVair[i].rfidData[3] == Rfid_Num[3]))
       {
          return (i + MAX_ADMIN_USER_NUMBER);  // 返回普通用户的索引（加上管理员数量偏移）
       }
     }
     return 0xff;  // 返回0xff表示没有找到匹配的RFID
}
//检查密码数据是否被录入
unsigned char ParaCheckPassWordData(unsigned char *PassWord)
{
        unsigned char xdata i;
        unsigned char xdata markflag = 0;  // 用于标记是否所有管理员标识都无效
        for(i=0;i<MAX_ADMIN_USER_NUMBER;i++)
        {
           if(adminUserVair[i].mark == 1)
           {
              if((adminUserVair[i].password[0] == PassWord[0]) &&
                 (adminUserVair[i].password[1] == PassWord[1]) &&
                 (adminUserVair[i].password[2] == PassWord[2]) &&
                 (adminUserVair[i].password[3] == PassWord[3]))
              {
                return i;  // 返回管理员的索引
              }                
           }
           else
           {
              markflag ++;  // 标记无效的管理员
           }
        }
        if(markflag == MAX_ADMIN_USER_NUMBER)
        {  // 如果所有管理员的标识都无效
            return 0xfe;  // 返回0xfe表示出厂状态
        }
        return 0xff;  // 返回0xff表示没有找到匹配的密码
}

//检测当前的用户ID是否已经被学习
unsigned char ParaUserInfo(unsigned char CurrentId) 
{
        unsigned char xdata i;
        unsigned char xdata HaveAddUserId =0;
        for(i=0;i<MAX_ADMIN_USER_NUMBER;i++)
        {
          if((adminUserVair[i].mark) == 1)
          {        
            HaveAddUserId ++;
          }
          if(HaveAddUserId == CurrentId)
          {
            return i;
          }        
        }        
        for(i=0;i<MAX_USER_NUMBER;i++)
        {
          if((NormalUserVair[i].mark) == 1)
          {        
            HaveAddUserId ++;
          }
          if(HaveAddUserId == CurrentId)
          {
            return (i+MAX_ADMIN_USER_NUMBER);
          }                                
        }
        return 0xff;
}
//判断识别到的RFID数据是否已经保存到了内存中
unsigned char ParaCompareRfidId(unsigned char *Rfid_Num) 
{
        unsigned char xdata i;
        unsigned char xdata HaveAddUserId =0;
        for(i=0;i<MAX_ADMIN_USER_NUMBER;i++)
        {
          if(adminUserVair[i].mark == 1)
          {
            HaveAddUserId ++;
            if((adminUserVair[i].rfidData[0] == Rfid_Num[0]) 
               &&(adminUserVair[i].rfidData[1] == Rfid_Num[1]) 
               &&(adminUserVair[i].rfidData[2] == Rfid_Num[2])                 
               &&(adminUserVair[i].rfidData[3] == Rfid_Num[3]))
             {
                 return HaveAddUserId;        
             }                
          }
        }
        for(i=0;i<MAX_USER_NUMBER;i++)
        {
           if(NormalUserVair[i].mark == 1)
           {
              HaveAddUserId ++;
              if((NormalUserVair[i].rfidData[0] == Rfid_Num[0]) 
              &&(NormalUserVair[i].rfidData[1] == Rfid_Num[1]) 
              &&(NormalUserVair[i].rfidData[2] == Rfid_Num[2])                 
              &&(NormalUserVair[i].rfidData[3] == Rfid_Num[3]))
              {
                return HaveAddUserId;        
              }           
           }
        }
        return 0xff;
}
//获取当前的普通用户或管理员，或历史记录的个数
unsigned char ParaGetSumNumber(unsigned char en_ParaType,unsigned char *getId) 
{
        unsigned char xdata i,resum = 0;
        *getId = 0xff;
        switch(en_ParaType)
        { 
          case EN_PARATYPE_ADMINUSER:
               {
                 for(i=0;i<MAX_ADMIN_USER_NUMBER;i++)
                 {
                   if((adminUserVair[i].mark) == 1)
                   {        
                     resum ++;
                   }
                   else
                   {
                     if(*getId == 0xff)
                       *getId = i;
                   }
                 }
               }
          break;
          case EN_PARATYPE_NORMALUSER:
               {
                  for(i=0;i<MAX_USER_NUMBER;i++)
                  {
                     if((NormalUserVair[i].mark) == 1)
                     {        
                        resum ++;
                     }
                     else
                     {
                        if(*getId == 0xff)
                         *getId = i;
                     }                                
                  }
               }
          break;
          case EN_PARATYPE_HISTORY:
               {
                  for(i=0;i<MAX_HOSTORY_NUMBER;i++)
                  {
                    if((historyVari[i].mark))
                    {        
                       resum ++;
                    }
                    else
                    {
                       if(*getId == 0xff)
                       *getId = i;
                    }                                
                  }
               }
          break;
        }
        return resum;
}
//保存管理员数据
void ParaSaveAdminVair(unsigned char id,str_AdminUserVair vari)
{
        xdata unsigned char i;
        adminUserVair[id].mark = 1;
        adminUserVair[id].id = id+1;
        for(i=0;i<4;i++)
        {
          adminUserVair[id].password[i] = vari.password[i];
          adminUserVair[id].rfidData[i] = vari.rfidData[i];         
        }
        MT_IICWritePage(EEP_ADDR_ADMINUSERPARA_OFFSET,(unsigned char*)(&adminUserVair),sizeof(adminUserVair));        
}
//保存普通用户信息
void ParaSaveNormalVair(unsigned char id,str_NormalUserVair vari)
{
        xdata unsigned char i;
        NormalUserVair[id].mark = 1;
        NormalUserVair[id].id = (id + MAX_ADMIN_USER_NUMBER +1);
        for(i=0;i<4;i++)
        {
          NormalUserVair[id].rfidData[i] = vari.rfidData[i];         
        }
        MT_IICWritePage(EEP_ADDR_NORMALUSERPARA_OFFSET,(unsigned char*)(&NormalUserVair),sizeof(NormalUserVair));        
}
//保存历史记录数据
void ParaSaveHistoryVair(str_historyVari vari)
{
        unsigned char xdata id;
        if(historyManageVari.SumNumber < MAX_HOSTORY_NUMBER)
        {        
           id = historyManageVari.SumNumber;
           historyManageVari.SumNumber ++;        
           historyManageVari.startPointer = 0;
        }
        else
        {//10
           id = historyManageVari.startPointer;
           historyManageVari.startPointer ++;
           if(historyManageVari.startPointer == MAX_HOSTORY_NUMBER)
           {
              historyManageVari.startPointer = 0;
           }
        }
        historyVari[id].mark = 1;        
        historyVari[id].userid = vari.userid;
        historyVari[id].year = vari.year;        
        historyVari[id].month = vari.month;        
        historyVari[id].day = vari.day;
        historyVari[id].hour = vari.hour;
        historyVari[id].minute = vari.minute;        
        historyVari[id].sec = vari.sec;
        historyVari[id].type = vari.type;
        MT_IICWritePage(EEP_ADDR_HISTORYMANAGEPARA_OFFSET,(unsigned char*)(&historyManageVari),sizeof(historyManageVari));
        MT_IICWritePage(EEP_ADDR_HISTORYPARA_OFFSET+id*SIZE_HISTORYVARI_PARA_SIZE,(unsigned char*)(&historyVari[id]),SIZE_HISTORYVARI_PARA_SIZE);        
}
//获取历史记录数据
unsigned char ParaGetHistoryVari(unsigned char NumId)
{
        unsigned char xdata id;  //输出数据的历史记录ID号；
        if((NumId == 0) || (NumId > MAX_HOSTORY_NUMBER))
                return 0XFF;
        if(historyManageVari.SumNumber < MAX_HOSTORY_NUMBER)
        {
                ////如果最大记录是10；  SumNumber->3     0 1 2 3 4 5 6 7 8 9;
           //NumId = 1；->2;
           //NumId = 2；->1;5  2
           //NumId = 3；->0;
           id = historyManageVari.SumNumber - NumId;
        }
        else
        {  ////如果最大记录是10；   P = 2;   0 1 2 3 4 5 6 7 8 9;
           //NumId = 1；->1;
           //NumId = 2；->0;
           //NumId = 3；->9;
           //NumId = 4；->8;
           //NumId = 5；->7;
           //NumId = 6；->6;
           // ......  
            if(historyManageVari.startPointer >= NumId)
            {
               id = historyManageVari.startPointer - NumId;
            }
            else
            {
               id = historyManageVari.startPointer + MAX_HOSTORY_NUMBER - NumId;
            }
        }
        return id;
}
//将数据保存到EEPROM中
void ParaSaveAllVair(void)
{
        MT_IICWritePage(EEP_ADDR_ADMINUSERPARA_OFFSET,(unsigned char*)(&adminUserVair),sizeof(adminUserVair));        
        MT_IICWritePage(EEP_ADDR_NORMALUSERPARA_OFFSET,(unsigned char*)(&NormalUserVair),sizeof(NormalUserVair));
        MT_IICWritePage(EEP_ADDR_SYSTEMPARA_OFFSET,(unsigned char*)(&SystemVari),sizeof(SystemVari));
        MT_IICWritePage(EEP_ADDR_HISTORYMANAGEPARA_OFFSET,(unsigned char*)(&historyManageVari),sizeof(historyManageVari));
        MT_IICWritePage(EEP_ADDR_HISTORYPARA_OFFSET,(unsigned char*)(&historyVari),sizeof(historyVari));        
}