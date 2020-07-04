#include "bmp.h"


/*
	���������
		bmpfilename��bmp�ļ�����
		yuvfilename��yuv�ļ���
		yuvmode��yuv�ĸ�ʽ
	���ܣ���bmp�ļ�ת��Ϊyuv��ʽ��ģʽ��ѡ
	����ֵ����
*/

void get_bmpdata(char * bmpfilename,char * yuvfilename, char yuvmode)
{
	FILE * fp, *fv;
	int i;
	WORD lineSize;				//һ�е��ֽڸ���
	BMPFileHead bfh;			//�����ļ�ͷ��Ϣ
	BMPHeaderInfo bhi;			//����bmpͷ��Ϣ
	//bmp�е�ɫ��16ɫ��256ɫ��24λ�޵�ɫ������ģʽ��
	//ɫ����Ϊ���ݵ���ɫӳ�䣨ֱ��ӳ���������Ķ�������Ƶ�ʸߣ����Բ����������洢
	RGB  color[256]={0};		
	YUV  yuvcolor[256]={0};		//RGB��YUVӳ��
	BYTE * databuf;				//���ݻ���

	if((fp = fopen(bmpfilename, "r+b"))== NULL){
		printf("open bmpfile error!\n");
		exit(EXIT_FAILURE);
	}
	//��ȡ�ļ�ͷ��Ϣ
	if ((fread(&bfh,sizeof(WORD), 7, fp)) < 7){
		printf("read file error!\n");
		exit(EXIT_FAILURE);
	}
	//��ȡλͼ��Ϣͷ
	if ((fread(&bhi,sizeof(WORD),20 , fp)) < 20){
		printf("read file error!\n");
		exit(EXIT_FAILURE);
	}
	if (bhi.biHeight%2 == 1 || bhi.biWidth%2 ==1){
		printf("the size of picture is wrong!\n");
		exit(EXIT_FAILURE);
	}
	//�������24λɫ���ȡ��ɫ��
	if (bhi.biBitCount != 0x18){
		for (i=0;i<pow(2,bhi.biBitCount);i++){
			fread(&(color[i]),sizeof(BYTE), 4, fp);
			calculateYUV((yuvcolor+i), color[i]);
		}
	}
	lineSize = bhi.biSizeImage/bhi.biHeight;
	//�����ڴ��ȡbmp����
	databuf = (BYTE *)malloc(sizeof(BYTE)*bhi.biSizeImage);
	if (databuf == NULL){
		printf("mallocation error!\n");
		exit(EXIT_FAILURE);
	}
	//һ���Զ�ȡȫ������
	for (i = bhi.biSizeImage-lineSize;i>=0;i=i-lineSize){
		if ((fread(databuf+i, sizeof(BYTE),  lineSize, fp)) < lineSize){
			printf("get data error!\n");
			exit(EXIT_FAILURE);
		}
	}
	fclose(fp);
	//��Ҫд���yuv�ļ�
	if((fv = fopen(yuvfilename, "w+b"))== NULL){
		printf("open yuv file error!\n");
		exit(EXIT_FAILURE);
	}
	//��ʽת��
	to_yuv(fv,databuf, yuvcolor, yuvmode,bhi);
	//�ͷ��ڴ�ر��ļ�
	free(databuf);
	fclose(fv);
}




/*
	���������
		fv��yuv�ļ�ָ��
		databuf����bmp��ȡ��4�ֽ�����
		yuvcolor��rgb��yuv��ɫӳ���
		yuvmode��yuvд��ģʽ
		bhi:bmp�ĸ�ʽ��Ϣ
	���ܣ�����databuf�����ݲ���yuvcolor�е���ɫ������yuvmode�ĸ�ʽд��fvָ����ļ���
	����ֵ���ޣ�
*/
void to_yuv(FILE *fv, BYTE databuf[], YUV yuvcolor[], int yuvmode, BMPHeaderInfo bhi)
{
	switch (bhi.biBitCount){
	case 1://1������1λ,2����ɫ
		writeyuv2(fv, databuf, yuvcolor, yuvmode,bhi);
		break;
	case 4://1������4λ�� 16����ɫ
		writeyuv16(fv, databuf, yuvcolor, yuvmode,bhi);
		break;
	case 8://1������8λ��256����ɫ
		writeyuv256(fv, databuf, yuvcolor, yuvmode,bhi);
		break;
	case 24://24λ�޵�ɫ��
		writeyuv24(fv, databuf, yuvmode,bhi);
		break;
	default:{
		printf("bmp error!\n");
		exit(EXIT_FAILURE);
			}
	}
}


