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
void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val)//밝기, Val: 조절하기 원하는 밝기값
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] + Val > 255)		//클리핑
		{
			Out[i] = 255;
		}
		else if (Img[i] + Val < 0)	//클리핑
		{
			Out[i] = 0;
		}
		else 	Out[i] = Img[i] + Val;
	}
}
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val)//대비, Val: 조절하기 원하는 밝기값
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] * Val > 255.0)	//클리핑
		{
			Out[i] = 255;
		}
		else 	Out[i] = (BYTE)(Img[i] * Val);	//double이 8바이트인데 byte는 1바이트임, 다시 형변환 해줘야한다.
	}
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H)//히스토그램 만듦
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
}

void ObtainAHistogram(int* Histo, int* AHisto)//누적 histogram 만듦
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
		if (Histo[i] != 0) { //왼->오 처음으로 0이 아닌 값을 low로 취함
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) { //오->왼 처음으로 0이 아닌 값을 high
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++) {
		//정규화 공식
		Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0);// 분자: 현재 화소값이 어느정도 위치인지? 
	}
}

//평활화
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H)//AHisto: 누적 histogram
{
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255;//Gmax는 영상이 가능한 최대 밝기값
	double Ratio = Gmax / (double)Nt;
	BYTE NormSum[256];//정규화 합 저장할 배열, 자동으로 버림됨
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	}
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = NormSum[Img[i]];//정규화 합 값으로 대체하고자 하는 원본 영상값
	}
}

void Binarization(BYTE * Img, BYTE * Out, int W, int H, BYTE Threshold)//이진화, Threshold: 임계치
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}

int DivisionArea(int T, int * Histo)
{
	int G1 = 0, G2 = 0;//T보다 큰:G1 작은: G2
	int cG1 = 0, cG2 = 0;//count
	int aG1 = 0, aG2 = 0;
	for (int i = 0; i < T; i++)
	{
		G1 += i * Histo[i];//i가 T보다 작으면 G1에 더함, Histo[i]는 i번째에 몇개가 있는가?임
		cG1 += Histo[i];
	}
	for (int j = T; j < 256; j++)
	{
		G2 += j * Histo[j];
		cG2 += Histo[j];
	}
	aG1 = G1 / cG1;//aG1에 평균 저장
	aG2 = G2 / cG2;
	T = (aG1 + aG2) / 2;
	return T;
}

int GozalezBinThresh(BYTE * Img, int * Histo, int W, int H,int epsilon)
{
	BYTE Low=0 ,High=0;
	BYTE init_T,T;//임계치
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
	T = init_T;//복사
	printf("초기 임계치: %d\n", init_T);

	do {
		init_T = T;
		T=DivisionArea(init_T, Histo);
		printf("임계치: %d\n", T);
	} while (abs(init_T - T) > epsilon);//엡실론보다 크면 반복

	return T;
}

int main()
{
	BITMAPFILEHEADER hf; // 14바이트
	BITMAPINFOHEADER hInfo; // 40바이트
	RGBQUAD hRGB[256]; // 1024바이트
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
	int Thres = GozalezBinThresh(Image, Histo, hInfo.biWidth, hInfo.biHeight, 3);//엡실론:3
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
