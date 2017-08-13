/*告诉编译器，有一个函数在别的文件里*/
void io_hlt(void);

void HariMain(void)
{
fin:
	io_hlt();
	goto fin;
}