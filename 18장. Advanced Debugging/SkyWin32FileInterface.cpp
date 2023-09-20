//����°����Լ� 
size_t sky_fread(void* ptr, size_t size, size_t nmemb, FILE* stream){
	return fread(ptr, size, nmemb, stream);
}

FILE* sky_fopen(const char* filename, const char* mode){
	return fopen(filename, mode);
}
...
SKY_FILE_Interface g_FileInterface={//����� �������̽� 
	sky_fread,
	sky_fopen,
	...
};

//ȭ����°����Լ� 
unsigned int sky_kmalloc(unsigned int sz){
	if(sz==0)
		return 0;
	return (unsigned int)new char[sz];
}

void sky_printf(const char* str, ...){
	char* p=new char[128];
	va_list ap;
	
	va_start(ap, str);
	(void)vsnprintf(p, 128, str, ap);//���� p�� ���ڵ� �μ� 
	va_end(ap);
	
	printf(p);//p��� 
	delete p;
}

Sky_Print_Interface g_printInterface={
	sky_printf,
};