/*
	���������
		fv��yuv�ļ�ָ�룻
		databuf�����ݻ�������
		yuvcolor��rgb��yuv��ɫӳ�䣻
		yuvmode��ģʽ��
		bhi��bmp�ļ���Ϣ
	���ܣ�����ɫ��bmp�ļ�ת��Ϊָ����ʽ��yuv�ļ�
	����ֵ����
*/
void writeyuv2(FILE *fv, BYTE databuf[], YUV yuvcolor[], int yuvmode,BMPHeaderInfo bhi)
{
	unsigned long i;
	int flag=0, count=0;
	BYTE bit = 0x80;
	DWORD lineSize = bhi.biSizeImage/bhi.biHeight;

	if (yuvmode == '0'){
		for (i=0; i<(bhi.biSizeImage*sizeof(BYTE)); i++){
			bit = 0x80;
			while (bit != 0){
				flag=0;
				if ((databuf[i]&bit) == bit){
					flag=1;
				}
				if ((fwrite(&(yuvcolor[flag].yuvY), sizeof(BYTE), 1, fv))== 0){
					printf("write error!\n");
					exit(EXIT_FAILURE);
				}
				count++;
				if(count == (bhi.biWidth)){
					count = 0;
					//д��Y֮��i���ϸ���ʣ��Ŀ��ֽ�
					if((i+1)%lineSize != 0)
						i = i+ (lineSize-(i+1)%lineSize);
					break;
				}
				bit >>=1;
			}	
		}
		//дU
		for (i=0; i<(bhi.biSizeImage*sizeof(BYTE)); i++){
			bit = 0x80;
			while (bit != 0){
				flag=0;
				if ((databuf[i]&bit) == bit){
					flag=1;
				}
				if ((fwrite(&(yuvcolor[flag].yuvU), sizeof(BYTE), 1, fv))== 0){
					printf("write error!\n");
					exit(EXIT_FAILURE);
				}
				count++;
				if(count == (bhi.biWidth)/2){
					count = 0;
					//д��U֮��i���ϸ���ʣ��Ŀ��ֽں���һ�е��ֽڣ�������һ��
					if((i+1)%lineSize == 0 && (i+1)>=lineSize)
						i = i+lineSize;
					else
						i = i+ (lineSize-(i+1)%lineSize)+ lineSize;
					break;
				}
				bit >>=2;
			}
		}
		//дV
		for (i=0; i<(bhi.biSizeImage*sizeof(BYTE)); i++){
			bit = 0x80;
			while (bit != 0){
				flag=0;
				if ((databuf[i]&bit) == bit){
					flag=1;
				}
				if ((fwrite(&(yuvcolor[flag].yuvV), sizeof(BYTE), 1, fv))== 0){
					printf("write error!\n");
					exit(EXIT_FAILURE);
				}
				count++;
				if(count == (bhi.biWidth)/2){
					count = 0;
					//д��V֮��i���ϸ���ʣ��Ŀ��ֽں���һ�е��ֽڣ�������һ��
					if((i+1)%lineSize == 0 && (i+1)>=lineSize)
						i = i+lineSize;
					else
						i = i+ (lineSize-(i+1)%lineSize)+ lineSize;
					break;
				}
				bit >>=2;
			}
		}

	} 
	else if(yuvmode == '2'){
		for (i=0; i<(bhi.biSizeImage*sizeof(BYTE)); i++){
			bit = 0x80;
			while (bit != 0){
				flag=0;
				if ((databuf[i]&bit) == bit){
					flag=1;
				}
				if ((fwrite(&(yuvcolor[flag].yuvY), sizeof(BYTE), 1, fv))== 0){
					printf("write error!\n");
					exit(EXIT_FAILURE);
				}
				count++;
				if(count == (bhi.biWidth)){
					count = 0;
					//д��Y֮��i���ϸ���ʣ��Ŀ��ֽ�
					if((i+1)%lineSize != 0)
						i = i+ (lineSize-(i+1)%lineSize);
					break;
				}
				bit >>=1;
			}	
		}
		//дU
		for (i=0; i<(bhi.biSizeImage*sizeof(BYTE)); i++){
			bit = 0x80;
			while (bit != 0){
				flag=0;
				if ((databuf[i]&bit) == bit){
					flag=1;
				}
				if ((fwrite(&(yuvcolor[flag].yuvU), sizeof(BYTE), 1, fv))== 0){
					printf("write error!\n");
					exit(EXIT_FAILURE);
				}
				count++;
				if(count == (bhi.biWidth)/2){
					count = 0;
					//д��U֮��i���ϸ���ʣ��Ŀ��ֽ�
					if((i+1)%lineSize != 0)
						i = i+ (lineSize-(i+1)%lineSize);
					break;
				}
				bit >>=2;
			}
		}
		//дV
		for (i=0; i<(bhi.biSizeImage*sizeof(BYTE)); i++){
			bit = 0x80;
			while (bit != 0){
				flag=0;
				if ((databuf[i]&bit) == bit){
					flag=1;
				}
				if ((fwrite(&(yuvcolor[flag].yuvV), sizeof(BYTE), 1, fv))== 0){
					printf("write error!\n");
					exit(EXIT_FAILURE);
				}
				count++;
				if(count == (bhi.biWidth)/2){
					count = 0;
					//д��V֮��i���ϸ���ʣ��Ŀ��ֽ�
					if((i+1)%lineSize != 0)
						i = i+ (lineSize-(i+1)%lineSize);
					break;
				}
				bit >>=2;
			}
		}
	}
	else if(yuvmode == '4'){
		for (i=0; i<(bhi.biSizeImage*sizeof(BYTE)); i++){
			bit = 0x80;
			while (bit != 0){
				flag=0;
				if ((databuf[i]&bit) == bit){
					flag=1;
				}
				if ((fwrite(&(yuvcolor[flag].yuvY), sizeof(BYTE), 1, fv))== 0){
					printf("write error!\n");
					exit(EXIT_FAILURE);
				}
				count++;
				if(count == (bhi.biWidth)){
					count = 0;
					//д��Y֮��i���ϸ���ʣ��Ŀ��ֽ�
					if((i+1)%lineSize != 0)
						i = i+ (lineSize-(i+1)%lineSize);
					break;
				}
				bit >>=1;
			}	
		}
		//дU
		for (i=0; i<(bhi.biSizeImage*sizeof(BYTE)); i++){
			bit = 0x80;
			while (bit != 0){
				flag=0;
				if ((databuf[i]&bit) == bit){
					flag=1;
				}
				if ((fwrite(&(yuvcolor[flag].yuvU), sizeof(BYTE), 1, fv))== 0){
					printf("write error!\n");
					exit(EXIT_FAILURE);
				}
				count++;
				if(count == (bhi.biWidth)){
					count = 0;
					//д��U֮��i���ϸ���ʣ��Ŀ��ֽ�
					if((i+1)%lineSize != 0)
						i = i+ (lineSize-(i+1)%lineSize);
					break;
				}
				bit >>=1;
			}
		}
		//дV
		for (i=0; i<(bhi.biSizeImage*sizeof(BYTE)); i++){
			bit = 0x80;
			while (bit != 0){
				flag=0;
				if ((databuf[i]&bit) == bit){
					flag=1;
				}
				if ((fwrite(&(yuvcolor[flag].yuvV), sizeof(BYTE), 1, fv))== 0){
					printf("write error!\n");
					exit(EXIT_FAILURE);
				}
				count++;
				if(count == (bhi.biWidth)){
					count = 0;
					//д��V֮��i���ϸ���ʣ��Ŀ��ֽ�
					if((i+1)%lineSize != 0)
						i = i+ (lineSize-(i+1)%lineSize);
					break;
				}
				bit >>=1;
			}
		}
	}
	else{
		printf("bmp to yuv error!��2��\n");
		exit(EXIT_FAILURE);
	}
}



