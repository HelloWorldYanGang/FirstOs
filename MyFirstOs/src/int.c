#include "bootpack.h"

void init_pic()
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
	struct BOOTINFO *binfo = (struct BOOTINFO *)(ADDR_BOOTINFO);
	boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32* 8 -1, 15);
	printstring(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "INT 21 keyboard");
	for(;;)
	{
		io_hlt();
	}
}