; haribote-os
; TAB=4
		ORG		0xc200

		MOV		AL,0x13		;VGA显卡320*200*8位真彩色
		MOV		AH,0x00	;设定显卡模式
		INT		0x10
fin:
		HLT
		JMP 	fin