/*
	���������
		fv��yuv�ļ�ָ�룻
		databuf�����ݻ�������
		yuvcolor��rgb��yuv��ɫӳ�䣻
		yuv��ģʽ��
		bhi��bmp�ļ���Ϣ
	���ܣ���16ɫ��bmp�ļ�ת��Ϊָ����ʽ��yuv�ļ�
	����ֵ����
*/
void writeyuv16(FILE *fv, BYTE databuf[], YUV yuvcolor[], int yuvmode,BMPHeaderInfo bhi)
{
	DWORD lineSize = bhi.biSizeImage/bhi.biHeight;
	unsigned long i, count=0;
	BYTE bit = 0x0f;

	if (yuvmode == '0'){
		for (i=0; i<bhi.biSizeImage*sizeof(BYTE); i++){
			bit = databuf[i] & 0xf0;
			bit >>=4;
			if ((fwrite(&(yuvcolor[bit].yuvY), sizeof(BYTE), 1, fv))== 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
				continue;
			}
			bit = databuf[i] & 0x0f;
			if ((fwrite(&(yuvcolor[bit].yuvY), sizeof(BYTE), 1, fv))== 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��Y֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
			}
		}
		//дU
		count =0;
		for (i=0; i<bhi.biSizeImage*sizeof(BYTE); i++){
			bit = databuf[i] & 0xf0;
			bit >>=4;
			if ((fwrite(&(yuvcolor[bit].yuvU), sizeof(BYTE), 1, fv))== 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)/2){
				count = 0;
				//д��U֮��i���ϸ���ʣ��Ŀ��ֽں���һ�е��ֽڣ�������һ��
				if((i+1)%lineSize == 0 && (i+1)>=lineSize)
					i = i+lineSize;
				else
					i = i+ (lineSize-(i+1)%lineSize)+ lineSize;
			}
			
		}
		//дV
		count =0;
		for (i=0; i<bhi.biSizeImage*sizeof(BYTE); i++){
			bit = databuf[i] & 0xf0;
			bit >>=4;
			if ((fwrite(&(yuvcolor[bit].yuvV), sizeof(BYTE), 1, fv))== 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)/2){
				count = 0;
				//д��V֮��i���ϸ���ʣ��Ŀ��ֽں���һ�е��ֽڣ�������һ��
				if((i+1)%lineSize == 0 && (i+1)>=lineSize)
					i = i+lineSize;
				else
					i = i+ (lineSize-(i+1)%lineSize)+ lineSize;
			}
		}
	}
	else if(yuvmode == '2'){
		for (i=0; i<bhi.biSizeImage*sizeof(BYTE); i++){
			bit = databuf[i] & 0xf0;
			bit >>=4;		
			if ((fwrite(&(yuvcolor[bit].yuvY), sizeof(BYTE), 1, fv))== 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��Y֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
				continue;
			}
			bit = databuf[i] & 0x0f;
			if ((fwrite(&(yuvcolor[bit].yuvY), sizeof(BYTE), 1, fv))== 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��Y֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
			}
		}
		//дU
		count =0;
		for (i=0; i<bhi.biSizeImage*sizeof(BYTE); i++){
			bit = databuf[i] & 0xf0;
			bit >>=4;
			if ((fwrite(&(yuvcolor[bit].yuvU), sizeof(BYTE), 1, fv))== 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)/2){
				count = 0;
				//д��U֮��i���ϸ���ʣ��Ŀ��ֽں���һ�е��ֽڣ�������һ��
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
			}
		}
		//дV
		count =0;
		for (i=0; i<bhi.biSizeImage*sizeof(BYTE); i++){
			bit = databuf[i] & 0xf0;
			bit >>=4;
			if ((fwrite(&(yuvcolor[bit].yuvV), sizeof(BYTE), 1, fv))== 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)/2){
				count = 0;
				//д��V֮��i���ϸ���ʣ��Ŀ��ֽں���һ�е��ֽڣ�������һ��
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
			}
		}
	}
	else if(yuvmode == '4'){
		for (i=0; i<bhi.biSizeImage*sizeof(BYTE); i++){
			bit = databuf[i] & 0xf0;
			bit >>=4;
			if ((fwrite(&(yuvcolor[bit].yuvY), sizeof(BYTE), 1, fv))== 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			bit = databuf[i] & 0x0f;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��Y֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
				continue;
			}
			if ((fwrite(&(yuvcolor[bit].yuvY), sizeof(BYTE), 1, fv))== 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��Y֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
			}
		}
		//дU
		count =0;
		for (i=0; i<bhi.biSizeImage*sizeof(BYTE); i++){
			bit = databuf[i] & 0xf0;
			bit >>=4;
			if ((fwrite(&(yuvcolor[bit].yuvU), sizeof(BYTE), 1, fv))== 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			bit = databuf[i] & 0x0f;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��U֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
				continue;
			}
			if ((fwrite(&(yuvcolor[bit].yuvU), sizeof(BYTE), 1, fv))== 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��U֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
			}
		}
		//дV
		count =0;
		for (i=0; i<bhi.biSizeImage*sizeof(BYTE); i++){
			bit = databuf[i] & 0xf0;
			bit >>=4;
			if ((fwrite(&(yuvcolor[bit].yuvV), sizeof(BYTE), 1, fv))== 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			bit = databuf[i] & 0x0f;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��V֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
				continue;
			}
			if ((fwrite(&(yuvcolor[bit].yuvV), sizeof(BYTE), 1, fv))== 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��V֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
			}
		}
	}
}


