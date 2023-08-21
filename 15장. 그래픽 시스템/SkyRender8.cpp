/*
렌더링은 오브젝트를 장면으로 ㄱ구성해 출력하는 과정이다. 아래는 인터페이스	 
*/
void SkyRender8::InitPalette(){
	static unsigned char table_rgb[16*3]={
		0x00, 0x00, 0x00, //0: 흑
		0xff, 0x00, 0x00, //1: 밝은빨강
		.....
		0x84, 0x84, 0x84 //15: 어두운 회색 
	}
}

void SkyRenderer8::PutFont(char* vram, int xsize, int x, int y, char c, char* font){//문자를 화면에 출력 
	int i;
	char* p, d;
	
	for(i=0; i<16; i++){//루프를 돌며 버퍼위치마다 컬러값을 출력한다. 
		p=vram(y+i)*xsize+x;//문자를 출력할 버퍼 위치를 찾는다
		d=font[i];//폰트의 1바이트 가로 데이터를 얻는다
		if((d&0x80)!=0){ p[0]=c; }//1바이트의 각 비트들을 테스트하여 0이 아니면 컬러값을 픽셀에 써넣는다. 
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

 
