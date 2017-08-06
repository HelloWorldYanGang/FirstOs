; hello-os
; TAB=4

		ORG		0x7c00			; 程序装载

; FAT12软盘描述

        JMP        entry
        DB        0x90
        DB        "HELLOIPL"       
        DW        512                
        DB        1               
        DW        1                
        DB        2                
        DW        224                
        DW        2880            
        DB        0xf0            
        DW        9                
        DW        18                
        DW        2                
        DD        0                
        DD        2880            
        DB        0,0,0x29        
        DD        0xffffffff        
        DB        "HELLO-OS   "    
        DB        "FAT12   "        
        RESB    18                


;核心程序

entry:
		MOV		AX,0			;初始化寄存器
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
		MOV		ES,AX

		MOV		SI,msg
putloop:
		MOV		AL,[SI]         ;AL装载要显示的字符
		ADD		SI,1			; 每次显示一个字符
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 设置显示字符
		MOV		BX,15			; 设置颜色
		INT		0x10			; 调用16号中断  BIOS显卡显示
		JMP		putloop
fin:
		HLT						; 
		JMP		fin				; 

msg:
		DB		0x0a, 0x0a		; 换行2次
		DB		"hello, world"
		DB		0x0a			; 
		DB		0

		RESB	0x7dfe-$		; 

		DB		0x55, 0xaa
