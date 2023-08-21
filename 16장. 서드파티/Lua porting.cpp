/*
����: ũ�ν� �÷��� ���α׷����� ���� �̱����� ���̸� �����ϴ� ������ �۾�. ���, JSON�ļ�, eazyzlib�� ��� 
��� ���� �� ���ǻ���: ǥ�� ����� ó��, ��� ������, ���ڿ� ��� ���� 
*/

//1. ���� ����� �������̽����� ������ �ȵǾ����� ���Ǵ� �����ξ� �ڵ尡 �����ϰ� �Ѵ�.
FILE* g_stdOut=nullptr;
FILE* g_stdIn=nullptr;
FILE* g_stdErr=nullptr;

StorageManager::StorageManager(){
	memset(m_fileSystems, 0, sizeof(FileSysAdaptor*)*STORAGE_DEVICE_MAX);
	m_storageCount=0;
	m_pCurrentFileSystem=nullptr;
	
	g_stdOut=new FILE;//���� ��ü�� ���� 
	g_stdIn=new FILE;
	strcpy(g_stdOut->_name, "STDOUT");
	strcpy(g_stdIn->_name, "STDIN");
	strcpy(g_stdErr->_name, "STDERR");
} 

static void closefile(FILE* f){
	if(f!=g_stdIn && f!=g_stdOut){//���� �ݱ����� Ȯ���ڵ� �߰� 
		int tag=gettag();
		CLOSEFILE(f);
		lua_pushusertag(f, tag);
		lua_setting(CLOSEDTAG(tag));
	}
}

int StorageManager::WriteFile(PFILE file, unsigned char* buffer, unsigned int size, int count){
	if(strcmp(file->_name, "STDOUT")==0){//���Ͽ� ����Ϸ��� �ϸ� �׳� �ֿܼ� ��� 
		SkyConsole::Print("%s", buffer);
		return size*count;
	}
	.....
	int len=m_pCurrentFileSystem->Write(file, buffer, size, count);
}
//������� �ٷ�� liolib.cpp ���Ͽ��� �������� ���� ������ ��� �ּ�ó��, %f����� ������ %3f���� ������ ��� ������ �ȵ��ֱ⿡ �ܼ�ȭ�Ѵ�.

void TestLua(){
	lua_open();
	lua_pushString("> ");
	lua_setglobal("_PROMPT");
	lua_userinit();
	
	int result=lus_dofile("fact.lua");
	SkyConsole::Print("Lua Exec Result: %d", result);
} 

//�ܼ� ��ɾ lua��� �߰�
long cmdLua(char* theCommand){
	static bool init=false;
	if(init==false){//�ʱ�ȭ�� �ѹ��� �����Ѵ� 
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
