#include <stdio.h>
#include "bsp_software_i2c.h"
#include "driver_gpio.h"

/************************代码移植修改区************************************/
SOFT_I2C_SCL_DEF(CAMERA_IIC, B, 4); 
SOFT_I2C_SDA_DEF(CAMERA_IIC, B, 7); 
SOFT_I2C_DEF(CAMERA_IIC,100000);


SOFT_I2C_SCL_DEF(TOUCH_CAP_IIC, D, 5); 
SOFT_I2C_SDA_DEF(TOUCH_CAP_IIC, D, 7); 
SOFT_I2C_DEF(TOUCH_CAP_IIC,400000);

/**************************************************************************/

static void SDA_IN(typdef_software_i2c* soft_i2c)  
{
    soft_i2c->iic_sda->gpio_mode=IN_PU; 
    driver_gpio_general_init(soft_i2c->iic_sda);
}

static void SDA_OUT_PP(typdef_software_i2c* soft_i2c,bit_status bit_value)  
{
    soft_i2c->iic_sda->gpio_mode=OUT_PP; 
    soft_i2c->iic_sda->default_state=bit_value;    
    driver_gpio_general_init(soft_i2c->iic_sda);
    soft_i2c->iic_sda->default_state=SET; 
}

static void SDA_OUT_OD(typdef_software_i2c* soft_i2c,bit_status bit_value)  
{
    soft_i2c->iic_sda->gpio_mode=OUT_OD; 
    soft_i2c->iic_sda->default_state=bit_value;    
    driver_gpio_general_init(soft_i2c->iic_sda);
    soft_i2c->iic_sda->default_state=SET;  
}

//IO操作函数	 
static void IIC_SCL_H(typdef_software_i2c* soft_i2c)    
{
    driver_gpio_pin_set(soft_i2c->iic_scl); //SCL
}

//IO操作函数	 
static void IIC_SDA_H(typdef_software_i2c* soft_i2c)    
{
    driver_gpio_pin_set(soft_i2c->iic_sda); //SCL
}

//IO操作函数	 
static void IIC_SDA_L(typdef_software_i2c* soft_i2c)    
{
    driver_gpio_pin_reset(soft_i2c->iic_sda); //SCL
}

//IO操作函数	 
static void IIC_SCL_L(typdef_software_i2c* soft_i2c)    
{
    driver_gpio_pin_reset(soft_i2c->iic_scl); //SCL
}

//IO操作函数	 
static bit_status READ_SDA(typdef_software_i2c* soft_i2c)    
{
    return driver_gpio_pin_get(soft_i2c->iic_sda); //SCL
}


/******************************************************************************
*函  数：void bsp_software_i2c_delay(soft_i2c,void)
*功　能：IIC延时
*参  数：无
*返回值：无
*备  注: 移植时只需要将bsp_software_i2c_delay(soft_i2c,)换成自己的延时即可
*******************************************************************************/	

void bsp_software_i2c_delay(typdef_software_i2c* soft_i2c,uint8_t delay)
{   
    while(delay--)
    {
        delay_us(soft_i2c->delay_us);
    }
}

/******************************************************************************
*函  数：void bsp_software_i2c_init(void)
*功　能：IIC初始化
*参  数：无
*返回值：无
*备  注：无
*******************************************************************************/	
void bsp_software_i2c_init(typdef_software_i2c* soft_i2c)
{			
    driver_gpio_general_init(soft_i2c->iic_sda);
    driver_gpio_general_init(soft_i2c->iic_scl);    
}
/******************************************************************************
*函  数：void bsp_software_i2c_start(void)
*功　能：产生IIC起始信号
*参  数：无
*返回值：无
*备  注：无
*******************************************************************************/	
void bsp_software_i2c_start(typdef_software_i2c* soft_i2c)
{
	SDA_OUT_PP(soft_i2c,SET); //sda线输出 
	IIC_SDA_H(soft_i2c);	
	IIC_SCL_H(soft_i2c);
	bsp_software_i2c_delay(soft_i2c,1);
 	IIC_SDA_L(soft_i2c); //START:when CLK is high,DATA change form high to low 
	bsp_software_i2c_delay(soft_i2c,1);
	IIC_SCL_L(soft_i2c); //钳住I2C总线，准备发送或接收数据 
	bsp_software_i2c_delay(soft_i2c,1);    
}

