#include <Servo.h>
#include <MsTimer2.h>
#include <EEPROM.h> 

int ledpin = 13;//设置系统启动指示灯
int ENA = 5;//L298使能A
int ENB = 6;//L298使能B
int INPUT2 = 7;//电机接口1
int INPUT1 = 8;//电机接口2
int INPUT3 = 12;//电机接口3
int INPUT4 = 13;//电机接口4

int Echo = A0;  // 定义超音波信号接收脚位  
int Trig = A1;  // 定义超音波信号发射脚位
int Input_Detect_LEFT = A2;    //定义小车左侧红外
int Input_Detect_RIGHT = A3;  //定义小车右侧红外
int Input_Detect = A4;//定义小车前方红外
int Carled = 2;//定义小车车灯接口
int Cruising_Flag = 0;
int Pre_Cruising_Flag = 0 ;
int Left_Speed_Hold = 200;//定义左侧速度变量
int Right_Speed_Hold = 200;//定义右侧速度变量

#define MOTOR_GO_FORWARD  {digitalWrite(INPUT1,LOW);digitalWrite(INPUT2,HIGH);digitalWrite(INPUT3,LOW);digitalWrite(INPUT4,HIGH);}    //车体前进	                            
#define MOTOR_GO_BACK	  {digitalWrite(INPUT1,HIGH);digitalWrite(INPUT2,LOW);digitalWrite(INPUT3,HIGH);digitalWrite(INPUT4,LOW);}    //车体前进
#define MOTOR_GO_RIGHT	  {digitalWrite(INPUT1,HIGH);digitalWrite(INPUT2,LOW);digitalWrite(INPUT3,LOW);digitalWrite(INPUT4,HIGH);}    //车体前进
#define MOTOR_GO_LEFT	  {digitalWrite(INPUT1,LOW);digitalWrite(INPUT2,HIGH);digitalWrite(INPUT3,HIGH);digitalWrite(INPUT4,LOW);}    //车体前进
#define MOTOR_GO_STOP	  {digitalWrite(INPUT1,LOW);digitalWrite(INPUT2,LOW);digitalWrite(INPUT3,LOW);digitalWrite(INPUT4,LOW);}    //车体前进

int Left_Speed[11]={90,106,122,138,154,170,186,203,218,234,255};//左侧速度档位值
int Right_Speed[11]={90,106,122,138,154,170,186,203,218,234,255};//右侧速度档位值

Servo servo1;// 创建舵机#1号
Servo servo2;// 创建舵机#2号

byte angle1=70;//舵机#1初始值
byte angle2=60;//舵机#2初始值

int buffer[3];  //串口接收数据缓存
int rec_flag;   //串口接收标志位
int serial_data;
int Uartcount;
int IR_R;
int IR_L;
int IR;
unsigned long Pretime;
unsigned long Nowtime;
unsigned long Costtime;
float Ldistance;

void Open_Light()//开大灯
    {      
      digitalWrite(Carled,HIGH);   //拉低电平，正极接电源，负极接Io口
      delay(1000);             
    }
void Close_Light()//关大灯
    {  
      digitalWrite(Carled, LOW);   //拉低电平，正极接电源，负极接Io口
      delay(1000);             
    }
    
void  Avoiding()//红外避障函数
    {  
      IR = digitalRead(Input_Detect);
       if((IR == HIGH))
       {
          MOTOR_GO_FORWARD;//直行 
          return;            
       }
       if((IR == LOW))
       {
            MOTOR_GO_STOP;//停止
            return;
       }
    }
           
   

void FollowLine()   // 巡线模式
    {  
      IR_L = digitalRead(Input_Detect_LEFT);//读取左边传感器数值
      IR_R = digitalRead(Input_Detect_RIGHT);//读取右边传感器数值
      
      if((IR_L == LOW) && (IR_R == LOW))//两边同时探测到障碍物
      {
        MOTOR_GO_FORWARD;//直行 
        return;          
        
      }
      if((IR_L == LOW) && (IR_R == HIGH))//右侧遇到障碍  
      {
        MOTOR_GO_LEFT;//左转 
        return;
        
      }
      if((IR_L == HIGH) &&( IR_R == LOW))//左侧遇到障碍 
      {
        MOTOR_GO_RIGHT;//右转 
        return;
        
      }
      if((IR_L == HIGH) && (IR_R == HIGH))//左右都检测到，就如视频中的那样遇到一道横的胶带
      {
        MOTOR_GO_STOP;//停止
        return;
       }
    }    