/*
	���������
		fv��yuv�ļ�ָ�룻
		databuf�����ݻ�������
		yuvcolor��rgb��yuv��ɫӳ�䣻
		yuv��ģʽ��
		bhi��bmp�ļ���Ϣ
	���ܣ���256ɫ��bmp�ļ�ת��Ϊָ����ʽ��yuv�ļ�
	����ֵ����
*/
void writeyuv256(FILE *fv, BYTE databuf[], YUV yuvcolor[], int yuvmode,BMPHeaderInfo bhi)
{
	unsigned long i, count=0;
	DWORD lineSize = bhi.biSizeImage/bhi.biHeight;

	if (yuvmode == '0'){
		for (i=0;i<bhi.biSizeImage*sizeof(BYTE); i++){
			if ((fwrite(&(yuvcolor[databuf[i]].yuvY),sizeof(BYTE), 1, fv)) == 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			if(count == (bhi.biWidth)){
				count = 0;
				//д��Y֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
			}
		}
		//дU
		for (i=0;i<bhi.biSizeImage*sizeof(BYTE); i=i+2){
			if ((fwrite(&(yuvcolor[databuf[i]].yuvU),sizeof(BYTE), 1, fv)) ==0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)/2){
				count = 0;
				//д��U֮��i���ϸ���ʣ��Ŀ��ֽں���һ�е��ֽڣ�������һ��
				if((i+1)%lineSize == 0 && (i+1)>=lineSize)
					i = i+lineSize;
				else
					i = i+ (lineSize-(i+1)%lineSize)+ lineSize;
			}
		}
		//дV
		for (i=0;i<bhi.biSizeImage*sizeof(BYTE); i=i+2){
			if ((fwrite(&(yuvcolor[databuf[i]].yuvV),sizeof(BYTE), 1, fv)) ==0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)/2){
				count = 0;
				//д��V֮��i���ϸ���ʣ��Ŀ��ֽں���һ�е��ֽڣ�������һ��
				if((i+1)%lineSize == 0 && (i+1)>=lineSize)
					i = i+lineSize;
				else
					i = i+ (lineSize-(i+1)%lineSize)+ lineSize;
			}
		}
	} 
	else if(yuvmode == '2'){
		for (i=0;i<bhi.biSizeImage*sizeof(BYTE); i++){
			if ((fwrite(&(yuvcolor[databuf[i]].yuvY),sizeof(BYTE), 1, fv)) == 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��Y֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
			}
		}
		//дU
		for (i=0;i<bhi.biSizeImage*sizeof(BYTE); i=i+2){
			if ((fwrite(&(yuvcolor[databuf[i]].yuvU),sizeof(BYTE), 1, fv)) ==0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)/2){
				count = 0;
				//д��U֮��i���ϸ���ʣ��Ŀ��ֽں���һ�е��ֽڣ�������һ��
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
			}
		}
		//дV
		for (i=0;i<bhi.biSizeImage*sizeof(BYTE); i=i+2){
			if ((fwrite(&(yuvcolor[databuf[i]].yuvV),sizeof(BYTE), 1, fv)) ==0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)/2){
				count = 0;
				//д��V֮��i���ϸ���ʣ��Ŀ��ֽں���һ�е��ֽڣ�������һ��
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
			}
		}
	}
	else if(yuvmode == '4'){
		for (i=0;i<bhi.biSizeImage*sizeof(BYTE); i++){
			if ((fwrite(&(yuvcolor[databuf[i]].yuvY),sizeof(BYTE), 1, fv)) == 0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��Y֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
			}
		}
		//дU
		for (i=0;i<bhi.biSizeImage*sizeof(BYTE); i++){
			if ((fwrite(&(yuvcolor[databuf[i]].yuvU),sizeof(BYTE), 1, fv)) ==0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count ++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��U֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
			}
		}
		//дV
		for (i=0;i<bhi.biSizeImage*sizeof(BYTE); i++){
			if ((fwrite(&(yuvcolor[databuf[i]].yuvV),sizeof(BYTE), 1, fv)) ==0){
				printf("write error!\n");
				exit(EXIT_FAILURE);
			}
			count ++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��V֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+1)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize);
			}
		}
	}

}


