; naskfunc
; TAB=4

[FORMAT "WCOFF"]		;制作目标文件的模式							
[BITS 32]			    ;制作32位机器语言

;制作目标文件的信息
[FILE "naskfunc.nas"]			

		GLOBAL	_io_hlt		;程序中包含的函数名

;以下是实际的函数
[SECTION .text]

_io_hlt:	; void io_hlt(void);
		HLT
		RET