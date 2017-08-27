#include "bootpack.h"
#include <stdio.h>

struct buffer keybuf;

void init_pic(void)
{
	io_out8(PIC0_IMR,  0xff  ); /* 禁止所有中断 */
	io_out8(PIC1_IMR,  0xff  ); /* 禁止所有中断 */

	io_out8(PIC0_ICW1, 0x11  ); /* 边沿触发模式 */
	io_out8(PIC0_ICW2, 0x20  ); /* IRQ0-7由INT20-27 */
	io_out8(PIC0_ICW3, 1 << 2); /* PIC1由IRQ2连接 */
	io_out8(PIC0_ICW4, 0x01  ); /* 无缓冲区模式 */

	io_out8(PIC1_ICW1, 0x11  ); /* 边沿触发模式 */
	io_out8(PIC1_ICW2, 0x28  ); /* IRQ8-15由INT28-2f */
	io_out8(PIC1_ICW3, 2     ); /* PIC1由IRQ2连接 */
	io_out8(PIC1_ICW4, 0x01  ); /* 无缓冲区模式 */

	io_out8(PIC0_IMR,  0xfb  ); /* 11111011 PIC1以外全部禁止 */
	io_out8(PIC1_IMR,  0xff  ); /* 11111111 禁止所有中断*/
	return ;
}

void int_handler_21(int *esp)
{
	unsigned char key_data;
	io_out8(PIC0_OCW2, 0x61);  /*以0x60+IRQn的方式通知PIC0的IRQn号中断已经被受理，可以继续监听*/
	key_data = io_in8(PORT_KEYBOARD_DATA);
	if(buffer_put(&keybuf, key_data) == 0)
	{
		return;
	}
	return;
}
