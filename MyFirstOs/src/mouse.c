#include "bootpack.h"


struct buffer mousebuf;

//激活鼠标
void enable_mouse(struct MOUSE_DESC *mouse_desc)
{
	wait_KB_ready();
	io_out8(PORT_KB_CMD, SENDTO_MOUSE);
	wait_KB_ready();
	io_out8(PORT_KB_DATA, MOUSE_ENABLE);
	//正常，0xfa会收到
	mouse_desc->phase = 0;
	return ;
}

//鼠标数据解读，当三个一组的数据凑齐之后，返回1，否则返回-1
int mouse_data_decode(struct MOUSE_DESC *mouse_desc, unsigned char data)
{
	if(mouse_desc->phase == 0)
	{
		if(data == 0xfa)
			mouse_desc->phase = 1;
		return 0;
	}
	//第一个字节，0xab:a只是在0-3之间变化  b在8-F变化
	if(mouse_desc->phase == 1)
	{
		if((data & 0xc8) == 0x08) //如果第一字节正确
		{
			mouse_desc->buf[0] = data;
			mouse_desc->phase = 2;
		}
		
		return 0;
	}
	//第二个字节，与鼠标左右移动有关
	if(mouse_desc->phase == 2)
	{
		mouse_desc->buf[1] = data;
		mouse_desc->phase = 3;
		return 0;
	}
	//第三个字节，与鼠标上下移动有关
	if(mouse_desc->phase == 3)
	{
		mouse_desc->buf[2] = data;
		mouse_desc->phase = 1;
		mouse_desc->btn = mouse_desc->buf[0] & 0x07;//取出低三位为鼠标键的状态
		mouse_desc->x = mouse_desc->buf[1];
		mouse_desc->y = mouse_desc->buf[2];
		if((mouse_desc->buf[0] & 0x10) != 0)
		{
			mouse_desc->x |= 0xffffff00;
		}
		if((mouse_desc->buf[0] & 0x20) != 0)
		{
			mouse_desc->y |= 0xffffff00;
		}
		mouse_desc->y = -mouse_desc->y;
		return 1;
	}
	return -1;
}

//来自鼠标的中断
void int_handler_2c(int *esp)
{
	unsigned char data;
	io_out8(PIC1_OCW2, 0x64);   /*以0x60+IRQn的方式通知PIC1的IRQn号中断已经被受理，可以继续监听*/
	io_out8(PIC0_OCW2, 0x62);    /*以0x60+IRQn的方式通知PIC0的IRQn号中断已经被受理，可以继续监听*/
	data = io_in8(PORT_KEYBOARD_DATA);
	buffer_put(&mousebuf, data);
	return;
}