#include "bootpack.h"
#include <stdio.h>

#define KEY_DOWN  1
#define KEY_UP    2

extern struct buffer keybuf;

//键盘键码与字符对应关系
static char keytable[0x54] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.'
	};

/*主函数*/
void HariMain(void)
{
	
	struct BOOTINFO *binfo = (struct BOOTINFO *)(ADDR_BOOTINFO);
	char mouse_data[256];
	char buf_data[32];
	unsigned char key_flag;
	buffer_init(&keybuf, 32, buf_data);
	init_gdtidt();
	init_pic();
	io_sti();

	init_palette();//初始化调色板
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

	int mouse_pos_x, mouse_pos_y;
	mouse_pos_x = (binfo->scrnx - 16 ) / 2;
	mouse_pos_y = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mouse_data, COL8_008484);
	show_graph_in_background(binfo->vram, binfo->scrnx, 0, 0, mouse_pos_x, mouse_pos_y, mouse_data, 16);

	io_out8(PIC0_IMR, 0xf9); //11111001b  IRQ1 IRQ2不屏蔽
	io_out8(PIC1_IMR, 0xef); //11101111b  IRQ12不屏蔽


	for(;;)
	{
		io_cli();
		if(buffer_used_size(&keybuf) > 0)
		{
			//先保存按键值再放开中断
			unsigned char ch;
			if(buffer_get(&keybuf, &ch) == 0)
			{
				io_sti();
				char str[2];
				if(ch < 0x80)  //键按下
				{
					key_flag = KEY_DOWN;
					str[0] = keytable[ch];
					str[1] = 0;
				}
				else          //键松开
				{
					key_flag = KEY_UP;
					str[0] = keytable[ch - 0x80];
					str[1] = 0;
				}
				boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 15, 150, 31);
				char msg[10];
				if(key_flag == KEY_DOWN)
				{
					sprintf(msg, "%s is down!", str);
					
				}
				else
				{
					sprintf(msg, "%s is up!", str);
				}
				printstring(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, msg);
			}
			
		}
		else
		{
			io_stihlt();//没有按键按下，打开中断hlt，等待有中断被唤醒
		}

	}	
}

