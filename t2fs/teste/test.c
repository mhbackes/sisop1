#include <stdio.h>
#include "../include/t2fs.h"
#include <stdio.h>


int main() {
	int outputVal;
	
	FILE2 dave = create2("dave");
	printf("handle é %d\n", (int)dave );

	char data[3500];
	int i;
	for(i = 0; i<3500; i++){
		data[i]=i%256;	
	}

	outputVal = write2(dave, data,3500);
	printf("write output value:%d\n", outputVal);

	outputVal = seek2(dave, 0);
	printf("seek output value:%d\n", outputVal);
	//return 0;
	char out[3500];
	outputVal = read2(dave, out,3500);
	printf("read output value:%d\n", outputVal);

	for(i = 0; i<3500; i++){
		if(out[i]!=data[i]){
			printf("error");	
			break;
		}

	}
	close2(0);
	

	dave = open2("dave");
	read2(dave, out,3500);
}
