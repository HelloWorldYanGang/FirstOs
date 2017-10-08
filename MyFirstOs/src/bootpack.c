#include "bootpack.h"
#include <stdio.h>

#define KEY_DOWN  1
#define KEY_UP    2

extern struct buffer keybuf;
extern struct buffer mousebuf;

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
	char key_buf_data[32];
	char mouse_buf_data[128];//鼠标数据较键盘的更多，因此缓冲区设置的大一些
	buffer_init(&keybuf, 32, key_buf_data);
	buffer_init(&mousebuf, 128, mouse_buf_data);
	init_gdtidt();
	init_pic();
	io_sti();

	init_KB();

	init_palette();//初始化调色板
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

	int mouse_pos_x, mouse_pos_y;
	mouse_pos_x = (binfo->scrnx - 16 ) / 2;
	mouse_pos_y = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mouse_data, COL8_008484);
	show_graph_in_background(binfo->vram, binfo->scrnx, 0, 0, mouse_pos_x, mouse_pos_y, mouse_data, 16);

	io_out8(PIC0_IMR, 0xf9); //11111001b  IRQ1 IRQ2不屏蔽
	io_out8(PIC1_IMR, 0xef); //11101111b  IRQ12不屏蔽

	struct MOUSE_DESC mouse_desc;
	enable_mouse(&mouse_desc);

	
	for(;;)
	{
		io_cli();
		if(buffer_used_size(&keybuf) + buffer_used_size(&mousebuf) > 0)
		{
			unsigned char data;
			char s[15];
			if(buffer_used_size(&keybuf) > 0)  //键盘数据
			{
				buffer_get(&keybuf, &data);
				io_sti();
				sprintf(s, "%02X", data);
				boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
				printstring(binfo->vram, binfo->scrnx, 0,16, COL8_FFFFFF, s);
			}
			else if(buffer_used_size(&mousebuf) > 0)  //鼠标数据
			{
				buffer_get(&mousebuf, &data);
				io_sti();
				/*解读鼠标数据、并显示*/
				if(mouse_data_decode(&mouse_desc, data) == 1)
				{
					//三个字节凑齐，显示出来
					sprintf(s, "[lcr %4d %4d]", mouse_desc.x, mouse_desc.y);
					
					if((mouse_desc.btn & 0x01) != 0)//鼠标左键被按下时显示"L"
					{
						s[1] = 'L';
					}
					if((mouse_desc.btn & 0x02) != 0)//鼠标右键被按下时显示"R"
					{
						s[2] = 'R';
					}
					if((mouse_desc.btn & 0x04) != 0)//鼠标中键被按下时显示"C"
					{
						s[3] = 'C';
					}
					boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8-1 , 31);
					printstring(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
					/*鼠标指针的移动*/
					boxfill8(binfo->vram, binfo->scrnx, COL8_008484, mouse_pos_x, mouse_pos_y, mouse_pos_x + 15, mouse_pos_y + 15);//隐藏鼠标
					mouse_pos_x += mouse_desc.x;
					mouse_pos_y += mouse_desc.y;
					if(mouse_pos_x < 0)
						mouse_pos_x = 0;
					if(mouse_pos_y < 0)
						mouse_pos_y = 0;
					if(mouse_pos_x > binfo->scrnx - 16)
						mouse_pos_x = binfo->scrnx - 16;
					if(mouse_pos_y > binfo->scrny - 16)
						mouse_pos_y = binfo->scrny - 16;
					sprintf(s, "[%3d, %3d]", mouse_pos_x, mouse_pos_y);
					boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 0, 79, 15);
					printstring(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
					show_graph_in_background(binfo->vram, binfo->scrnx, 16, 16, mouse_pos_x, mouse_pos_y, mouse_data, 16);
				}
			}
			
		}
		else
		{
			io_stihlt();//没有按键按下，打开中断hlt，等待有中断被唤醒
		}

	}	
}

