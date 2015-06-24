#include <stdio.h>
#include "../include/t2fs.h"
#include <stdio.h>


int main() {
	int outputVal;
	FILE2 dave = create2("dave");
	printf("handle é %d\n", (int)dave );

	char data[1024];
	int i;
	for(i = 0; i<1000; i++){
		data[i]=i%256;	
	}

	outputVal = write2(dave, data,1000);
	
	printf("write output value:%d\n", outputVal);
	//return 0;
	char out[1024];
	outputVal = read2(dave, out,1000);
	printf("read output value:%d\n", outputVal);
	out[19] = '\0';
	printf("%s", out);
}
