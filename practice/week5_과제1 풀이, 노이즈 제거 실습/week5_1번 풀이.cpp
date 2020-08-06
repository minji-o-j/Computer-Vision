#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h>//abs

void InverseImage(BYTE* Img, BYTE *Out, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = 255 - Img[i];
	}
}
void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val)//���, Val: �����ϱ� ���ϴ� ��Ⱚ
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] + Val > 255)		//Ŭ����
		{
			Out[i] = 255;
		}
		else if (Img[i] + Val < 0)	//Ŭ����
		{
			Out[i] = 0;
		}
		else 	Out[i] = Img[i] + Val;
	}
}
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val)//���, Val: �����ϱ� ���ϴ� ��Ⱚ
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] * Val > 255.0)	//Ŭ����
		{
			Out[i] = 255;
		}
		else 	Out[i] = (BYTE)(Img[i] * Val);	//double�� 8����Ʈ�ε� byte�� 1����Ʈ��, �ٽ� ����ȯ ������Ѵ�.
	}
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H)//������׷� ����
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
}

void ObtainAHistogram(int* Histo, int* AHisto)//���� histogram ����
{
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHisto[i] += Histo[j];
		}
	}
}

void HistogramStretching(BYTE * Img, BYTE * Out, int * Histo, int W, int H)
{
	int ImgSize = W * H;
	BYTE Low, High;
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) { //��->�� ó������ 0�� �ƴ� ���� low�� ����
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) { //��->�� ó������ 0�� �ƴ� ���� high
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++) {
		//����ȭ ����
		Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0);// ����: ���� ȭ�Ұ��� ������� ��ġ����? 
	}
}

//��Ȱȭ
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H)//AHisto: ���� histogram
{
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255;//Gmax�� ������ ������ �ִ� ��Ⱚ
	double Ratio = Gmax / (double)Nt;
	BYTE NormSum[256];//����ȭ �� ������ �迭, �ڵ����� ������
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	}
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = NormSum[Img[i]];//����ȭ �� ������ ��ü�ϰ��� �ϴ� ���� ����
	}
}

void Binarization(BYTE * Img, BYTE * Out, int W, int H, BYTE Threshold)//����ȭ, Threshold: �Ӱ�ġ
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}

int DivisionArea(int T, int * Histo)
{
	int G1 = 0, G2 = 0;//T���� ū:G1 ����: G2
	int cG1 = 0, cG2 = 0;//count
	int aG1 = 0, aG2 = 0;
	for (int i = 0; i < T; i++)
	{
		G1 += i * Histo[i];//i�� T���� ������ G1�� ����, Histo[i]�� i��°�� ��� �ִ°�?��
		cG1 += Histo[i];
	}
	for (int j = T; j < 256; j++)
	{
		G2 += j * Histo[j];
		cG2 += Histo[j];
	}
	aG1 = G1 / cG1;//aG1�� ��� ����
	aG2 = G2 / cG2;
	T = (aG1 + aG2) / 2;
	return T;
}

int GozalezBinThresh(BYTE * Img, int * Histo, int W, int H,int epsilon)
{
	BYTE Low=0 ,High=0;
	BYTE init_T,T;//�Ӱ�ġ
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) { 
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) { 
			High = i;
			break;
		}
	}
	init_T = (Low - High) / 2;
	T = init_T;//����
	printf("�ʱ� �Ӱ�ġ: %d\n", init_T);

	do {
		init_T = T;
		T=DivisionArea(init_T, Histo);
		printf("�Ӱ�ġ: %d\n", T);
	} while (abs(init_T - T) > epsilon);//���Ƿк��� ũ�� �ݺ�

	return T;
}

int main()
{
	BITMAPFILEHEADER hf; // 14����Ʈ
	BITMAPINFOHEADER hInfo; // 40����Ʈ
	RGBQUAD hRGB[256]; // 1024����Ʈ
	FILE* fp;
	//fp = fopen("coin.bmp", "rb");
	fp = fopen("LENNA.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE * Image = (BYTE *)malloc(ImgSize);
	BYTE * Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	int Histo[256] = { 0 };
	int AHisto[256] = { 0 };

	/*
	ObtainAHistogram(Histo, AHisto);
	HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);
	*/

	ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	int Thres = GozalezBinThresh(Image, Histo, hInfo.biWidth, hInfo.biHeight, 3);//���Ƿ�:3
	Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);



	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 0.5);

	fp = fopen("output.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp);
	fclose(fp);
	free(Image);
	free(Output);
	return 0;
}