char Get_Distence()//测出距离
 {  
      digitalWrite(Trig, LOW);   // 让超声波发射低电压2μs  
      delayMicroseconds(2);  
      digitalWrite(Trig, HIGH);  // 让超声波发射高电压10μs，这里至少是10μs  
      delayMicroseconds(10);  
      digitalWrite(Trig, LOW);    // 维持超声波发射低电压  
      Ldistance = pulseIn(Echo, HIGH);  // 读差相差时间  
      Ldistance= Ldistance/5.8/10;      // 将时间转为距离距离（单位：公分）    
    //  Serial.println(Ldistance);      //显示距离  
      return Ldistance;
      
  }    
  
void Avoid_wave()//超声波避障函数
{
  Get_Distence();
  if(Ldistance < 15)
      {
          MOTOR_GO_STOP;
      }
      else
      {
          MOTOR_GO_FORWARD;
      }
}

void Avoid_wave_auto()
{
  Get_Distence();
  if(Ldistance < 15)
  {
    MOTOR_GO_BACK;
    delay(300);
    MOTOR_GO_STOP;
  }
}
/*
*********************************************************************************************************
** 函数名称 ：Delayed()
** 函数功能 ：延时程序
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************
*/
void  Delayed()    //延迟40秒等WIFI模块启动完毕
{
    int i;
    for(i=0;i<20;i++)
    {
        digitalWrite(ledpin,LOW);
        delay(1000);
        digitalWrite(ledpin,HIGH);
        delay(1000);
    }
}

/*
*********************************************************************************************************
** 函数名称 ：setup().Init_Steer()
** 函数功能 ：系统初始化（串口、电机、舵机、指示灯初始化）。
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************
*/
void Init_Steer()//舵机初始化(角度为上次保存数值)
{
    servo1.write(angle1);
    servo2.write(angle2);
    
    angle1 = EEPROM.read(0x01);//读取寄存器0x01里面的值
    angle2 = EEPROM.read(0x02);//读取寄存器0x02里面的值
    if(angle1 == 255 && angle2 == 255)
    {
        EEPROM.write(0x01,70);//把初始角度存入地址0x01里面
        EEPROM.write(0x02,60);//把初始角度存入地址0x02里面
        return;
    }
 
    servo1.write(angle1);//把保存角度赋值给舵机1
    servo2.write(angle2);//把保存角度赋值给舵机2
}

void setup()
{
    pinMode(ledpin,OUTPUT); 
    pinMode(ENA,OUTPUT); 
    pinMode(ENB,OUTPUT); 
    pinMode(INPUT1,OUTPUT); 
    pinMode(INPUT2,OUTPUT); 
    pinMode(INPUT3,OUTPUT); 
    pinMode(INPUT4,OUTPUT); 
    pinMode(Input_Detect_LEFT,INPUT);
    pinMode(Input_Detect_RIGHT,INPUT);
    pinMode(Carled, OUTPUT);
    pinMode(Input_Detect,INPUT);
    pinMode(Echo,INPUT);
    pinMode(Trig,OUTPUT);

    Delayed();//延迟40秒等WIFI模块启动完毕
    analogWrite(ENB,Left_Speed_Hold);//给L298使能端B赋值
    analogWrite(ENA,Right_Speed_Hold);//给L298使能端A赋值
    digitalWrite(ledpin,LOW);
    servo1.attach(9);//定义舵机7控制口
    servo2.attach(10);//定义舵机8控制口
    Serial.begin(9600);//串口波特率设置为9600 bps
    Init_Steer();
}
/*
*********************************************************************************************************
** 函数名称 ：loop()
** 函数功能 ：主函数
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************
*/
void Cruising_Mod()//模式功能切换函数
    {
       
	 if(Pre_Cruising_Flag != Cruising_Flag)
	 {
	     if(Pre_Cruising_Flag != 0)
		 {
		     MOTOR_GO_STOP; 
		 }

    	 Pre_Cruising_Flag =  Cruising_Flag;
	 }	
	switch(Cruising_Flag)
	  {
	   //case 1:Send_Distance();//超声波测距
	   case 2:FollowLine(); return;//巡线模式
	   case 3:Avoiding(); return;//避障模式
	   case 4:Avoid_wave();return;//超声波避障模式
	   default:return;
	  }
        	 
}

