SECTION .text
extern kmain64; C++Ŀ�� ��Ʈ������Ʈ 
global main64; PE���� ��Ʈ������Ʈ 

main64:
	; IA-32e��� Ŀ�� ������ ���׸�Ʈ ��ũ���� ����. 64bit�� �����ϸ� Ŀ�� ���׸�Ʈ ��ũ���Ͱ� 0x08�����̴�. 
	mov ax, 0x10
	mov ds, ax
	mov ed, ax
	mov fs, ax
	mov gs, ax
	
	; ���ü��� 0x500000~0x600000 
	mov ss, ax
	mov rsp, 0x600000
	mov rbo, 0x6000000
	
	call kmain64
	
	jmp $
