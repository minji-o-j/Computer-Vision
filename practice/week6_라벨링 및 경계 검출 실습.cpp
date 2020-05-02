#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>//��Ʈ�� ������� ������� �ȷ�Ʈ�� �ش��ϴ� ����ü ������ ����
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

//---------------------------------------------------------------
//[���� 3] Gonzalez �ڵ� ����ȭ �Ӱ�ġ ���� ���α׷� ����

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

//[���� 3] Gonzalez �ڵ� ����ȭ �Ӱ�ġ ���� ���α׷� ����
//---------------------------------------------------------------

void AverageConv(BYTE*Img, BYTE*Out, int W, int H)//�ڽ� ��Ȱȭ
{
	double Kernel[3][3] = { 0.1111 ,0.1111 ,0.1111 ,
						0.1111 ,0.1111 ,0.1111 ,
						0.1111 ,0.1111 ,0.1111 };

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y��ǥ,��,���ι���, ���Ʒ����ʿ����� margin�� ���Ͽ� 1���ͽ���, -1��
	{
		for (int j = 1; j < W-1; j++)//X��ǥ,��, ���ι���
		{
			//i,j:Ŀ���� ���� ��ġ
			for (int m = -1; m <= 1; m++)//Ŀ���� ��, m=-1����, m=0������, m=1�Ʒ���
			{
				for (int n = -1; n <= 1; n++)//Ŀ���� ��, n=-1���ʿ� n=1�����ʿ�
				{
					//i*W+j: 1���� �迭���� i,j�� �ش��ϴ� ��ġ indexing
					//i:�� j:��
					//Y*W+X(Y:Y��ǥ, X:X��ǥ)
					SumProduct += Img[(i+m)*W+(j+n)] * Kernel[m+1][n+1];//(m��n�� -1���Ͷ�)
				}
			}
			Out[i*W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void GausianAvrConv(BYTE*Img, BYTE*Out, int W, int H)//����þ� ��Ȱȭ,���Ϳ� ����ġ ���� ���� �ش�.
{
	double Kernel[3][3] = { 0.0625 ,0.125 ,0.0625 ,
						0.125 ,0.25 ,0.125 ,
						0.06251 ,0.125 ,0.0625 };

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y��ǥ,��,���ι���, ���Ʒ����ʿ����� margin�� ���Ͽ� 1���ͽ���, -1��
	{
		for (int j = 1; j < W - 1; j++)//X��ǥ,��, ���ι���
		{
			//i,j:Ŀ���� ���� ��ġ
			for (int m = -1; m <= 1; m++)//Ŀ���� ��, m=-1����, m=0������, m=1�Ʒ���
			{
				for (int n = -1; n <= 1; n++)//Ŀ���� ��, n=-1���ʿ� n=1�����ʿ�
				{
					//i*W+j: 1���� �迭���� i,j�� �ش��ϴ� ��ġ indexing
					//i:�� j:��
					//Y*W+X(Y:Y��ǥ, X:X��ǥ)
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];//(m��n�� -1���Ͷ�)
				}
			}
			Out[i*W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_X_Conv(BYTE*Img, BYTE*Out, int W, int H)//Prewitt ����ũ X, ���� ��谡 �� �� ���´�.
{
	double Kernel[3][3] = { -1.0,0.0,1.0,
							-1.0,0.0,1.0, 
							-1.0,0.0,1.0};

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y��ǥ,��,���ι���, ���Ʒ����ʿ����� margin�� ���Ͽ� 1���ͽ���, -1��
	{
		for (int j = 1; j < W - 1; j++)//X��ǥ,��, ���ι���
		{
			//i,j:Ŀ���� ���� ��ġ
			for (int m = -1; m <= 1; m++)//Ŀ���� ��, m=-1����, m=0������, m=1�Ʒ���
			{
				for (int n = -1; n <= 1; n++)//Ŀ���� ��, n=-1���ʿ� n=1�����ʿ�
				{
					//i*W+j: 1���� �迭���� i,j�� �ش��ϴ� ��ġ indexing
					//i:�� j:��
					//Y*W+X(Y:Y��ǥ, X:X��ǥ)
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];//(m��n�� -1���Ͷ�)
				}
			}
			//0~255���̰� ������ ���� ����
			//0~765(���񰪾���)������ ��-->0~255
			Out[i*W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_Y_Conv(BYTE*Img, BYTE*Out, int W, int H)//Prewitt ����ũ Y, ���� ��谡 �� �� ���´�.
{
	double Kernel[3][3] = { -1.0,-1.0,-1.0,
							0.0,0.0,0.0,
							1.0,1.0,1.0 };

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y��ǥ,��,���ι���, ���Ʒ����ʿ����� margin�� ���Ͽ� 1���ͽ���, -1��
	{
		for (int j = 1; j < W - 1; j++)//X��ǥ,��, ���ι���
		{
			//i,j:Ŀ���� ���� ��ġ
			for (int m = -1; m <= 1; m++)//Ŀ���� ��, m=-1����, m=0������, m=1�Ʒ���
			{
				for (int n = -1; n <= 1; n++)//Ŀ���� ��, n=-1���ʿ� n=1�����ʿ�
				{
					//i*W+j: 1���� �迭���� i,j�� �ش��ϴ� ��ġ indexing
					//i:�� j:��
					//Y*W+X(Y:Y��ǥ, X:X��ǥ)
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];//(m��n�� -1���Ͷ�)
				}
			}
			//0~255���̰� ������ ���� ����
			//0~765(���񰪾���)������ ��-->0~255
			Out[i*W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Sobel_X_Conv(BYTE*Img, BYTE*Out, int W, int H)//Prewitt ����ũ X, ���� ��谡 �� �� ���´�.
{
	double Kernel[3][3] = { -1.0,0.0,1.0,
							-2.0,0.0,2.0,
							-1.0,0.0,1.0 };

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y��ǥ,��,���ι���, ���Ʒ����ʿ����� margin�� ���Ͽ� 1���ͽ���, -1��
	{
		for (int j = 1; j < W - 1; j++)//X��ǥ,��, ���ι���
		{
			//i,j:Ŀ���� ���� ��ġ
			for (int m = -1; m <= 1; m++)//Ŀ���� ��, m=-1����, m=0������, m=1�Ʒ���
			{
				for (int n = -1; n <= 1; n++)//Ŀ���� ��, n=-1���ʿ� n=1�����ʿ�
				{
					//i*W+j: 1���� �迭���� i,j�� �ش��ϴ� ��ġ indexing
					//i:�� j:��
					//Y*W+X(Y:Y��ǥ, X:X��ǥ)
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];//(m��n�� -1���Ͷ�)
				}
			}
			//0~255���̰� ������ ���� ����
			//0~1020(���񰪾���)������ ��-->0~255
			Out[i*W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Sobel_Y_Conv(BYTE*Img, BYTE*Out, int W, int H)//Prewitt ����ũ Y, ���� ��谡 �� �� ���´�.
{
	double Kernel[3][3] = { -1.0,-2.0,-1.0,
							0.0,0.0,0.0,
							1.0,2.0,1.0 };

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y��ǥ,��,���ι���, ���Ʒ����ʿ����� margin�� ���Ͽ� 1���ͽ���, -1��
	{
		for (int j = 1; j < W - 1; j++)//X��ǥ,��, ���ι���
		{
			//i,j:Ŀ���� ���� ��ġ
			for (int m = -1; m <= 1; m++)//Ŀ���� ��, m=-1����, m=0������, m=1�Ʒ���
			{
				for (int n = -1; n <= 1; n++)//Ŀ���� ��, n=-1���ʿ� n=1�����ʿ�
				{
					//i*W+j: 1���� �迭���� i,j�� �ش��ϴ� ��ġ indexing
					//i:�� j:��
					//Y*W+X(Y:Y��ǥ, X:X��ǥ)
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];//(m��n�� -1���Ͷ�)
				}
			}
			//0~255���̰� ������ ���� ����
			//0~1020(���񰪾���)������ ��-->0~255
			Out[i*W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Laplace_Conv(BYTE*Img, BYTE*Out, int W, int H)
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0, 
							-1.0, 8.0,-1.0, 
							-1.0, -1.0, -1.0};

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y��ǥ,��,���ι���, ���Ʒ����ʿ����� margin�� ���Ͽ� 1���ͽ���, -1��
	{
		for (int j = 1; j < W - 1; j++)//X��ǥ,��, ���ι���
		{
			//i,j:Ŀ���� ���� ��ġ
			for (int m = -1; m <= 1; m++)//Ŀ���� ��, m=-1����, m=0������, m=1�Ʒ���
			{
				for (int n = -1; n <= 1; n++)//Ŀ���� ��, n=-1���ʿ� n=1�����ʿ�
				{
					//i*W+j: 1���� �迭���� i,j�� �ش��ϴ� ��ġ indexing
					//i:�� j:��
					//Y*W+X(Y:Y��ǥ, X:X��ǥ)
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];//(m��n�� -1���Ͷ�)
				}
			}
			//0~255���̰� ������ ���� ����
			//0~2040(���񰪾���)������ ��-->0~255
			Out[i*W + j] = abs((long)SumProduct) / 8;
			SumProduct = 0.0;
		}
	}
}

void Laplace_Conv_DC(BYTE*Img, BYTE*Out, int W, int H)//Noise Ȯ����
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							-1.0, 9.0,-1.0,	//Ŀ���� ����1, ���� ������ ȭ�� ��� ������
							-1.0, -1.0, -1.0 };

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y��ǥ,��,���ι���, ���Ʒ����ʿ����� margin�� ���Ͽ� 1���ͽ���, -1��
	{
		for (int j = 1; j < W - 1; j++)//X��ǥ,��, ���ι���
		{
			//i,j:Ŀ���� ���� ��ġ
			for (int m = -1; m <= 1; m++)//Ŀ���� ��, m=-1����, m=0������, m=1�Ʒ���
			{
				for (int n = -1; n <= 1; n++)//Ŀ���� ��, n=-1���ʿ� n=1�����ʿ�
				{
					//i*W+j: 1���� �迭���� i,j�� �ش��ϴ� ��ġ indexing
					//i:�� j:��
					//Y*W+X(Y:Y��ǥ, X:X��ǥ)
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];//(m��n�� -1���Ͷ�)
				}
			}
			//0~255���̰� ������ ���� ����
			//0~2295(���񰪾���)������ ��, clipping
			if (SumProduct > 255.0)
				Out[i*W + j] = 255;
			else if (SumProduct < 0.0)
				Out[i*W + j] = 0;
			else
				Out[i*W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo, RGBQUAD*hRGB, 
	BYTE*Output, int W, int H, const char*FileName)
{
	//���� ����
	FILE* fp = fopen(FileName, "wb");		//���� ����->���� ����, wb: write binary
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);		//���: fwrite ����Ҷ��� 1byte������
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), W*H, fp);	//������ ȭ�� ���� ���, ���� ó���� ���
	fclose(fp);
}

void swap(BYTE* a, BYTE* b)
{
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

BYTE Median(BYTE*arr, int size)//3*3�̸� 9 ������ 5*5�̸� 25...
{
	// �������� ����, �������� �̿�(?)
	const int S = size;//���ͷ� ���
	for (int i = 0; i < size - 1; i++) // pivot index, �������� pivot�� �� �ʿ䰡 ����.
	{
		for (int j = i + 1; j < size; j++) // �񱳴�� index
		{
			if (arr[i] > arr[j]) 
				swap(&arr[i], &arr[j]);//&:call by reference, �ּ� �ѱ��
		}
	}
	return arr[S / 2];//�߰��� �ش��ϴ� index
}

BYTE MaxPooling(BYTE* arr, int size)
{
	// �������� ����
	const int S = size;//���ͷ� ���
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // �񱳴�� index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S - 1];//���� ū �� ���
}

BYTE MinPooling(BYTE* arr, int size)
{
	// �������� ����
	const int S = size;//���ͷ� ���
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // �񱳴�� index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[0];//���� ���� �� ���
}

//======================================================================================
int main()
{
	BITMAPFILEHEADER hf; // BMP ������� 14����Ʈ
	BITMAPINFOHEADER hInfo; // BMP ������� 40����Ʈ
	RGBQUAD hRGB[256]; // �ȷ�Ʈ (256*4Bytes) 1024����Ʈ

	FILE* fp;	//fp: �����͸� �аų� ������ �� �ʿ�
	//fp = fopen("coin.bmp", "rb");
	//fp = fopen("LENNA.bmp", "rb");
	fp = fopen("LENNA_impulse.bmp", "rb");
	//fp = fopen("LENNA_gauss.bmp", "rb");
	//rb: read binary, ���Ͽ��� ������ �о���ڴ�. (txt�϶� rt�̿�)
	//���� �����ʹ� �׻� �ּҳ� ��ġ�� ����Ų��. �� ù��° ����Ŵ.

	if (fp == NULL) //���ܰ˻�
	{
		printf("File not found!\n");
		return -1;
	}

	//fread�Լ��� ������ �о �޸� ������ ����
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);	//14Bytes��ŭ �о hf�� ����. ->���� ���� ��ġ ����Ŵ
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);	//40bytes��ŭ �о hinfo�� ����
	fread(hRGB, sizeof(RGBQUAD), 256, fp);			//4*256��ŭ �о hRGP�� ����
	//�迭�� �̸� ��ü�� �ּ��̹Ƿ� & �Ⱥ�����., RGPQUAD�� 4bytes�̹Ƿ� 256�� �ҷ��´�.

	//������ ȭ������ �б�
	int ImgSize = hInfo.biWidth*hInfo.biHeight;	//������ ���� ������*���� ������=������ ��ü ������
	BYTE*Image = (BYTE*)malloc(ImgSize);	//ImgSize��ŭ �����Ҵ�, Image: ���� ���� ȭ�� ����
	BYTE*Output = (BYTE*)malloc(ImgSize);	//Output: ������ ó���� ��� ����
	//malloc->�����Ҵ��� �޸��� ù��° �ּ� ��ȯ, void�� �����Ͷ� ���� ����ȯ�� �ʿ��ϴ�.
	//BYTE * Temp = (BYTE*)malloc(ImgSize);//�ӽ� �迭


	fread(Image, sizeof(BYTE), ImgSize, fp);//������ ȭ������, �� ��ġ�������� 1����Ʈ�� ImgSize��ŭ ���� file pointer���� �о����
	fclose(fp);	//���� �����Ϳ� ���ϰ��� ���踦 ���´�.
//==================================================================================
	
	//--------------------------------------------------
	//[�ǽ�]w2+[���� 2] 3�� �̹������� �����ϴ� ���α׷�
	/*
	BYTE * Output1 = (BYTE *)malloc(ImgSize);
	BYTE * Output2 = (BYTE *)malloc(ImgSize);
	BYTE * Output3 = (BYTE *)malloc(ImgSize);

	for (int i = 0; i < ImgSize; i++)
	{
		Output1[i] = Image[i];
		Output2[i] = Image[i] + 50;
		Output3[i] = 255 - Image[i];

	}

	fp = fopen("output1.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output1, sizeof(BYTE), ImgSize, fp);
	fclose(fp);
	//SaveBMPFile(hf, hInfo, hRGB, Output1, hInfo.biWidth, hInfo.biHeight, "output1.bmp");

	fp = fopen("output2.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output2, sizeof(BYTE), ImgSize, fp);
	fclose(fp);
	//SaveBMPFile(hf, hInfo, hRGB, Output2, hInfo.biWidth, hInfo.biHeight, "output2.bmp");

	fp = fopen("output3.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output3, sizeof(BYTE), ImgSize, fp);
	fclose(fp);
	//SaveBMPFile(hf, hInfo, hRGB, Output3, hInfo.biWidth, hInfo.biHeight, "output3.bmp");

	free(Image);
	free(Output1);
	free(Output2);
	free(Output3);*/
	//[�ǽ�]w2+[���� 2] 3�� �̹������� �����ϴ� ���α׷�
	//--------------------------------------------------

	//--------------------------------------------------
	//[�ǽ�]w3
	int Histo[256] = { 0 };
	int AHisto[256] = { 0 };

	/*
	//ObtainAHistogram(Histo, AHisto);
	//HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);
	
	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 0.5);
	*/
	//[�ǽ�]w3
	//--------------------------------------------------
	
	//--------------------------------------------------
	//[���� 3] Gonzalez �ڵ� ����ȭ �Ӱ�ġ ���� ���α׷� ����
	/*
	//ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	//int Thres = GozalezBinThresh(Image, Histo, hInfo.biWidth, hInfo.biHeight, 3);//���Ƿ�:3
	//Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);*/
	//[���� 3] Gonzalez �ڵ� ����ȭ �Ӱ�ġ ���� ���α׷� ����
	//--------------------------------------------------

	//--------------------------------------------------
	//[�ǽ�]w4
	/*
	//AverageConv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//GausianAvrConv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//Prewitt_X_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//Prewitt_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	
	//---X,Y�Ѵ� �ϰ���� ��
	Prewitt_X_Conv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
	Prewitt_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	for (int i = 0; i < ImgSize; i++)
	{
		if (Temp[i] > Output[i])
			Output[i] = Temp[i];//Temp�� ���(X���� ���)�� �� �ѷ��ϸ� �̸� �ݿ�
	}
	
	//Laplace_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);

	//GausianAvrConv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
	//Laplace_Conv_DC(Temp, Output, hInfo.biWidth, hInfo.biHeight);//Noise ���� �� ��ȭ
	//Binarization(Output, Output, hInfo.biWidth, hInfo.biHeight, 40);
	*/
	//[�ǽ�]w4
	//--------------------------------------------------

	//--------------------------------------------------
	//[�ǽ�]w5- Median filtering
	//Median Filtering
	/*
	BYTE temp[9];
	int W = hInfo.biWidth, H = hInfo.biHeight;
	int i, j;
	for (i = 1; i < H - 1; i++) {		//i: ���� ��
		for (j = 1; j < W - 1; j++) {	//j: ���� ��
			temp[0] = Image[(i - 1) * W + j-1];
			temp[1] = Image[(i - 1) * W + j];
			temp[2] = Image[(i - 1) * W + j+1];
			temp[3] = Image[i * W + j-1];
			temp[4] = Image[i * W + j]; //center, ���� ��ġ i,j
			temp[5] = Image[i * W + j+1];
			temp[6] = Image[(i + 1) * W + j-1];
			temp[7] = Image[(i + 1) * W + j];
			temp[8] = Image[(i + 1) * W + j+1];

			//Output[i * W + j] = Median(temp, 9); //Median: �߰��� �ش��ϴ� �� ��ȯ
			//Output[i * W + j] = MaxPooling(temp, 9);//���� ȭ�Ұ���(salt noise)�� �þ ���̴�.
			Output[i * W + j] = MinPooling(temp, 9);//pepper noise�� ������ ���̴�.
		}
	}
	*/
	
	//AverageConv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//[�ǽ�] w5 ��
	//--------------------------------------------------

	//#################################################################
	//#################################################################
	//#################################################################�̹�����
	//#################################################################
	//--------------------------------------------------
	//[���� w5]Median filter ����� ���� ��� ��
	// Median filtering

	//int Length = 5;  // ����ũ�� �� ���� ����
	//int Length = 7;  // ����ũ�� �� ���� ����
	int Length = 9;  // ����ũ�� �� ���� ����
		
	int Margin = Length / 2; //���� �� ������ ó������ �ʵ��� �ϱ� ���Ͽ� Margin�� �����Ѵ�.

	int WSize = Length * Length;//���� ����� Wsize�� �����Ѵ�. (���ʹ� ���簢���̹Ƿ� �Ѻ��� ���� ����)

	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize);//median filtering�� �ϱ� ���� ���� �����ŭ�� temp��� �迭�� �����Ҵ��Ѵ�.

	int W = hInfo.biWidth, H = hInfo.biHeight;	//������ width�� height�� W�� H�� �����Ѵ�.

	int i, j, m, n;//��������

	for (i = Margin; i < H - Margin; i++) { //������ ��(Y��ǥ),���� �������� �̵�, Margin�� ����Ͽ� �������� ������ ���� ����

		for (j = Margin; j < W - Margin; j++) { //������ ��(X��ǥ),���� �������� �̵�, Margin�� ����Ͽ� �������� ������ ���� ����

			for (m = -Margin; m <= Margin; m++) {//temp �迭������ ��, m=0�� �� center��, m<0�϶� center�� ���� ��, m>0�� ���� �Ʒ��� ��

				for (n = -Margin; n <= Margin; n++) { //temp �迭������ ��, n=0�� �� center��, n<0�϶� center�� ���� ��, n>0�� ���� ������ ��

					temp[(m + Margin) * Length + (n + Margin)] = Image[(i + m)*W + j + n];//������ i�� j��(m=0,n=0�� ��)�� �� �ֺ��� ������ temp �迭�� ����

				}

			}

			Output[i * W + j] = Median(temp, WSize);//Median �Լ��� ���� ��ȯ�� Median ���� Output �迭�� i��j�� ��ġ�� ����

		}

	}
	//SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "median_5.bmp");
	//SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "median_7.bmp");
	SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "median_9.bmp");
	free(temp);

	// Median filtering 

	//[���� w5]Median filter ����� ���� ��� ��
	//--------------------------------------------------
	//#################################################################
	//#################################################################
	//#################################################################
	//#################################################################
	//==================================================================================

	//SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "output.bmp");
	free(Image);
	free(Output);
	//free(Temp);
	return 0;
}