void loop()
  {  
    while(1)
    {
        Get_uartdata();
        UartTimeoutCheck();
        Cruising_Mod();
     }  
  }



/*
*********************************************************************************************************
** 函数名称 ：Communication_Decode()
** 函数功能 ：串口命令解码
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************
*/
void Communication_Decode()
{   
    if(buffer[0]==0x00)
    {
        switch(buffer[1])   //电机命令
        {
            case 0x01:MOTOR_GO_FORWARD; return;
	    case 0x02:MOTOR_GO_BACK;    return;
	    case 0x03:MOTOR_GO_LEFT;    return;
            case 0x04:MOTOR_GO_RIGHT;   return;
	    case 0x00:MOTOR_GO_STOP;    return;
           default: return;
        }	
    }
   else if(buffer[0]==0x01)//舵机命令
    {
        switch(buffer[1])
        {
            case 0x07:if(buffer[2]>160)return;angle1 = buffer[2];servo1.write(angle1);return;
            case 0x08:if(buffer[2]>90)return;angle2 = buffer[2];servo2.write(angle2);return;
            default:return;
        }
    }
    
   else if(buffer[0]==0x02)//调速
	{
                int i,j;
		if(buffer[2]>10)return;
             
		if(buffer[1]==0x01)//左侧调档
		{
                        i=buffer[2];
			Left_Speed_Hold=Left_Speed[i] ;
                        analogWrite(ENB,Left_Speed_Hold);
		}
                if(buffer[1]==0x02)//右侧调档
                {
                        j=buffer[2];
                        Right_Speed_Hold=Right_Speed[j] ;
                        analogWrite(ENA,Right_Speed_Hold);
                }else return;
        }
    else if(buffer[0]==0x33)//读取舵机角度并赋值
	{
		 Init_Steer();return;
        }
    else if(buffer[0]==0x32)//保存命令
    { 
        EEPROM.write(0x01,angle1);
        EEPROM.write(0x02,angle2);
        return;
    }
    	else if(buffer[0]==0x13)//模式切换开关
	{
	    switch(buffer[1])
		{
                  case 0x02: Cruising_Flag = 2; return;//巡线
		  case 0x03: Cruising_Flag = 3; return;//避障
		  case 0x04: Cruising_Flag = 4; return;//雷达避障
                  case 0x00: Cruising_Flag = 0; return;//正常模式
		  default:Cruising_Flag = 0; return;//正常模式
		}
	}
        else if(buffer[0]==0x05)
    {
        switch(buffer[1])   //
        {
            case 0x00:Open_Light(); return;
	    case 0x02:Close_Light(); return;
            default: return;
        }	
    }
    
    
}
/*
*********************************************************************************************************
** 函数名称 ：Get_uartdata()
** 函数功能 ：读取串口命令
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************
*/
void Get_uartdata(void)
{
    static int i;
   
    if (Serial.available() > 0) //判断串口缓冲器是否有数据装入
    {
        serial_data = Serial.read();//读取串口
        if(rec_flag==0)
        {
            if(serial_data==0xff)
            {
                rec_flag = 1;
                i = 0;
               Costtime = 0; 
            }
        }
        else
        {
            if(serial_data==0xff)
            {
                rec_flag = 0;
                if(i==3)
                {
                    Communication_Decode();
                }
                i = 0;
            }
            else
            {
                buffer[i]=serial_data;
                i++;
            }
        }
    }
}
/*
*********************************************************************************************************
** 函数名称 ：UartTimeoutCheck()
** 函数功能 ：串口超时检测
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************
*/
void UartTimeoutCheck(void)
{
    if(rec_flag == 1)
    {
       Costtime++;  
      if(Costtime == 100000)
      {
           rec_flag = 0;
      }
    }
}
