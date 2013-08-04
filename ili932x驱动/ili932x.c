/******************************************************************************
* 文件名称：ili932x.c
* 摘 要：支持ILI9320和ILI9325驱动IC控制的QVGA显示屏，使用16位并行传输
  到头文件中配置 屏幕使用方向和驱动IC类型
  注意：16位数据线色彩分布>>  BGR(565)

* 当前版本：V1.3
* 修改说明：版本修订说明：
  1.修改翻转模式下的ASCII字符写Bug
  2.增加可以在翻转模式下的自动行写
  3.优化刷图片 使用流水线方法提效率

* 创建：荣晓溪
* 修订：荣晓溪
* 创建日期：2008年9月29日
* 完成日期：2008年12月11日

*重要说明！
在.h文件中，#define Immediately时是立即显示当前画面
而如果#define Delay，则只有在执行了LCD_WR_REG(0x0007,0x0173);
之后才会显示，执行一次LCD_WR_REG(0x0007,0x0173)后，所有写入数
据都立即显示。
#define Delay一般用在开机画面的显示，防止显示出全屏图像的刷新
过程
******************************************************************************/
#include "stm32f10x_lib.h"
#include "ili932x.h"
#include "spi_flash.h"


/****************************************************************
函数名：Lcd配置函数
功能：配置所有和Lcd相关的GPIO和时钟
引脚分配为：
PE——16Bit数据总线
PD15——Lcd_rst
PD14——Lcd_rd*
PD13——Lcd_wr
PD12——Lcd_rs*
PD11——Lcd_cs
PB5——Lcd_blaklight 背光靠场效应管驱动背光模块
*****************************************************************/
void Lcd_Configuration(void)
{
  
	GPIO_InitTypeDef GPIO_InitStructure;
	/*开启相应时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);  
	/*所有Lcd引脚配置为推挽输出*/
	/*16位数据*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	/*控制脚*/
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	/*背光控制*/
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/**********************************************
函数名：Lcd初始化函数
功能：初始化Lcd
入口参数：无
返回值：无
***********************************************/
void Lcd_Initialize(void)
{
  
	Lcd_Light_ON;
	DataToWrite(0xffff);//数据线全高
	Set_nWr;
	Set_Cs;
	Set_Rs;
	Set_nRd;
	Set_Rst;
	Set_Rst;
	Delay_nms(1);
	Clr_Rst;
	Delay_nms(1);
	Set_Rst;
	Delay_nms(1);       
#ifdef ILI9325        
	LCD_WR_REG(0x00e3,0x3008);
	LCD_WR_REG(0x00e7,0x0012);
	LCD_WR_REG(0x00ef,0x1231);//Set the internal vcore voltage
	LCD_WR_REG(0x0001,0x0100);//S
#endif  
#ifdef ILI9320
	LCD_WR_REG(0x00e5,0x8000);
	LCD_WR_REG(0x0000,0x0001);
	LCD_WR_REG(0x0001,0x0100);//S
	Delay_nms(10);
#endif        
	LCD_WR_REG(0x0002,0x0700);//Line inversion        
#if   ID_AM==000       
	LCD_WR_REG(0x0003,0x0000);//屏幕旋转控制 TFM=0,TRI=0,SWAP=1,16 bits system interface  swap RGB to BRG，此处ORG和HWM 为0
#elif ID_AM==001        
	LCD_WR_REG(0x0003,0x0008);      
#elif ID_AM==010  
	LCD_WR_REG(0x0003,0x0010);        
#elif ID_AM==011
	LCD_WR_REG(0x0003,0x0018);
#elif ID_AM==100  
	LCD_WR_REG(0x0003,0x0020);      
#elif ID_AM==101  
	LCD_WR_REG(0x0003,0x0028);      
#elif ID_AM==110  
	LCD_WR_REG(0x0003,0x0030);      
#elif ID_AM==111  
	LCD_WR_REG(0x0003,0x0038);
#endif      
	LCD_WR_REG(0x0004,0x0000);
	LCD_WR_REG(0x0008,0x0207);
	LCD_WR_REG(0x0009,0x0000);
	LCD_WR_REG(0x000a,0x0000);
	LCD_WR_REG(0x000c,0x0001);//此处配置接口类型 16位 system接口
	LCD_WR_REG(0x000d,0x0000);
	LCD_WR_REG(0x000f,0x0000);      
	//电源配置
	LCD_WR_REG(0x0010,0x0000);
	LCD_WR_REG(0x0011,0x0000);
	LCD_WR_REG(0x0012,0x0000);
	LCD_WR_REG(0x0013,0x0000);
	Delay_nms(200);
	LCD_WR_REG(0x0010,0x17b0);
	LCD_WR_REG(0x0011,0x0137);
	Delay_nms(50);
	LCD_WR_REG(0x0012,0x0139);
	Delay_nms(50);
	LCD_WR_REG(0x0013,0x1700);
	LCD_WR_REG(0x0029,0x000c);
	//LCD_WR_REG(0x002b,0x000d);
	Delay_nms(50);      
#if   ID_AM==000         
	LCD_WR_REG(0x0020,0x00ef);//GRAM水平起始位置
	LCD_WR_REG(0x0021,0x013f);      
#elif ID_AM==001
	LCD_WR_REG(0x0020,0x00ef);
	LCD_WR_REG(0x0021,0x013f);      
#elif ID_AM==010
	LCD_WR_REG(0x0020,0x0000);
	LCD_WR_REG(0x0021,0x013f);      
#elif ID_AM==011
	LCD_WR_REG(0x0020,0x0000);
	LCD_WR_REG(0x0021,0x013f);       
#elif ID_AM==100
	LCD_WR_REG(0x0020,0x00ef);
	LCD_WR_REG(0x0021,0x0000);      
#elif ID_AM==101  
	LCD_WR_REG(0x0020,0x00ef);
	LCD_WR_REG(0x0021,0x0000);      
#elif ID_AM==110
	LCD_WR_REG(0x0020,0x0000);
	LCD_WR_REG(0x0021,0x0000);      
#elif ID_AM==111
	LCD_WR_REG(0x0020,0x0000);
	LCD_WR_REG(0x0021,0x0000);         
#endif       
	LCD_WR_REG(0x0030,0x0000);
	LCD_WR_REG(0x0031,0x0507);
	LCD_WR_REG(0x0032,0x0104);
	LCD_WR_REG(0x0035,0x0105);
	LCD_WR_REG(0x0036,0x0404);
	LCD_WR_REG(0x0037,0x0603);
	LCD_WR_REG(0x0038,0x0004);
	LCD_WR_REG(0x0039,0x0007);
	LCD_WR_REG(0x003c,0x0501);
	LCD_WR_REG(0x003d,0x0404);
	LCD_WR_REG(0x0050,0x0000);//水平 GRAM起始位置
	LCD_WR_REG(0x0051,0x00ef);//水平GRAM终止位置
	LCD_WR_REG(0x0052,0x0000);//垂直GRAM起始位置
	LCD_WR_REG(0x0053,0x013f);//垂直GRAM终止位置
#ifdef ILI9325        
	LCD_WR_REG(0x0060,0xa700);//G
#endif
#ifdef ILI9320        
	LCD_WR_REG(0x0060,0x2700);//G  
#endif    
	LCD_WR_REG(0x0061,0x0001);//Enables the grayscale inversion of the image by setting REV=1.??????????????????????????????
	LCD_WR_REG(0x006a,0x0000);//不使用卷曲功能
	LCD_WR_REG(0x0080,0x0000);
	LCD_WR_REG(0x0081,0x0000);
	LCD_WR_REG(0x0082,0x0000);
	LCD_WR_REG(0x0083,0x0000);
	LCD_WR_REG(0x0084,0x0000);
	LCD_WR_REG(0x0085,0x0000);
	LCD_WR_REG(0x0090,0x0010);
	LCD_WR_REG(0x0092,0x0000);
	LCD_WR_REG(0x0093,0x0003);
	LCD_WR_REG(0x0095,0x0110);
	LCD_WR_REG(0x0097,0x0000);
	LCD_WR_REG(0x0098,0x0000);     
	//显示画面写入 0x0130是不显示当前画面
	//0x0173是显示当前画面
	LCD_WR_REG(0x0007,0x0130);
#ifdef Immediately
	LCD_WR_REG(0x0007,0x0173);      
#endif        
	//第一个像素的读取数据是无效值，这里事先读取一次，舍去无效值
}


/******************************************
函数名：Lcd写命令函数
功能：向Lcd指定位置写入应有命令或数据
入口参数：Index 要寻址的寄存器地址
          ConfigTemp 写入的数据或命令值
返回值：无
******************************************/
void LCD_WR_REG(u16 Index,u16 CongfigTemp)
{
	Clr_Cs;
	Clr_Rs;
	DataToWrite(Index);
	Clr_nWr;
	//Delay_nus(1);
	Set_nWr;
	Set_Rs;       
	DataToWrite(CongfigTemp);       
	Clr_nWr;
	//Delay_nus(1);
	Set_nWr;
	Set_Cs;
}


/************************************************
函数名：Lcd写开始函数
功能：控制Lcd控制引脚 执行写操作
入口参数：无
返回值：无
************************************************/
void Lcd_WR_Start(void)
{
	Clr_Cs;
	Clr_Rs;
	DataToWrite(0x0022);
	Clr_nWr;
	//Delay_nus(1);
	Set_nWr;
	Set_Rs;
}


/*************************************************
函数名：Lcd光标起点定位函数
功能：指定320240液晶上的一点作为写数据的起始点
入口参数：x 坐标 0~239
          y 坐标 0~319
返回值：无
*************************************************/
void Lcd_SetCursor(u8 x,u16 y)
{ 
	LCD_WR_REG(0x20,x);
	LCD_WR_REG(0x21,y);    
}


/**********************************************
函数名：Lcd全屏擦除函数
功能：将Lcd整屏擦为指定颜色
入口参数：color 指定Lcd全屏颜色 RGB(5-6-5)
返回值：无
***********************************************/
void Lcd_Clear(u16 Color)
{
	u32 temp;
  
	Lcd_SetCursor(0x00, 0x0000);
	LCD_WR_REG(0x0050,0x00);//水平 GRAM起始位置
	LCD_WR_REG(0x0051,239);//水平GRAM终止位置
	LCD_WR_REG(0x0052,0x00);//垂直GRAM起始位置
	LCD_WR_REG(0x0053,319);//垂直GRAM终止位置   
	Lcd_WR_Start();
	Set_Rs;
  
	for (temp = 0; temp < 76800; temp++)
	{
		DataToWrite(Color);
		Clr_nWr;
		Set_nWr;
	}
  
	Set_Cs;
}
/**********************************************
函数名：Lcd块选函数
功能：选定Lcd上指定的矩形区域

注意：xStart和 yStart随着屏幕的旋转而改变，位置是矩形框的四个角

入口参数：xStart x方向的起始点
          ySrart y方向的终止点
          xLong 要选定矩形的x方向长度
          yLong  要选定矩形的y方向长度
返回值：无
***********************************************/
void Lcd_SetBox(u8 xStart,u16 yStart,u8 xLong,u16 yLong,u16 x_offset,u16 y_offset)
{
  
#if ID_AM==000    
	Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+yLong-1+y_offset);

#elif ID_AM==001
	Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+yLong-1+y_offset);
     