/******************************************************************************
*函  数：void bsp_software_i2c_stop(void)
*功　能：产生IIC停止信号
*参  数：无
*返回值：无
*备  注：无
*******************************************************************************/	  
void bsp_software_i2c_stop(typdef_software_i2c* soft_i2c)
{
	IIC_SCL_L(soft_i2c);
	SDA_OUT_PP(soft_i2c,RESET); //sda线输出    
	IIC_SDA_L(soft_i2c); //STOP:when CLK is high DATA change form low to high
  	bsp_software_i2c_delay(soft_i2c,1);
	IIC_SCL_H(soft_i2c); 
  	bsp_software_i2c_delay(soft_i2c,1);    
	IIC_SDA_H(soft_i2c); //发送I2C总线结束信号
    bsp_software_i2c_delay(soft_i2c,1);							   	
}

/******************************************************************************
*函  数: uint8_t bsp_software_i2c_wait_ack(void)
*功　能: 等待应答信号到来 （有效应答：从机第9个 SCL=0 时 SDA 被从机拉低,
                            并且 SCL = 1时 SDA依然为低）
*参  数：无
*返回值：1，接收应答失败
         0，接收应答成功
*备  注：从机给主机的应答
*******************************************************************************/
uint8_t bsp_software_i2c_wait_ack(typdef_software_i2c* soft_i2c)
{
	uint8_t ucErrTime=0;
	IIC_SCL_L(soft_i2c); //时钟输出0 	         
    bsp_software_i2c_delay(soft_i2c,1);	    
	IIC_SDA_H(soft_i2c);    
	while(READ_SDA(soft_i2c))
	{        
		ucErrTime++;
		if(ucErrTime>5)
		{
            IIC_SCL_H(soft_i2c);
            bsp_software_i2c_delay(soft_i2c,1);	
			bsp_software_i2c_stop(soft_i2c);
			return 1;
		}
        bsp_software_i2c_delay(soft_i2c,1);
	}
    IIC_SCL_H(soft_i2c);
    bsp_software_i2c_delay(soft_i2c,1);    
	IIC_SCL_L(soft_i2c); //时钟输出0 	   
	return 0;  
} 

/******************************************************************************
*函  数: void bsp_software_i2c_ack(void)
*功　能: 产生ACK应答 （主机接收完一个字节数据后，主机产生的ACK通知从机一个
                       字节数据已正确接收）
*参  数：无
*返回值：无
*备  注：主机给从机的应答
*******************************************************************************/

void bsp_software_i2c_ack(typdef_software_i2c* soft_i2c)
{
	IIC_SCL_L(soft_i2c);
	SDA_OUT_PP(soft_i2c,RESET);
	IIC_SDA_L(soft_i2c);
	bsp_software_i2c_delay(soft_i2c,1);
	IIC_SCL_H(soft_i2c);
	bsp_software_i2c_delay(soft_i2c,1);
	IIC_SCL_L(soft_i2c);
}

/******************************************************************************
*函  数: void bsp_software_i2c_nack(void)
*功　能: 产生NACK应答 （主机接收完最后一个字节数据后，主机产生的NACK通知从机
                        发送结束，释放SDA,以便主机产生停止信号）
*参  数：无
*返回值：无
*备  注：主机给从机的应答
*******************************************************************************/
void bsp_software_i2c_nack(typdef_software_i2c* soft_i2c)
{
	IIC_SCL_L(soft_i2c);
	SDA_OUT_PP(soft_i2c,RESET);
	IIC_SDA_H(soft_i2c);
	bsp_software_i2c_delay(soft_i2c,1);
	IIC_SCL_H(soft_i2c);
	bsp_software_i2c_delay(soft_i2c,1);
	IIC_SCL_L(soft_i2c);
}					 				     

/******************************************************************************
*函  数：void bsp_software_i2c_send_byte(soft_i2c,uint8_t txd)
*功  能：IIC发送一个字节
*参  数：data 要写的数据
*返回值：无
*备  注：主机往从机发
*******************************************************************************/		  
void bsp_software_i2c_send_byte(typdef_software_i2c* soft_i2c,uint8_t data)
{                        
    uint8_t t;   
    
    SDA_OUT_PP(soft_i2c,(bit_status)((data&0x80)>>7)); 
    
    for(t=0;t<8;t++)
    {   
        IIC_SCL_L(soft_i2c);
        if(t==7)
        {
            SDA_OUT_OD(soft_i2c,(bit_status)((data&0x80)>>7));         
        }
        if((data&0x80)>>7)
            IIC_SDA_H(soft_i2c);
        else
            IIC_SDA_L(soft_i2c);
        data<<=1;	
        bsp_software_i2c_delay(soft_i2c,1);        
        IIC_SCL_H(soft_i2c);
        bsp_software_i2c_delay(soft_i2c,1);	   
    }   
} 	 
   
