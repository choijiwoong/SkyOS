//압축을 위한 zlib을 쉽게 사용하도록 수정한 라이브러리 중 한자가 easyzlib

char easyTestBuffer[]="Sky OS Compression Test!!";

void TestEasyZLib(){
	long nSrcLen=sizeof(easyTestBuffer);
	
	char* destBuffer=new char[256];//압축된 데이터를 저장할 버퍼
	long destBufferLen=256; 
	char* decompressedBuffer=new char[256];//압축 해제된 데이터를 저장할 버퍼
	long decompreddedLen=256;
	memset(destBuffer, 0, 256);
	memset(decompressedBuffer, 0, 256);
	 
	SkyConsole::Print("text : %s\n", easyTestBuffer);
	
	//압축
	if(0!=ezcompress((unsigned char*)destBuffer, &destBufferLen), (const unsigned char*)easyTestBuffer, nSrcLen)
		HaltSystem("easyzlib test fail!!");
	SkyConsole::Print("Compressed : Src Size %d, Dest Size %d\n", nSrcLen, destBufferLen);//압축 결과의 길이를 인자로 전달한 len변수에 저장 
	
	//해제 
	if(0!=ezuncompress((unsigned char*)decompressedBuffer, &decompressedLen, (cosnt unsigned char*)destBuffer, destBufferLen))
		HaltSystem("easyzlib test fail!!");
	SkyConsole::Print("Decompressed: Src Size %d, Dest Size %d\n", destBufferLen, decompressedLen);
	SkyCOnsole::Print("result: %s\n", decompressedBuffer);
	
	delete destBuffer;
	delete decompressedBuffer;
} 
