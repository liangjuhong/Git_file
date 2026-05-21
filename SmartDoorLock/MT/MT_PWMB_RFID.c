#include "HAL_PWMB_RFID.H"
#include "MT_PWMB_RFID.H"

MT_RFID_Task xdata RFID_Task_init;
void MT_PWMB_RFID_Init()
{
    RFID_Task_init.state=0;
    RFID_Task_init.rfid_receive_timer=0;
    RFID_Task_init.rfid_receive_flag=0;
}
void MT_PWMB_RFID_Process()
{
    if(Hal_PWMB_GetRfidData(RFID_Task_init.RfidData))
    {
        if(RFID_Task_init.state==0)
        {
            RFID_Task_init.state=1;
        }
        RFID_Task_init.rfid_receive_timer=0;
    }
    if(RFID_Task_init.state)
    {
        RFID_Task_init.rfid_receive_timer++;
        if(RFID_Task_init.rfid_receive_timer==MT_RFID_TIMER_MAX)
        {
            RFID_Task_init.rfid_receive_timer=0;
            RFID_Task_init.state=0;//未接收数据状态
            RFID_Task_init.rfid_receive_flag=0;//上层未接收数据
        }
        if(RFID_Task_init.state==1)
        {
            RFID_Task_init.state=2;//数据接收完成
            RFID_Task_init.rfid_receive_flag=0;//上层未接收数据
        }
    }
}
unsigned char MT_PWMB_GetRfidData(unsigned char *rfidNum)
{
    if(RFID_Task_init.state==2)
    {
        if(RFID_Task_init.rfid_receive_flag==0)
        {
            RFID_Task_init.rfid_receive_flag=1;//上层应用已接收数据状态
            *rfidNum=RFID_Task_init.RfidData[1];
            rfidNum++;
            *rfidNum=RFID_Task_init.RfidData[2];
            rfidNum++;
            *rfidNum=RFID_Task_init.RfidData[3];
            rfidNum++;
            *rfidNum=RFID_Task_init.RfidData[4];
						RFID_Task_init.RfidData[1]=0xff;
						RFID_Task_init.RfidData[2]=0xff;
						RFID_Task_init.RfidData[3]=0xff;
						RFID_Task_init.RfidData[4]=0xff;
            return 1;
        }
    }
    return 0;
}