/******************************************************************************
*函  数：uint8_t bsp_software_i2c_read_byte(soft_i2c,uint8_t ack)
*功  能：IIC读取一个字节
*参  数：ack=1 时，主机数据还没接收完 ack=0 时主机数据已全部接收完成
*返回值：无
*备  注：从机往主机发
*******************************************************************************/	
uint8_t bsp_software_i2c_read_byte(typdef_software_i2c* soft_i2c,uint8_t ack)
{
	uint8_t i,receive=0;
	SDA_IN(soft_i2c); //SDA设置为输入模式 等待接收从机返回数据
    for(i=0;i<8;i++ )
    {
        IIC_SCL_L(soft_i2c); 
        bsp_software_i2c_delay(soft_i2c,1);
        IIC_SCL_H(soft_i2c);
        receive<<=1;
        if(READ_SDA(soft_i2c)) receive++; //从机发送的电平
        bsp_software_i2c_delay(soft_i2c,1); 
    }					 
    if(ack)
        bsp_software_i2c_ack(soft_i2c); //发送ACK 
    else
        bsp_software_i2c_nack(soft_i2c); //发送nACK  
    return receive;
}

/******************************************************************************
*函  数：uint8_t bsp_software_i2c_read_byteFromSlave(uint8_t I2C_Addr,uint8_t addr)
*功　能：读取指定设备 指定寄存器的一个值
*参  数：I2C_Addr  目标设备地址
		     reg	     寄存器地址
         *buf      读取数据要存储的地址    
*返回值：返回 1失败 0成功
*备  注：无
*******************************************************************************/ 
uint8_t bsp_software_i2c_mem_read_byte(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr,uint8_t reg,uint8_t *buf)
{
	bsp_software_i2c_start(soft_i2c);	
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|0);	 //发送从机地址
	if(bsp_software_i2c_wait_ack(soft_i2c)) //如果从机未应答则数据发送失败
	{
		bsp_software_i2c_stop(soft_i2c);
		return 1;
	}
	bsp_software_i2c_send_byte(soft_i2c,reg); //发送寄存器地址
	bsp_software_i2c_wait_ack(soft_i2c);	  
	
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|1); //进入接收模式			   
	bsp_software_i2c_wait_ack(soft_i2c);
	*buf=bsp_software_i2c_read_byte(soft_i2c,0);	   
  bsp_software_i2c_stop(soft_i2c); //产生一个停止条件
	return 0;
}


uint8_t bsp_software_i2c_read_data(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr,uint8_t *buf)
{
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|1); //进入接收模式			   
	bsp_software_i2c_wait_ack(soft_i2c);
	*buf=bsp_software_i2c_read_byte(soft_i2c,0);	   
    bsp_software_i2c_stop(soft_i2c); //产生一个停止条件
	return 0;
}

/******************************************************************************
*函  数：uint8_t IIC_WriteByteFromSlave(uint8_t I2C_Addr,uint8_t addr，uint8_t buf))
*功　能：写入指定设备 指定寄存器的一个值
*参  数：I2C_Addr  目标设备地址
		     reg	     寄存器地址
         buf       要写入的数据
*返回值：1 失败 0成功
*备  注：无
*******************************************************************************/ 
uint8_t bsp_software_i2c_mem_write_byte(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr,uint8_t reg,uint8_t data)
{
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|0); //发送从机地址
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c);
		return 1; //从机地址写入失败
	}
	bsp_software_i2c_send_byte(soft_i2c,reg); //发送寄存器地址
	bsp_software_i2c_wait_ack(soft_i2c);	  
	bsp_software_i2c_send_byte(soft_i2c,data); 
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c); 
		return 1; //数据写入失败
	}
	bsp_software_i2c_stop(soft_i2c); //产生一个停止条件

  //return 1; //status == 0;
	return 0;
}

uint8_t bsp_software_i2c_write_byte(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr,uint8_t data)
{
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|0); //发送从机地址
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c);
		return 1; //从机地址写入失败
	}  
	bsp_software_i2c_send_byte(soft_i2c,data); 
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c); 
		return 1; //数据写入失败
	}
	bsp_software_i2c_stop(soft_i2c); //产生一个停止条件
	return 0;
}