#elif ID_AM==010
	Lcd_SetCursor(xStart+x_offset,yStart+yLong-1+y_offset);
     
#elif ID_AM==011 
	Lcd_SetCursor(xStart+x_offset,yStart+yLong-1+y_offset);
     
#elif ID_AM==100
	Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+y_offset);     
     
#elif ID_AM==101
	Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+y_offset);     
     
#elif ID_AM==110
	Lcd_SetCursor(xStart+x_offset,yStart+y_offset); 
     
#elif ID_AM==111
	Lcd_SetCursor(xStart+x_offset,yStart+y_offset);  
     
#endif
     
	LCD_WR_REG(0x0050,xStart+x_offset);//水平 GRAM起始位置
	LCD_WR_REG(0x0051,xStart+xLong-1+x_offset);//水平GRAM终止位置
	LCD_WR_REG(0x0052,yStart+y_offset);//垂直GRAM起始位置
	LCD_WR_REG(0x0053,yStart+yLong-1+y_offset);//垂直GRAM终止位置 
}


void Lcd_ColorBox(u8 xStart,u16 yStart,u8 xLong,u16 yLong,u16 Color)
{
	u32 temp;
  
	Lcd_SetBox(xStart,yStart,xLong,yLong,0,0);
	Lcd_WR_Start();
	Set_Rs;
  
	for (temp=0; temp<xLong*yLong; temp++)
	{
		DataToWrite(Color);
		Clr_nWr;
		Set_nWr;
	}

	Set_Cs;
}


