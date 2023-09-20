SECTION .text
extern kmain64; C++커널 엔트리포인트 
global main64; PE포멧 엔트리포인트 

main64:
	; IA-32e모드 커널 데이터 세그먼트 디스크립터 설정. 64bit로 점프하며 커널 세그먼트 디스크립터가 0x08상태이다. 
	mov ax, 0x10
	mov ds, ax
	mov ed, ax
	mov fs, ax
	mov gs, ax
	
	; 스택설정 0x500000~0x600000 
	mov ss, ax
	mov rsp, 0x600000
	mov rbo, 0x6000000
	
	call kmain64
	
	jmp $
