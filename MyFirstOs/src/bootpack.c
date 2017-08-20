#include "bootpack.h"

/*主函数*/
void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *)(0x0ff0);
	char mouse_data[256];
	init_palette();//初始化调色板

	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
	
	printstring(binfo->vram, binfo->scrnx, 8, 8, COL8_FF0000, "aaa");

	int mouse_pos_x, mouse_pos_y;
	mouse_pos_x = (binfo->scrnx - 16 ) / 2;
	mouse_pos_y = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mouse_data, COL8_008484);
	show_graph_in_background(binfo->vram, binfo->scrnx, 16, 16, mouse_pos_x, mouse_pos_y, mouse_data, 16);
	for(;;)
		io_hlt();
}

