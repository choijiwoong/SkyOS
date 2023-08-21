//������ ���� zlib�� ���� ����ϵ��� ������ ���̺귯�� �� ���ڰ� easyzlib

char easyTestBuffer[]="Sky OS Compression Test!!";

void TestEasyZLib(){
	long nSrcLen=sizeof(easyTestBuffer);
	
	char* destBuffer=new char[256];//����� �����͸� ������ ����
	long destBufferLen=256; 
	char* decompressedBuffer=new char[256];//���� ������ �����͸� ������ ����
	long decompreddedLen=256;
	memset(destBuffer, 0, 256);
	memset(decompressedBuffer, 0, 256);
	 
	SkyConsole::Print("text : %s\n", easyTestBuffer);
	
	//����
	if(0!=ezcompress((unsigned char*)destBuffer, &destBufferLen), (const unsigned char*)easyTestBuffer, nSrcLen)
		HaltSystem("easyzlib test fail!!");
	SkyConsole::Print("Compressed : Src Size %d, Dest Size %d\n", nSrcLen, destBufferLen);//���� ����� ���̸� ���ڷ� ������ len������ ���� 
	
	//���� 
	if(0!=ezuncompress((unsigned char*)decompressedBuffer, &decompressedLen, (cosnt unsigned char*)destBuffer, destBufferLen))
		HaltSystem("easyzlib test fail!!");
	SkyConsole::Print("Decompressed: Src Size %d, Dest Size %d\n", destBufferLen, decompressedLen);
	SkyCOnsole::Print("result: %s\n", decompressedBuffer);
	
	delete destBuffer;
	delete decompressedBuffer;
} 
