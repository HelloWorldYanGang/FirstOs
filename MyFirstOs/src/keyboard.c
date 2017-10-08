#include "bootpack.h"

struct buffer keybuf;  //键盘按键编码缓存

//等待键盘控制电路可以接受CPU指令
void wait_KB_ready()
{
	for(;;)
		if((io_in8(PORT_KB_STA) & 0x02) == 0)  //倒数第二位是0 表示ready
		{
			break;
		}
	return;
}
//初始化键盘控制电路
void init_KB()
{
	wait_KB_ready();
	io_out8(PORT_KB_CMD, PORT_MODE_SET);
	wait_KB_ready();
	io_out8(PORT_KB_DATA, MOUSE_MODE);
	return ;
}

//来自键盘的中断
void int_handler_21(int *esp)
{
	unsigned char key_data;
	io_out8(PIC0_OCW2, 0x61);  /*以0x60+IRQn的方式通知PIC0的IRQn号中断已经被受理，可以继续监听*/
	key_data = io_in8(PORT_KEYBOARD_DATA);
	buffer_put(&keybuf, key_data);
	return;
}