void Lcd_ClearCharBox(u8 x,u16 y,u16 Color)
{
	u32 temp;
  
	Lcd_SetBox(x*8,y*16,8,16,0,0); 
	Lcd_WR_Start();
	Set_Rs;
  
	for (temp=0; temp < 128; temp++)
	{
		DataToWrite(Color); 
		Clr_nWr;
		//Delay_nus(22);
		Set_nWr; 
	}
	
	Set_Cs;
}


/****************************************************************
函数名：Lcd写1个ASCII字符函数
入口参数：x,横向坐标，由左到右分别是0~29 
          y,纵向坐标，由上到下分别为0~19
          CharColaor,字符的颜色 
          CharBackColor,字符背景颜色 
         ASCIICode,相应字符的ASCII码
也就是说，320240分辨率的显示屏，横向能显示30个ASCII字符，竖向能显示20行
返回值：无

注意！！！！！如果单独使用此函数则应该加上Lcd_Rs_H()和Set_Cs;为了优化系统省去了
这个指令，假设此函数执行的上一条语句是写命令，（RS_L情况）则写入将出错
，因为ILI9320认为当RS_L时写入的是命令
*****************************************************************/
void Lcd_WriteASCII(u8 x,u8 y,u16 x_offset,u16 y_offset,u16 CharColor,u16 CharBackColor,u8 ASCIICode)
{
	u8 RowCounter,BitCounter;
	u8 *ASCIIPointer;
	u8 ASCIIBuffer[16];
  
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)
	Lcd_SetBox(x*8,y*16,8,16,x_offset,y_offset);    
