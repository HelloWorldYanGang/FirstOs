#include "bootpack.h"

void init_gdtidt(void)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADDR_GDT;
	struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *) ADDR_IDT;
	int i;
	//初始化gdt
	for(i = 0; i <= LIMIT_GDT / 8; i++)
	{
		set_segment_desc(gdt+i, 0, 0, 0);
	}
	//段号为1的段：4GB 数据段
	set_segment_desc(gdt+1, 0xffffffff, 0x00000000, AR_DATA_RW);
	//段号为2的段： 代码段
	set_segment_desc(gdt+2, 0x0007ffff, 0x00280000, AR_CODE_ER);
	//调用LGDT命令装载gdt的地址
	load_gdtr(LIMIT_GDT,  ADDR_GDT);

	//idt初始化
	for (i = 0; i < LIMIT_IDT / 8; i++)
	{
		set_gate_desc(idt+i, 0, 0, 0);
	}
	load_idtr(LIMIT_IDT, ADDR_IDT);

	//设置键盘的中断回调函数
	set_gate_desc(idt + 0x21, (int) asm_int_handler_21, 2 << 3,  AR_INTGATE);

	return ;
}

void set_segment_desc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
	if(limit > 0xfffff)
	{
		ar |= 0x8000; //G_bit=1
		limit /= 0x1000; // limit / 4KB
	}
	sd->limit_low = limit & 0xffff;
	sd->base_low = base & 0xffff;
	sd->base_mid = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high    = (base >> 24) & 0xff;
	return ;
}

void set_gate_desc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
	gd->offset_low = offset & 0xffff;
	gd->selector = selector;
	gd->dw_count = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high = ( offset >> 16 ) & 0xffff;
	return ;
}
