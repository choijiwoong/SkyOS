#include "UserAPI.h"
#include "string.h"
#include "math.h"

int main(){
	double value=sqrt(10.0f);
	printf("sqrt(10.0f) : %f\n", value);
	
	float value2=(float)cos(M_PI / 4.0f);
	printf("cos(45) : %f\n", value2);
	
	float value3=(float)sin((double)(M_PI / 3.0f));
	printf("sin(60): %f\n", value3);
	
	float value4=45.0f * (M_PI/180.0);
	printf("General float Operation Test: %f\n", value4);
}
