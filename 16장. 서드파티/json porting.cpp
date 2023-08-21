static const char* JSON_STRING;//json형식

void TestJson(){
	int i, r;
	jsmn_parser p;//파서 
	jsmntok_t t[128];//토큰개수제한 
	
	jsmn_init(&p);//파서 초기화 및 파싱 
	r=jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if(r<0){
		SkyConsole::Print("Failed to parse JSON: %d\n", r);
		return;
	}
	
	if(r<1 || t[0].type!=JSMN_OBJECT){
		SkyConsole::Print("Object expected\n")l
		return;
	}
	
	char buf[256];
	memset(buf, 0, 256);
	
	for(i=1; i<r; i++){//json에서 user admin uid groups정보를 찾아 값을 출력 
		if(jsoneq(JSON_STRING, &t[i], "user")==0){
			memcpy(buf, JSON_STRING+t+[i+1].start, t[i+1].end-t[i+1].start);//시작부터 크기만큼 buf에 복사하여 출력 
			SkyConsole::Print("- User: %s\n", buf);
			i++;
		} else if(jsoneq(JSON_STRING, &t[i], "admin")==0){
			SkyConsole;:Print("- Admin: %.*s\n", t[i+1].end-t[i+1].start, JSON_STRING+t[i+1].start);
			i++;
		} else if(jsoneq(JSON_STRING, &t[i], "uid")==0){
			SkyConsole;:Print("- UID: %.*s\n", t[i+1].end-t[i+1].start, JSON_STRING+t[i+1].start);
			i++;
		} else if(jsoneq(JSON_STRING, &t[i], "groups")==0){
			int j;
			SkyConsole::Print("- Groups:\n");
			if(t[i+1].type!=JSMN_ARRAY)
				continue;
			
			for(j=0; j<t[i+1].size; j++){
				jsmntok_t *g=&t[i+j+2];
				SkyConsole::Print("  * %.*s\n", g->end-g->start, JSON_STRING+g->start);
			}
			i+=t[i+1].size+1;
		} else{
			SkyConsole::Print("Unexpected key: %d.*s\n", t[i].end-t[i].start, JSON_STRING+t[i].start);
		}
	}
} 
