#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
//������Ϣ
void usage(void);


int main(int argc, char * argv[])
{
	
	int i=0;
	char option[3]= {'0','2','4'};//YUV�ļ���3��ģʽѡ��ֱ�Ϊ420��422��444
	if (argc ==4){
		if(argv[1][1] == option[0] || argv[1][1] == option[1] \
			||argv[1][1] == option[2] || argv[2]!= NULL ||argv[3]!= NULL){
				get_bmpdata(argv[2],argv[3],argv[1][1] );
				//get_bmpdata("16_20_20.bmp","1620_20yuv420.yuv",'0' );
		}
		else{
			usage();
		}
	}
	else {
		usage();
	}
}

//help
void usage(void)
{
	printf("\n**************BMP TO YUV**************\n");
	printf("\tBmp�ļ��ĳ��Ϳ�����ظ�������Ϊż��\n");
	printf("command:\n\t-0\t��bmpת��ΪI420(4:2:0)��yuv�ļ�\n");
	printf("\t-2\t��bmpת��ΪYUY2��4:2:2�����ļ���ʽ\n");
	printf("\t-4\t��bmpת��ΪYUV444(4:4:4)���ļ���ʽ\n");
	printf("example:  BMPtoYUV.exe -0 input.bmp ouput.yuv\n");
}