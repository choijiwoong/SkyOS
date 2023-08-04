//VS�� __FILE__ __func__ __LINE__��ũ�η� ��Ȱ�ϰ� �α׸� ���� �� �ִ�.

#define SKY_ASSERT(Expr, Msg) __SKY_ASSERT(#Expr, Expr, __FILE__, __LINE__, Msg)

void __SKY_ASSERT(const char* expr_str, bool expr, const char* file, int line, const char* msg){
	if(!expr){
		char buf[256];
		sprintf(buf, "Assert failed: %s Expected: %s %s %d\n", msg, expr_str, file, line);
		HaltSystem(buf);//��罺ũ�� 
	}
} 
