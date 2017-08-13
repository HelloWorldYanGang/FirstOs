; hello-os
; TAB=4
		CYLS	EQU		10
		ORG		0x7c00			; 程序装载

; FAT12软盘描述

        JMP       entry
        DB        0x90
        DB        "HARIBOTE"       
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
        DB        "HARIBOTEOS "    
        DB        "FAT12   "        
        RESB      18                


;核心程序

entry:
		MOV		AX,0			;初始化寄存器
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
		
		;磁盘内容加载到0x8200的位置,0x0820左移4位就是0x8200
		;由于我们是从扇区2开始加载，相当于整个磁盘加载到0x8000地址
		MOV		AX,0x0820
		MOV		ES,AX			;[ES:BX]为缓冲地址
		MOV		CH,0			;柱面0
		MOV		DH,0			;磁头0
		MOV		CL,2			;扇区2
		
readloop:
		MOV		SI,0			;记录失败次数
retry:
		MOV		AH,0x02			;读盘
		MOV		AL,1			;1个扇区
		MOV		BX,0
		MOV		DL,0x00			;A驱动器
		INT		0x13			;磁盘BIOS
		JNC		next
		ADD		SI,1			;出错往SI加1
		CMP		SI,5			;比较SI与5
		JAE		error			;SI>=5时 跳转到error
		;复位软盘状态，再读一次
		MOV		AH,0x00
		MOV		DL,0x00
		INT		0x13
		JMP		retry
next:
		MOV		AX,ES			;把内存地址后移0x0020
		ADD		AX,0x0020
		MOV		ES,AX
		ADD		CL,1			;扇区数+1
		CMP		CL,18			;是否到18个扇区
		JBE		readloop		;读下一个扇区
		
		MOV 	CL,1			;该柱面读完读下个柱面时，扇区重新值1
		ADD		DH,1			;两个磁头
		CMP		DH,2
		JB		readloop
		MOV		DH,0
		ADD		CH,1
		CMP		CH,CYLS			;柱面
		JB		readloop

;由于进入32位模式之后可以使用大于1M的内存，这里记录下已读的柱面数
;实际上记录了磁盘装载内容的结束地址，为进入32位模式做准备
		MOV		[0x0ff0],CH
;跳转到0xc200
		JMP		0xc200

error:
		MOV		SI,msg
putloop:
		MOV		AL,[SI]         ; AL装载要显示的字符
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
		DB		"load error"
		DB		0x0a			; 
		DB		0

		RESB	0x7dfe-$		; 

		DB		0x55, 0xaa
