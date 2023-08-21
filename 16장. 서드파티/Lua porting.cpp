/*
포팅: 크로스 플랫폼 프로그래밍을 위해 이기종간 차이를 수정하는 별도의 작업. 루아, JSON파서, eazyzlib을 사용 
루아 포팅 시 유의사항: 표준 입출력 처리, 언어 로케일, 문자열 출력 포맷 
*/

//1. 파일 입출력 인터페이스에서 구현이 안되었더라도 정의는 만들어두어 코드가 동작하게 한다.
FILE* g_stdOut=nullptr;
FILE* g_stdIn=nullptr;
FILE* g_stdErr=nullptr;

StorageManager::StorageManager(){
	memset(m_fileSystems, 0, sizeof(FileSysAdaptor*)*STORAGE_DEVICE_MAX);
	m_storageCount=0;
	m_pCurrentFileSystem=nullptr;
	
	g_stdOut=new FILE;//파일 객체로 선언 
	g_stdIn=new FILE;
	strcpy(g_stdOut->_name, "STDOUT");
	strcpy(g_stdIn->_name, "STDIN");
	strcpy(g_stdErr->_name, "STDERR");
} 

static void closefile(FILE* f){
	if(f!=g_stdIn && f!=g_stdOut){//파일 닫기전에 확인코드 추가 
		int tag=gettag();
		CLOSEFILE(f);
		lua_pushusertag(f, tag);
		lua_setting(CLOSEDTAG(tag));
	}
}

int StorageManager::WriteFile(PFILE file, unsigned char* buffer, unsigned int size, int count){
	if(strcmp(file->_name, "STDOUT")==0){//파일에 출력하려고 하면 그냥 콘솔에 출력 
		SkyConsole::Print("%s", buffer);
		return size*count;
	}
	.....
	int len=m_pCurrentFileSystem->Write(file, buffer, size, count);
}
//입출력을 다루는 liolib.cpp 파일에서 구현하지 않은 내용은 모두 주석처리, %f출력은 되지만 %3f같은 복잡한 출력 구현이 안돼있기에 단순화한다.

void TestLua(){
	lua_open();
	lua_pushString("> ");
	lua_setglobal("_PROMPT");
	lua_userinit();
	
	int result=lus_dofile("fact.lua");
	SkyConsole::Print("Lua Exec Result: %d", result);
} 

//콘솔 명령어세 lua명령 추가
long cmdLua(char* theCommand){
	static bool init=false;
	if(init==false){//초기화는 한번만 수행한다 
		lua_open();
		lua_pushstring("> ");
		lua_setglobal("_PROMPT");
		lua_userinit();
		init=true;
	}
	int result=lua_dofile(theCommand);
	if(result!=0)
		SkyConsole::Print("Lua Exec Result: %d\n", result);
	return false;
} 
