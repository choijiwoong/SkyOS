/*
�Լ� ���ڷ� STL��Ʈ���� �ƴ� char*�̾���Ѵ�. 
*/

class I_MapFileReader{
	public:
		virtual int readFile(char* fileName)=0;
		virtual void setLoadAddress(DWORD loadAddress)=0;
		virtual int getAddressInfo(DWORD address, char* module, char* fileName, int& lineNumber, char* function, DOWRD& resultAddress)=0;
};

class MapFileReader: public I_MapFileReader{
	public:
		MAPFILEDLL_API MapFileReader(SKY_FILE_Interface, fileInterface, char* fileName);
		MAPFILEDLL_API int getAddressInfo(.....);
		.....
		
	protected:
		int readDLLName(FILE* fp);
		int readTimeStamp(FILE* fp);
		int readPreferredLoadAddress(FILE* fp);
		.....
		
	private:
		void flushSymbols(std::vector<MapFileSymbol*> &syms);
		.....
	
	private:
		std::string		 				filename;
		std::vector<MapFileSection*> 	sections;
		std::vector<MapFileSymbol*>		publicSymbols;
		std::vector<MapFileSymbol*>		staticSymbols;
		std::vector<MapFileImport*>		imports;
		std::vector<MapFileExport*>		exports;
		.....
}

int MapFileReader::readFile(char* filename){
	FILE* fp;
	int r=TRUE;
	
	fp=g_mockInterface.g_fileInterface.sky_fopen(fileName, "r");
	if(fp!=NULL){
		r=readDLLName(fp);//DLL�̸� 
		r=readTimeStamp(fp);//�����ð� 
		r=readPreferredLoadAddress(fp);//�̹������̽� 
		r=readSectionInformation(fp);//�������� 
		r=readPublicSymbols(fp);//�ɹ����� 
		r=readEntryPoint(fp);//��Ʈ���ּ� 
		.....
	}
	
	g_mockInterface.g_printInterface.sky_printf("succedd to open map file %s\n", fileName);
	
	return r;
}
