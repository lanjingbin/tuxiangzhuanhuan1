#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>

/*
位图头结构
*/
#pragma pack(1)
typedef struct tagBITMAPFILEHEADER
{
      unsigned char bfType[2];//文件格式
      unsigned long bfSize;//文件大小
      unsigned short bfReserved1;//保留
      unsigned short bfReserved2;
      unsigned long bfOffBits; //DIB数据在文件中的偏移量
 }fileHeader;
  #pragma pack()
  /*
  位图数据信息结构
  */
  #pragma pack(1)
  typedef struct tagBITMAPINFOHEADER
  {
      unsigned long biSize;//该结构的大小
      long biWidth;//文件宽度
      long biHeight;//文件高度
      unsigned short biPlanes;//平面数
      unsigned short biBitCount;//颜色位数
      unsigned long biCompression;//压缩类型
      unsigned long biSizeImage;//DIB数据区大小
      long biXPixPerMeter;
      long biYPixPerMeter;
      unsigned long biClrUsed;//多少颜色索引表
      unsigned long biClrImporant;//多少重要颜色
  }fileInfo;
  #pragma pack()
  /*
  调色板结构
  */
  #pragma pack(1)
  typedef struct tagRGBQUAD
  {
      unsigned char rgbBlue; //蓝色分量亮度
      unsigned char rgbGreen;//绿色分量亮度
      unsigned char rgbRed;//红色分量亮度
      unsigned char rgbReserved;
  }rgbq;
  #pragma pack()

int main()
  {
      /*存储RGB图像的一行像素点*/
      unsigned char ImgData[3000][3];
      /*将灰度图的像素存到一个一维数组中*/
      unsigned char ImgData2[3000];
      int i,j,k;
      FILE * fpBMP,* fpGray;
      fileHeader * fh;
      fileInfo * fi;
      rgbq * fq;
      
      if((fpBMP=fopen("2.bmp","rb"))==NULL)
      {
          printf("打开文件失败");
          exit(0);
      }
      
      if((fpGray=fopen("1.bmp","wb"))==NULL)
      {
          printf("创建文件失败");
          exit(0);
      }
      
      fh=(fileHeader *)malloc(sizeof(fileHeader));
      fi=(fileInfo *)malloc(sizeof(fileInfo));
      //读取位图头结构和信息头
      fread(fh,sizeof(fileHeader),1,fpBMP);
      fread(fi,sizeof(fileInfo),1,fpBMP);
      //修改头信息
      fi->biBitCount=8;
      fi->biSizeImage=( (fi->biWidth*3+3)/4 ) * 4*fi->biHeight;
      //fi->biClrUsed=256;
      
      fh->bfOffBits = sizeof(fileHeader)+sizeof(fileInfo)+256*sizeof(rgbq);
      fh->bfSize = fh->bfOffBits + fi->biSizeImage;
      
      //创建调色版
      fq=(rgbq *)malloc(256*sizeof(rgbq));
      for(i=0;i<256;i++)
      {
          fq[i].rgbBlue=fq[i].rgbGreen=fq[i].rgbRed=i;
          //fq[i].rgbReserved=0;
      }
      //将头信息写入
     fwrite(fh,sizeof(fileHeader),1,fpGray);  
     fwrite(fi,sizeof(fileInfo),1,fpGray); 
     fwrite(fq,sizeof(rgbq),256,fpGray);
     //读取RGB图像素并转换为灰度值
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
         //将灰度图信息写入
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