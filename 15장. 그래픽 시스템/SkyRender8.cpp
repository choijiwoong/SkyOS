/*
�������� ������Ʈ�� ������� �������� ����ϴ� �����̴�. �Ʒ��� �������̽�	 
*/
void SkyRender8::InitPalette(){
	static unsigned char table_rgb[16*3]={
		0x00, 0x00, 0x00, //0: ��
		0xff, 0x00, 0x00, //1: ��������
		.....
		0x84, 0x84, 0x84 //15: ��ο� ȸ�� 
	}
}

void SkyRenderer8::PutFont(char* vram, int xsize, int x, int y, char c, char* font){//���ڸ� ȭ�鿡 ��� 
	int i;
	char* p, d;
	
	for(i=0; i<16; i++){//������ ���� ������ġ���� �÷����� ����Ѵ�. 
		p=vram(y+i)*xsize+x;//���ڸ� ����� ���� ��ġ�� ã�´�
		d=font[i];//��Ʈ�� 1����Ʈ ���� �����͸� ��´�
		if((d&0x80)!=0){ p[0]=c; }//1����Ʈ�� �� ��Ʈ���� �׽�Ʈ�Ͽ� 0�� �ƴϸ� �÷����� �ȼ��� ��ִ´�. 
		if((d&0x40)!=0){ p[1]=c; }
		.....
		if((d&0x01)!=0){ p[7]=c; }
	}
	return; 
}

class SkyRenderer{
	.....
	virtual bool Initialize()=0;
	virtual void MakeTextBox(SkySheet *sht, int x0, int y0, int sx, int sy, int c)=0;
	virtual void MakeWindow(unsigned char *buf, int xsize, int ysize, char* title, char act)=0;
	virtual void InitMouseCursor(char* mouse, char bc)=0;
	virtual void InitScreen(unsigned char *vram, int x, int y)=0;
	virtual void PutFont(char* vram, int xsize, int x, int y, char c, char* font)=0;
	virtual void MakeWindowTitle(unsigned char* buf, int xsize, char* title, char act)=0;
	virtual void BoxFill(unsigned char* vram, int xsize, int color, int x0, int y0, int x1, int y1)=0;
};

 