#else
	Lcd_SetBox(x*16,y*8,16,8,x_offset,y_offset);    
#endif 
        
	Lcd_WR_Start();
	GetASCIICode(ASCIIBuffer,ASCIICode,ASCII_Offset);//取这个字符的显示代码
	ASCIIPointer=ASCIIBuffer;
  
	for (RowCounter=0; RowCounter<16; RowCounter++)
	{ 
		for (BitCounter=0; BitCounter<8; BitCounter++)
		{
			if ((*ASCIIPointer&(0x80 >> BitCounter)) == 0x00)
			{
				//Set_Rs;
				DataToWrite(CharBackColor);
				Clr_nWr;
				Set_nWr;
			}
			else
			{
				//Set_Rs;
				DataToWrite(CharColor);
				Clr_nWr;
				Set_nWr; 
			}
		}
		ASCIIPointer++;
	}
	// Set_Cs;
}


void Lcd_WriteASCIIClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u8 ASCIICode)
{
  
  GPIO_InitTypeDef GPIO_InitStructure;
  u8 RowCounter,BitCounter;
  u8 *ASCIIPointer;
  u8 ASCIIBuffer[16];
  u16 Temp;
        
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)
  Lcd_SetBox(x*8,y*16,8,16,x_offset,y_offset);    
#else
  Lcd_SetBox(x*16,y*8,16,8,x_offset,y_offset);    
#endif
    
  Lcd_WR_Start();
  GetASCIICode(ASCIIBuffer,ASCIICode,ASCII_Offset);//取这个字符的显示代码
  ASCIIPointer=ASCIIBuffer;
  
  for (RowCounter=0; RowCounter<16; RowCounter++)
  { 
    for(BitCounter=0;BitCounter<8;BitCounter++)
    {
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }
    ASCIIPointer++;
  }
  // Set_Cs;
  
}
/**************************************************************
向液晶屏写入32*16的ASCII字符
输入参数：
x向坐标   x :(0~14)
y向坐标   y :(0~9)
x向偏移量 x_offset:(理论上是0~239)
y向偏移量 y_offset:(理论上是0~319)  注意：偏移量不能过大，当x轴向超出16，y轴向超过32
                                          也就是32*16字符大小时强烈建议使用x和y调整，
                                          虽然偏移量可以实现大范围偏移但是可读性和维
                                          护性都较差
字符颜色 CharColor:字符颜色
背景颜色 CharBackColor：背景颜色
***************************************************************/
void Lcd_Write32X16ASCII(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,u8 ASCIICode)
{

  u8 RowCounter,BitCounter;
  u8 *ASCIIPointer;
  u8 ASCIIBuffer[16];
  
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)
  Lcd_SetBox(x*16,y*32,16,32,x_offset,y_offset);    
#else
  Lcd_SetBox(x*32,y*16,32,16,x_offset,y_offset);    