/******************************************************************************
*函  数：uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data)
*功　能：读取指定设备 指定寄存器的 length个值
*参  数：dev     目标设备地址
		     reg	   寄存器地址
         length  要读的字节数
		     *data   读出的数据将要存放的指针
*返回值：1成功 0失败
*备  注：无
*******************************************************************************/ 
uint8_t bsp_software_i2c_mem_read(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr, uint8_t reg, uint8_t length, uint8_t *data)
{
  uint8_t count = 0;
	uint8_t temp;
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|0); //发送从机地址
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c); 
		return 1; //从机地址写入失败
	}
	bsp_software_i2c_send_byte(soft_i2c,reg); //发送寄存器地址
	bsp_software_i2c_wait_ack(soft_i2c);	  
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|1); //进入接收模式	
	bsp_software_i2c_wait_ack(soft_i2c);
  	for(count=0;count<length;count++)
	{
		if(count!=(length-1))
		temp = bsp_software_i2c_read_byte(soft_i2c,1); //带ACK的读数据
		else  
		temp = bsp_software_i2c_read_byte(soft_i2c,0); //最后一个字节NACK

		data[count] = temp;
	}
    bsp_software_i2c_stop(soft_i2c); //产生一个停止条件
    //return count;
	 return 0;
}

/******************************************************************************
*函  数：uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
*功　能：将多个字节写入指定设备 指定寄存器
*参  数：dev     目标设备地址
         reg    寄存器地址
         length  要写的字节数
        *data   要写入的数据将要存放的指针
*返回值：1成功 0失败
*备  注：无
*******************************************************************************/ 
uint8_t bsp_software_i2c_mem_write(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr, uint8_t reg, uint8_t length, uint8_t* data)
{
  
 	uint8_t count = 0;
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|0); //发送从机地址
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c);
		return 1; //从机地址写入失败
	}
	bsp_software_i2c_send_byte(soft_i2c,reg); //发送寄存器地址
  	bsp_software_i2c_wait_ack(soft_i2c);	  
	for(count=0;count<length;count++)
	{
		bsp_software_i2c_send_byte(soft_i2c,data[count]); 
		if(bsp_software_i2c_wait_ack(soft_i2c)) //每一个字节都要等从机应答
		{
			bsp_software_i2c_stop(soft_i2c);
			return 1; //数据写入失败
		}
	}
	bsp_software_i2c_stop(soft_i2c); //产生一个停止条件

	return 0;
}

/******************************************************************************
*函  数：uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data)
*功　能：读取指定设备 指定寄存器的 length个值
*参  数：dev     目标设备地址
		     reg	   寄存器地址
         length  要读的字节数
		     *data   读出的数据将要存放的指针
*返回值：1成功 0失败
*备  注：无
*******************************************************************************/ 
uint8_t bsp_software_i2c_mem16_read(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr, uint16_t reg, uint8_t length, uint8_t *data)
{
  uint8_t count = 0;
	uint8_t temp;
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|0); //发送从机地址
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c); 
		return 1; //从机地址写入失败
	}
	bsp_software_i2c_send_byte(soft_i2c,reg>>8); //发送寄存器地址
  	bsp_software_i2c_wait_ack(soft_i2c);	
	bsp_software_i2c_send_byte(soft_i2c,reg&0XFF); //发送寄存器地址
  	bsp_software_i2c_wait_ack(soft_i2c);
    
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|1); //进入接收模式	
	bsp_software_i2c_wait_ack(soft_i2c);
  	for(count=0;count<length;count++)
	{
		if(count!=(length-1))
		temp = bsp_software_i2c_read_byte(soft_i2c,1); //带ACK的读数据
		else  
		temp = bsp_software_i2c_read_byte(soft_i2c,0); //最后一个字节NACK

		data[count] = temp;
	}
    bsp_software_i2c_stop(soft_i2c); //产生一个停止条件
    //return count;
	 return 0;
}

/******************************************************************************
*函  数：uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
*功　能：将多个字节写入指定设备 指定寄存器
*参  数：dev     目标设备地址
         reg    寄存器地址
         length  要写的字节数
        *data   要写入的数据将要存放的指针
*返回值：1成功 0失败
*备  注：无
*******************************************************************************/ 
uint8_t bsp_software_i2c_mem16_write(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr, uint16_t reg, uint8_t length, uint8_t* data)
{
  
 	uint8_t count = 0;
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|0); //发送从机地址
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c);
		return 1; //从机地址写入失败
	}
	bsp_software_i2c_send_byte(soft_i2c,reg>>8); //发送寄存器地址
  	bsp_software_i2c_wait_ack(soft_i2c);	

	bsp_software_i2c_send_byte(soft_i2c,reg&0XFF); //发送寄存器地址
  	bsp_software_i2c_wait_ack(soft_i2c);
    
	for(count=0;count<length;count++)
	{
		bsp_software_i2c_send_byte(soft_i2c,data[count]); 
		if(bsp_software_i2c_wait_ack(soft_i2c)) //每一个字节都要等从机应答
		{
			bsp_software_i2c_stop(soft_i2c);
			return 1; //数据写入失败
		}
	}
	bsp_software_i2c_stop(soft_i2c); //产生一个停止条件

	return 0;
}



