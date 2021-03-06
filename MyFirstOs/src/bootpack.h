/*asmhead.h*/
struct BOOTINFO
{
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};
#define ADDR_BOOTINFO 0x00000ff0

/*naskfunc.nas*/
void io_hlt(void);
void io_cli(void);
void io_sti(void);
unsigned io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void write_mem8(int addr, int data);
//执行LGDT命令
void load_gdtr(int limit, int addr);
//执行LLDT命令
void load_idtr(int limit, int addr);

int memtest_sub(unsigned int  start, unsigned int end);
void asm_int_handler_21(void);
void asm_int_handler_2c(void);

int load_cr0();
void store_cr0(int cr0);
/*graph.c*/

//初始化调色板
void init_palette(void);
//设定调色板颜色
void set_palette(int start, int end, unsigned char *rgb);
//矩形颜色填充
void boxfill8(unsigned char *vram, int xsize, unsigned char color, int x0, int y0, int x1, int y1);
//初始化屏幕显示
void init_screen(char *vram, int xsize, int ysize);
//显示一个字符，颜色为c，在字库中地址为hankaku+（ascii）*16
void printfont(char *vram, int xsize, int x, int y, char c, char *font);
//显示字符串
void printstring(char *vram, int xsize, int x, int y, char c, char *str);
//初始化鼠标指针
void init_mouse_cursor8(char *mouse, char bc);
//设置图形在背景中显示，图形由buf定义，bxsize为宽度
void show_graph_in_background(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize);

//定义调色板中的颜色代号
#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

/*destbl.c*/

//段描述符  8字节
/*
|   7    |     6       |     5     |   4    |   3    |   2    |   1    |   0    |  字节
|76543210|7 6 5 4 3210 |7 65 4 3210|76543210|76543210|76543210|76543210|76543210|  比特
|--------|-|-|-|-|---- |-|--|-|----|--------|--------|--------|--------|--------|  占位
|  BASE  |G|D|0|A|LIMIT|P|D |S|TYPE|<------- BASE 23-0 ------>|<-- LIMIT 15-0 ->|  含义
|  31-24 | |/| |V|19-16| |P |
           |B| |L|     | |L |

BASE: 段基址，由上图中的两部分(BASE 31-24 和 BSE23-0)组成
G：LIMIT的单位，该位 0 表示单位是字节，1表示单位是 4KB
D/B: 该位为 0 表示这是一个 16 位的段，1 表示这是一个 32 位段
AVL: 该位是用户位，可以被用户自由使用
LIMIT: 段的界限，单位由 G 位决定。数值上（经过单位换算后的值）等于段的长度（字节）- 1。
P: 段存在位，该位为 0 表示该段不存在，为 1 表示存在。
DPL：段权限
S: 该位为 1 表示这是一个数据段或者代码段。为 0 表示这是一个系统段（比如调用门，中断门等）
TYPE: 根据 S 位的结果，再次对段类型进行细分。详见：http://www.cnblogs.com/yangang92/p/7396968.html
*/
struct SEGMENT_DESCRIPTOR
{
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

//中断描述符表
/*
|   7    |     6  |     5     |   4    |   3    |   2    |   1    |   0    |  字节
|76543210|76543210|7 65 4 3210|76543210|76543210|76543210|76543210|76543210|  比特
|--------|--------|-|--|-|----|--------|--------|--------|--------|--------|  占位
|<--offset31-16-->|P|D |S|TYPE|<---0-->|----selector --->|<-- offset15-0 ->|  含义
|                   |P |
                  | |L |

offset:中断处理函数的地址32位
selector：中断处理函数的所在的段选择子
Type：门类型，这里是中断门  1110b
S：类似gdt该位，中断门为0
DPL：段权限
P：设置为0代表未被使用的中断
 */
struct GATE_DESCRIPTOR
{
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};

//初始化gdt idt
void init_gdtidt(void);
//设定gdt
void set_segment_desc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
//设定ldt
void set_gate_desc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

#define ADDR_GDT     0x00270000
#define ADDR_IDT     0x0026f800
#define LIMIT_GDT    0x0000ffff
#define LIMIT_IDT    0x000007ff
#define AR_DATA_RW   0x4092
#define AR_CODE_ER   0x409a
#define AR_INTGATE   0x008e

/* int.c */

#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

//键盘按键编码端口号
#define PORT_KEYBOARD_DATA  0x0060

void init_pic(void);



/*buffer.c*/
#define FLAG_OVERFLOW     0x0001
struct buffer
{
	unsigned char *data; //数据缓存区
	int next_write;  //下一个加入buffer的位置
	int next_read;//下一个读出buffer的位置
	int size;     //buffer总空间
	int free_size;  //buffer空闲大小
	int flag;   //标记，是否溢出
};

//缓存区初始化
void buffer_init(struct buffer *buffer, int size, unsigned char *buf);

//往缓存区写入一个数据,成功返回0，失败返回-1
int buffer_put(struct buffer *buffer, unsigned char data);

//往缓存区读取一个数据,成功返回0，失败返回-1
int buffer_get(struct buffer *buffer, unsigned char *ret_data);

//计算剩余空间
int buffer_used_size(struct buffer *buffer);

/*mouse.c  keyboard.c*/

#define PORT_KB_STA    0x0064 //键盘控制电路状态读取端口
#define PORT_KB_CMD    0x0064 //键盘控制电路指令写入端口
#define PORT_KB_DATA   0x0060 //键盘控制电路数据写入端口
#define PORT_MODE_SET  0x60   //键盘控制电路模式设定
#define MOUSE_MODE     0x47   //鼠标模式
#define SENDTO_MOUSE   0xd4   //将数据发送给鼠标
#define MOUSE_ENABLE   0xf4   //激活鼠标


//记录鼠标中断的三个一组的数据buf和当前已经从中断中获取到一组中的第几个数据phase
//用于记录鼠标的阶段，最初是0xfa，随后三个字节一组，用于描述鼠标的移动数据
struct MOUSE_DESC
{
	unsigned char buf[3], phase; 
	int x, y, btn;
};

void enable_mouse(struct MOUSE_DESC *mouse_desc);

int mouse_data_decode(struct MOUSE_DESC *mouse_desc, unsigned char data);

//鼠标中断函数
void int_handler_2c(int *esp);

//键盘中断函数
void int_handler_21(int *esp);
