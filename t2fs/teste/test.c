#include <stdio.h>
#include "../include/t2fs.h"
#include <stdio.h>


int main() {
	int outputVal;
	char out[1026];
	FILE2 dave = create2("dave");
	printf("handle é %d\n", (int)dave );

	char data[1026];
	int i;
	for(i = 0; i<1026; i++){
		
		data[i]=65 + i%26;	
		//printf("::%c\n", data[i]);
	}
	outputVal = seek2(dave, 0);
	outputVal = write2(dave, data,1026);
	printf("write output value:%d\n", outputVal);

	outputVal = seek2(dave, 0);
	printf("seek output value:%d\n", outputVal);
	//return 0;
	
	outputVal = read2(dave, out,1026);
	printf("read output value:%d\n", outputVal);

	for(i = 0; i<1026; i++){
		if(out[i]!=data[i]){
			printf("error");	
			break;
		}

	}
	
	char message[12] = "Hello Guys!";
	seek2(dave, 12);
	outputVal = write2(dave, message,12);
	printf("write output value:%d\n", outputVal);
	seek2(dave, 12);
	outputVal = read2(dave, out,12);
	printf("read output value:%d\n", outputVal);

	seek2(dave, 1024);
	outputVal = write2(dave,"@@fim",5);
	printf("write output value:%d\n", outputVal);
	seek2(dave, 1024);
	outputVal = read2(dave, out,12);
	printf("read output value:%d\n", outputVal);
	out[12] = '\0';
	printf("%s", out);

}