#endif
    
  Lcd_WR_Start();
  GetASCIICode(ASCIIBuffer,ASCIICode,ASCII_Offset);//取这个字符的显示代码
  ASCIIPointer=ASCIIBuffer;
  
  for(RowCounter=0; RowCounter<16; RowCounter++)
  { 
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }      
    }     
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {         
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }
    ASCIIPointer++;
  }
  // Set_Cs;
}
//可以单独使用的写一个ASCII字符函数
void Lcd_Write32X16ASCIIWrite(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,u8 ASCIICode)
{
        u8 RowCounter,BitCounter;
        u8 *ASCIIPointer;
        
        //配置ASCII字符位置
#if   (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110) 
        
        Lcd_SetBox(x*16,y*32,16,32,x_offset,y_offset);
        
#else
        
        Lcd_SetBox(x*32,y*16,32,16,x_offset,y_offset);
        
#endif
        
        Lcd_WR_Start();
        CatchASCII(ASCIICode,ASCII_Offset);//取这个字符的显示代码
        ASCIIPointer=CharBuffer;
        for(RowCounter=0;RowCounter<16;RowCounter++)
        { 
          for(BitCounter=0;BitCounter<8;BitCounter++)
          {
            if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
            {
               
               Set_Rs;
               DataToWrite(CharBackColor);
               Clr_nWr;
               Set_nWr;
               DataToWrite(CharBackColor);
               Clr_nWr;
               Set_nWr;

            }
            else
            {
               Set_Rs;
               DataToWrite(CharColor);
               Clr_nWr;
               Set_nWr; 
               DataToWrite(CharColor);
               Clr_nWr;
               Set_nWr; 
            }
            
          }
          
        for(BitCounter=0;BitCounter<8;BitCounter++)
          {
            if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
            {
               
               Set_Rs;
               DataToWrite(CharBackColor);
               Clr_nWr;
               Set_nWr;
               DataToWrite(CharBackColor);
               Clr_nWr;
               Set_nWr;

            }
            else
            {
               Set_Rs;
               DataToWrite(CharColor);
               Clr_nWr;
               Set_nWr; 
               DataToWrite(CharColor);
               Clr_nWr;
               Set_nWr; 
            }
            
          }
           ASCIIPointer++;
        }
         Set_Cs;
}
void Lcd_Write32X16ASCIIClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u8 ASCIICode)
{

  GPIO_InitTypeDef GPIO_InitStructure;
  u8 RowCounter,BitCounter;
  u8 *ASCIIPointer;
  u8 ASCIIBuffer[16];
  u16 Temp;

#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)
  Lcd_SetBox(x*16,y*32,16,32,x_offset,y_offset);    
#else
  Lcd_SetBox(x*32,y*16,32,16,x_offset,y_offset);    
#endif
    
  Lcd_WR_Start();
  GetASCIICode(ASCIIBuffer,ASCIICode,ASCII_Offset);//取这个字符的显示代码
  ASCIIPointer=ASCIIBuffer;
  
  for(RowCounter=0; RowCounter<16; RowCounter++)
  { 
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                     
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }    
    }
          
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
              
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;       
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;          
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }            
    }
    ASCIIPointer++;
  }
  // Set_Cs;
}
/************************************************************
函数名：Lcd写字符串函数
功能：向指定位置写入一个或多个字符，本函数带自动换行功能
入口参数：x,横向坐标，由左到右分别是0~29 
          y,纵向坐标，由上到下分别为0~19
          CharColaor,字符的颜色 
          CharBackColor,字符背景颜色 
          *s 指向要写的字符串
返回值：无
*************************************************************/
void Lcd_WriteString(u8 x,u8 y,u16 x_offset,u16 y_offset,u16 CharColor,u16 CharBackColor,char *s)
{

  u8 databuff;
  Set_Rs;

  do
  {
    databuff=*s++;  
    Lcd_WriteASCII(x,y,x_offset,y_offset,CharColor,CharBackColor,databuff);
    
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<29)
    {
      x++;
    } 
    else if (y<19)
    {
      x=0;
      y++;
    }   
    else
    {
      x=0;
      y=0;
    }
       
#else
    if (y<39)
    {
      y++;
    }
    else if (x<14)
    {
      y=0;
      x++;
    }
    else
    {
      x=0;
      y=0;
    }
#endif 
       
  }
  while (*s!=0);
  
  Set_Cs;
  
}
void Lcd_WriteStringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s)
{

  u8 databuff;
  Set_Rs;

  do
  {
    databuff=*s++;  
    Lcd_WriteASCIIClarity(x,y,x_offset,y_offset,CharColor,databuff);
    
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<29)
    {
      x++;
    } 
    else if (y<19)
    {
      x=0;
      y++;
    }   
    else
    {
      x=0;
      y=0;
    }       
#else
    if (y<39)
    {
      y++;
    }
    else if (x<14)
    {
      y=0;
      x++;
    }
    else
    {
      x=0;
      y=0;
    }
#endif
    
  }
  while(*s!=0);
  
  Set_Cs;
  
}
/****************************************
写入32X16的ASCII字符串
*****************************************/
void Lcd_Write32X16String(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,char *s)
{

  u8 databuff;
  Set_Rs;
  
  do
  {
    databuff=*s++;  
    Lcd_Write32X16ASCII(x,y,x_offset,y_offset,CharColor,CharBackColor,databuff);
    
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<14)
    {
      x++;
    } 
    else if (y<9)
    {
      x=0;
      y++;
    }   
    else
    {
      x=0;
      y=0;
    }   
#else
    if (y<6)
    {
      y++;
    }
    else if (x<19)
    {
      y=0;
      x++;
    }
    else
    {
      x=0;
      y=0;
    }   
#endif
      
  }
  while (*s!=0);
    
  Set_Cs;
  
}
void Lcd_Write32X16StringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s)
{

  u8 databuff;
  Set_Rs;
  
  do
  {
    databuff=*s++;  
    Lcd_Write32X16ASCIIClarity(x,y,x_offset,y_offset,CharColor,databuff);

#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<14)
    {
      x++;
    } 
    else if (y<9)
    {
      x=0;
      y++;
    }   
    else
    {
      x=0;
      y=0;
    }     
#else
    if (y<6)
    {
      y++;
    }
    else if (x<19)
    {
      y=0;
      x++;
    }
    else
    {
      x=0;
      y=0;
    }
#endif
    
  }
  while(*s!=0);
  
  Set_Cs;
  
}
/****************************************************************
函数名：Lcd写1个中文函数
入口参数：x,横向坐标，由左到右分别是0~15
          y,纵向坐标，由上到下分别为0~19
          CharColaor,字符的颜色 
          CharBackColor,字符背景颜色 
         ASCIICode,相应中文的编码
也就是说，320240分辨率的显示屏，横向能显示15中文字符，竖向能显示20行
返回值：无

注意！！！！！如果单独使用此函数则应该加上Lcd_Rs_H()和Set_Cs;为了优化系统省去了
这个指令，假设此函数执行的上一条语句是写命令，（RS_L情况）则写入将出错
，因为ILI9320认为当RS_L时写入的是命令
*****************************************************************/
void Lcd_WriteChinese(u8 x,u8 y,u16 x_offset,u16 y_offset,u16 CharColor,u16 CharBackColor,u16 ChineseCode)
{

  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer[32];
        
  Lcd_SetBox(x*16,y*16,16,16,x_offset,y_offset);         
  Lcd_WR_Start();
  GetChineseCode(ChineseBuffer,ChineseCode,Chinese_Offset);
  ChinesePointer=ChineseBuffer;
        
  for(ByteCounter=0; ByteCounter<32; ByteCounter++)
  { 
    for(BitCounter=0;BitCounter<8;BitCounter++)
    {
      if((*ChinesePointer & (0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }     
    }
    ChinesePointer++;
  }
  //Set_Cs;
  
}
void Lcd_WriteChineseClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 ChineseCode)
{

  GPIO_InitTypeDef GPIO_InitStructure;
  u16 Temp;
  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer[32];
  
  Lcd_SetBox(x*16,y*16,16,16,x_offset,y_offset);         
  Lcd_WR_Start();
  GetChineseCode(ChineseBuffer,ChineseCode,Chinese_Offset);
  ChinesePointer=ChineseBuffer;
  
  for(ByteCounter=0; ByteCounter<32; ByteCounter++)
  { 
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if((*ChinesePointer & (0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }     
    }
  ChinesePointer++;
  }
  //Set_Cs;
  
}
/****************************************************************
函数名：Lcd写1个32X32中文函数
入口参数：x,横向坐标，由左到右分别是0~7
          y,纵向坐标，由上到下分别为0~9
          CharColaor,字符的颜色 
          CharBackColor,字符背景颜色 
         ASCIICode,相应中文的编码
也就是说，320240分辨率的显示屏，横向能显示7中文字符，竖向能显示10行
返回值：无

注意！！！！！如果单独使用此函数则应该加上Lcd_Rs_H()和Set_Cs;为了优化系统省去了
这个指令，假设此函数执行的上一条语句是写命令，（RS_L情况）则写入将出错
，因为ILI9320认为当RS_L时写入的是命令
*****************************************************************/
void Lcd_Write32X32Chinese(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,u16 ChineseCode)
{
        
  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer[32];
  
  Lcd_SetBox(x*32,y*32,32,32,x_offset,y_offset);         
  Lcd_WR_Start();
  GetChineseCode(ChineseBuffer,ChineseCode,Chinese_Offset);
  ChinesePointer=ChineseBuffer;
  
  for(ByteCounter=0; ByteCounter<16; ByteCounter++)
  { 
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*ChinesePointer)&(0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }
    for (BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*(ChinesePointer+1)) & (0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }          
    for (BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*ChinesePointer) & (0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }        
    for (BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*(ChinesePointer+1)) & (0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }
    ChinesePointer+=2;
  }
  //Set_Cs;
 }
void Lcd_Write32X32ChineseClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 ChineseCode)
{
        
  GPIO_InitTypeDef GPIO_InitStructure;
  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer[32];
  u16 Temp;

  Lcd_SetBox(x*32,y*32,32,32,x_offset,y_offset);         
  Lcd_WR_Start();
  GetChineseCode(ChineseBuffer,ChineseCode,Chinese_Offset);
  ChinesePointer=ChineseBuffer;       
  
  for(ByteCounter=0; ByteCounter<16; ByteCounter++)
  { 
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*ChinesePointer)&(0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                     
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }
          
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*(ChinesePointer+1))&(0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                     
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }

          
          
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*ChinesePointer)&(0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                     
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }
          
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
      if (((*(ChinesePointer+1))&(0x80 >> BitCounter)) == 0x00)
      {
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                     
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        Clr_nRd;
        Set_nRd;
               
        Temp=GPIO_ReadInputData(GPIOE);
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
               
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr; 
      }
      else
      {
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
      }
    }
      ChinesePointer+=2;
  }
  //Set_Cs;
  
}
void Lcd_WriteChineseString(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,char *s)
{
   
  u8 databuffer;
  u16 ChineseCode;
  Set_Rs;
  
  do
  {
    databuffer=*s++;
    ChineseCode=databuffer<<8;
    ChineseCode=ChineseCode|*s++;
    Lcd_WriteChinese(x,y,x_offset,y_offset,CharColor,CharBackColor,ChineseCode);
    
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<14)
    {
      x++;
    }
    else if (y<19)
    {
      x=0;
      y++;
    }
    else
    {
      x=0;
      y=0;
    }     
#else
    if (y<19)
    {
      y++;
    }
    else if (x<14)
    {
      y=0;
      x++;
    }
    else
    {
      x=0;
      y=0;
    }  
#endif
      
  }
  while(*s!=0);
    
  Set_Cs;

}
void Lcd_WriteChineseStringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s)
{
   u8 databuffer;
   u16 ChineseCode;
   Set_Rs;
    do
   {
       databuffer=*s++;
       ChineseCode=databuffer<<8;
       ChineseCode=ChineseCode|*s++;
       Lcd_WriteChineseClarity(x,y,x_offset,y_offset,CharColor,ChineseCode);
       
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
       if (x<14)
       {
         x++;
       }
       else if (y<19)
       {
         x=0;
         y++;
       }
       else
       {
         x=0;
         y=0;
       }     
#else
       if (y<19)
       {
         y++;
       }
       else if (x<14)
       {
         y=0;
         x++;
       }
       else
       {
         x=0;
         y=0;
       }  
#endif
       
   }
     while(*s!=0);
    Set_Cs;
}
void Lcd_Write32X32ChineseString(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,char *s)
{
   
  u8 databuffer;
  u16 ChineseCode;
   
  Set_Rs;
  do
  {
    databuffer=*s++;
    ChineseCode=databuffer<<8;
    ChineseCode=ChineseCode|*s++;
    Lcd_Write32X32Chinese(x,y,x_offset,y_offset,CharColor,CharBackColor,ChineseCode);
       
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<6)
    {
      x++;
    }
    else if (y<9)
    {
      x=0;
      y++;
    }
    else
    {
      x=0;
      y=0;
    }     
#else
    if (y<9)
    {
      y++;
    }
    else if (x<6)
    {
      y=0;
      x++;
    }
    else
    {
      x=0;
      y=0;
    }  
#endif
     
  }
  while(*s!=0);

  Set_Cs;

}
void Lcd_Write32X32ChineseStringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s)
{
		u8 databuffer;
		u16 ChineseCode;
		Set_Rs;

		do
		{
			databuffer=*s++;
			ChineseCode=databuffer<<8;
			ChineseCode=ChineseCode|*s++;
			Lcd_Write32X32ChineseClarity(x,y,x_offset,y_offset,CharColor,ChineseCode);
    
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  

		if (x<6)
		{
			x++;
		}
		else if (y<9)
		{
			x=0;
			y++;
		}
		else
		{
			x=0;
			y=0;
		}     
#else
		if (y<9)
		{
			y++;
		}
		else if (x<6)
		{
			y=0;
			x++;
		}
			else
		{
			x=0;
			y=0;
		}  
#endif
    
	}
	while (*s!=0);
	
	Set_Cs;
}
/*********************************************************
这是画图函数
**********************************************************/
void LcdWritePictureFromSPI(u8 xStart,u16 yStart,u8 xLong,u16 yLong,u32 BaseAddr)
{
	ColorTypeDef color;
	u32 pixels;
	
	Lcd_SetBox(xStart,yStart,xLong,yLong,0,0);
	Lcd_WR_Start();
	Set_Rs;
	
	SPI_FLASH_CS_LOW();
	SPI_FLASH_SendByte(0x0B);//FSTREAD
	SPI_FLASH_SendByte((BaseAddr & 0xFF0000) >> 16);
	SPI_FLASH_SendByte((BaseAddr& 0xFF00) >> 8);
	SPI_FLASH_SendByte(BaseAddr & 0xFF);
	SPI_FLASH_SendByte(0);//Dummy_Byte
	
	SPI2->DR = 0;//Dummy_Byte
	while((SPI2->SR & SPI_I2S_FLAG_RXNE) == (u16) RESET);
	color.U8[1] = SPI2->DR;	
	
	SPI2->DR = 0;//Dummy_Byte
	
	for (pixels=0; pixels<(xLong*yLong); pixels++)
	{

		while((SPI2->SR & SPI_I2S_FLAG_RXNE) == (u16) RESET);
		color.U8[0] = SPI2->DR;
		
		SPI2->DR = 0;//Dummy_Byte
		
		DataToWrite(color.U16);
		Clr_nWr;
		Set_nWr;
		
		while((SPI2->SR & SPI_I2S_FLAG_RXNE) == (u16) RESET);
		color.U8[1] = SPI2->DR;
		
		SPI2->DR = 0;//Dummy_Byte
	}

	SPI_FLASH_CS_HIGH();
	Set_Cs;  
}
/*********************************************************
函数名：SPI取ASCII码子程序
输入参数：u8 ASCII 输入的ASCII码，如'A'
          BaseAddr 基址 即ASCII显示代码在FLASH中的启示位置
返回值：无
说明：输入一个ASCII码，取得它在SPI FLASH中的16Byte显示代码
并将其存放到一个16byte的ASCII显示缓冲CharBuffer[]中
**********************************************************/
void GetASCIICode(u8* pBuffer,u8 ASCII,u32 BaseAddr)
{
  
  SPI_FLASH_BufferRead(pBuffer,BaseAddr+16*ASCII,16);

}
/*********************************************************
函数名：SPI中文显示码子程序
输入参数：u16 ASCII 输入的中文，如"我"
          BaseAddr 基地 即显示代码在FLASH中的起始位置
返回值：无
说明：输入一个中文，取得它在SPI FLASH中的32Byte显示代码
并将其存放到一个32byte的显示缓冲ChineseBuffer[]
**********************************************************/
void GetChineseCode(u8* pBuffer,u16 ChineseCode,u32 BaseAddr)
{
  
  u8 High8bit,Low8bit;
  u16 temp;
  
  temp=ChineseCode-0xA0A0;//算出汉字区位码
  High8bit=(temp>>8);
  Low8bit=(temp&0x00FF);
  
  SPI_FLASH_BufferRead(pBuffer,BaseAddr+32*((High8bit-1)*94+Low8bit-1),32);

}
void Get320240PictureCode(u8* pBuffer,u32 BufferCounter,u32 BaseAddr)
{
    SPI_FLASH_BufferRead(pBuffer,BaseAddr+BufferCounter*32,32);
}
void Delay_nms(int n)
{
  
  u32 f=n,k;
  for (; f!=0; f--)
  {
    for(k=0xFFF; k!=0; k--);
  }
  
}
