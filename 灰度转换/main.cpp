#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>

/*
λͼͷ�ṹ
*/
#pragma pack(1)
typedef struct tagBITMAPFILEHEADER
{
      unsigned char bfType[2];//�ļ���ʽ
      unsigned long bfSize;//�ļ���С
      unsigned short bfReserved1;//����
      unsigned short bfReserved2;
      unsigned long bfOffBits; //DIB�������ļ��е�ƫ����
 }fileHeader;
  #pragma pack()
  /*
  λͼ������Ϣ�ṹ
  */
  #pragma pack(1)
  typedef struct tagBITMAPINFOHEADER
  {
      unsigned long biSize;//�ýṹ�Ĵ�С
      long biWidth;//�ļ����
      long biHeight;//�ļ��߶�
      unsigned short biPlanes;//ƽ����
      unsigned short biBitCount;//��ɫλ��
      unsigned long biCompression;//ѹ������
      unsigned long biSizeImage;//DIB��������С
      long biXPixPerMeter;
      long biYPixPerMeter;
      unsigned long biClrUsed;//������ɫ������
      unsigned long biClrImporant;//������Ҫ��ɫ
  }fileInfo;
  #pragma pack()
  /*
  ��ɫ��ṹ
  */
  #pragma pack(1)
  typedef struct tagRGBQUAD
  {
      unsigned char rgbBlue; //��ɫ��������
      unsigned char rgbGreen;//��ɫ��������
      unsigned char rgbRed;//��ɫ��������
      unsigned char rgbReserved;
  }rgbq;
  #pragma pack()

int main()
  {
      /*�洢RGBͼ���һ�����ص�*/
      unsigned char ImgData[3000][3];
      /*���Ҷ�ͼ�����ش浽һ��һά������*/
      unsigned char ImgData2[3000];
      int i,j,k;
      FILE * fpBMP,* fpGray;
      fileHeader * fh;
      fileInfo * fi;
      rgbq * fq;
      
      if((fpBMP=fopen("2.bmp","rb"))==NULL)
      {
          printf("���ļ�ʧ��");
          exit(0);
      }
      
      if((fpGray=fopen("1.bmp","wb"))==NULL)
      {
          printf("�����ļ�ʧ��");
          exit(0);
      }
      
      fh=(fileHeader *)malloc(sizeof(fileHeader));
      fi=(fileInfo *)malloc(sizeof(fileInfo));
      //��ȡλͼͷ�ṹ����Ϣͷ
      fread(fh,sizeof(fileHeader),1,fpBMP);
      fread(fi,sizeof(fileInfo),1,fpBMP);
      //�޸�ͷ��Ϣ
      fi->biBitCount=8;
      fi->biSizeImage=( (fi->biWidth*3+3)/4 ) * 4*fi->biHeight;
      //fi->biClrUsed=256;
      
      fh->bfOffBits = sizeof(fileHeader)+sizeof(fileInfo)+256*sizeof(rgbq);
      fh->bfSize = fh->bfOffBits + fi->biSizeImage;
      
      //������ɫ��
      fq=(rgbq *)malloc(256*sizeof(rgbq));
      for(i=0;i<256;i++)
      {
          fq[i].rgbBlue=fq[i].rgbGreen=fq[i].rgbRed=i;
          //fq[i].rgbReserved=0;
      }
      //��ͷ��Ϣд��
     fwrite(fh,sizeof(fileHeader),1,fpGray);  
     fwrite(fi,sizeof(fileInfo),1,fpGray); 
     fwrite(fq,sizeof(rgbq),256,fpGray);
     //��ȡRGBͼ���ز�ת��Ϊ�Ҷ�ֵ
     for ( i=0;i<fi->biHeight;i++ )
     {
         for(j=0;j<(fi->biWidth+3)/4*4;j++)
         {
             for(k=0;k<3;k++)
                 fread(&ImgData[j][k],1,1,fpBMP);
         }
         for(j=0;j<(fi->biWidth+3)/4*4;j++)
         {
             ImgData2[j]=int( (float)ImgData[j][0] * 0.114 +
                         (float)ImgData[j][1] * 0.587 +
                         (float)ImgData[j][2] * 0.299 );
         }
         //���Ҷ�ͼ��Ϣд��
         fwrite(ImgData2,j,1,fpGray);
     }   
         
         free(fh);
         free(fi);
         free(fq);
         fclose(fpBMP);
         fclose(fpGray);
         printf("success\n");
         return 0;
 }