/*
	���������
		fv��yuv�ļ�ָ�룻
		databuf�����ݻ�������
		yuv��ģʽ��
		bhi��bmp�ļ���Ϣ
	���ܣ���rgb��24λ��bmp�ļ�ת��Ϊָ����ʽ��yuv�ļ�
	����ֵ����
*/
void writeyuv24(FILE *fv, BYTE databuf[], int yuvmode,BMPHeaderInfo bhi)
{
	unsigned long i, count=0;
	YUV yuv24;
	DWORD lineSize = bhi.biSizeImage/bhi.biHeight;

	if (yuvmode == '0'){
		for (i=0; i<bhi.biSizeImage; i=i+3){
			yuv24.yuvY = 0.257*databuf[i+2] + \
				0.504*databuf[i+1] + 0.098*databuf[i] + 16;
			if ((fwrite(&(yuv24.yuvY), sizeof(BYTE), 1, fv)) == 0){
				printf("wtite error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��Y֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+3)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize)-2;
			}
		}
		//дU
		for (i=0; i<bhi.biSizeImage; i=i+6){
			yuv24.yuvU = -0.148*databuf[i+2] - \
				0.291*databuf[i+1] + 0.439*databuf[i] + 128;
			if ((fwrite(&(yuv24.yuvU), sizeof(BYTE), 1, fv)) == 0){
				printf("wtite error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)/2){
				count = 0;
				//д��U֮��i���ϸ���ʣ��Ŀ��ֽں���һ�е��ֽڣ�������һ��
				if((i+3)%lineSize == 0 && (i+3)>=lineSize)
					i = i+lineSize;
				else
					i = i+ (lineSize-(i+1)%lineSize)-2+ lineSize;
			}
		}
		//дV
		for (i=0; i<bhi.biSizeImage; i=i+6){
			yuv24.yuvV = 0.439*databuf[i+2] - \
				0.368*databuf[i+1] - 0.071*databuf[i] + 128;
			if ((fwrite(&(yuv24.yuvV), sizeof(BYTE), 1, fv)) == 0){
				printf("wtite error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)/2){
				count = 0;
				//д��V֮��i���ϸ���ʣ��Ŀ��ֽں���һ�е��ֽڣ�������һ��
				if((i+3)%lineSize == 0 && (i+3)>=lineSize)
					i = i+lineSize;
				else
					i = i+ (lineSize-(i+1)%lineSize)-2+ lineSize;
			}
		}
	} 
	else if(yuvmode == '2'){
		for (i=0; i<bhi.biSizeImage; i=i+3){
			yuv24.yuvY = 0.257*databuf[i+2] + \
				0.504*databuf[i+1] + 0.098*databuf[i] + 16;
			if ((fwrite(&(yuv24.yuvY), sizeof(BYTE), 1, fv)) == 0){
				printf("wtite error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��Y֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+3)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize)-2;
			}
		}
		//дU
		for (i=0; i<bhi.biSizeImage; i=i+6){
			yuv24.yuvU = -0.148*databuf[i+2] - \
				0.291*databuf[i+1] + 0.439*databuf[i] + 128;
			if ((fwrite(&(yuv24.yuvU), sizeof(BYTE), 1, fv)) == 0){
				printf("wtite error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��U֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+3)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize)-2;
			}
		}
		//дV
		for (i=0; i<bhi.biSizeImage; i=i+6){
			yuv24.yuvV = 0.439*databuf[i+2] - \
				0.368*databuf[i+1] - 0.071*databuf[i] + 128;
			if ((fwrite(&(yuv24.yuvV), sizeof(BYTE), 1, fv)) == 0){
				printf("wtite error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��V֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+3)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize)-2;
			}
		}
	}
	else if(yuvmode == '4'){
		for (i=0; i<bhi.biSizeImage; i=i+3){
			yuv24.yuvY = 0.257*databuf[i+2] + \
				0.504*databuf[i+1] + 0.098*databuf[i] + 16;
			if ((fwrite(&(yuv24.yuvY), sizeof(BYTE), 1, fv)) == 0){
				printf("wtite error!\n");
				exit(EXIT_FAILURE);
			}
			if(count == (bhi.biWidth)){
				count = 0;
				//д��Y֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+3)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize)-2;
			}
		}
		//дU
		for (i=0; i<bhi.biSizeImage; i=i+3){
			yuv24.yuvU = -0.148*databuf[i+2] - \
				0.291*databuf[i+1] + 0.439*databuf[i] + 128;
			if ((fwrite(&(yuv24.yuvU), sizeof(BYTE), 1, fv)) == 0){
				printf("wtite error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��U֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+3)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize)-2;
			}
		}
		//дV
		for (i=0; i<bhi.biSizeImage; i=i+3){
			yuv24.yuvV = 0.439*databuf[i+2] - \
				0.368*databuf[i+1] - 0.071*databuf[i] + 128;
			if ((fwrite(&(yuv24.yuvV), sizeof(BYTE), 1, fv)) == 0){
				printf("wtite error!\n");
				exit(EXIT_FAILURE);
			}
			count++;
			if(count == (bhi.biWidth)){
				count = 0;
				//д��V֮��i���ϸ���ʣ��Ŀ��ֽ�
				if((i+3)%lineSize != 0)
					i = i+ (lineSize-(i+1)%lineSize)-2;
			}
		}
	}
}


//ӳ��yuv����ɫ��
void calculateYUV(YUV *yuvcolor,RGB rgbcolor )
{
	yuvcolor->yuvY = 0.257*rgbcolor.rgbRed + \
		0.504*rgbcolor.rgbGreen + 0.098*rgbcolor.rgbBlue + 16;
	yuvcolor->yuvU = -0.148*rgbcolor.rgbRed - \
		0.291*rgbcolor.rgbGreen + 0.439*rgbcolor.rgbBlue + 128;
	yuvcolor->yuvV = 0.439*rgbcolor.rgbRed - \
		0.368*rgbcolor.rgbGreen - 0.071*rgbcolor.rgbBlue + 128;
}
