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

//*********************************************************//
//����3 Gonzalez �ڵ� ����ȭ �Ӱ�ġ ���� ���α׷� ���� �Լ�//
int DivisionArea(int T, int * Histo)//���� ������
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

int GozalezBinThresh(BYTE * Img, int * Histo, int W, int H,int epsilon)//�Ӱ�ġ ã��
{
	BYTE Low=0 ,High=0;
	BYTE init_T,T;//�Ӱ�ġ
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) { //������ ����
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
//����3 Gonzalez �ڵ� ����ȭ �Ӱ�ġ ���� ���α׷� ���� �Լ�//
//*********************************************************//

BYTE DetermThGonzalez(int*H)//���� 3 ���ô��
{
	BYTE ep = 3;
	BYTE Low, High;
	BYTE Th, NewTh;
	int G1 = 0, G2 = 0, cnt1 = 0, cnt2 = 0, mu1, mu2;
	
	for (int i = 0; i < 256; i++)
	{
		if (H[i] != 0)
		{
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--)
	{
		if (H[i] != 0)
		{
			High = i;
			break;
		}
	}
	Th = (Low + High / 2);
		
	//printf("%d\n",Th);

	while (1)
	{
		for (int i = Th + 1; i <= High; i++)
		{
			G1 += (H[i] * i);
			cnt1 += H[i];
		}
		for (int i = Low; i <= Th; i++)
		{
			G2 += (H[i] * i);
			cnt2 += H[i];
		}
		if (cnt1 == 0)
			cnt1 = 1;
		if (cnt2 == 0)
			cnt2 = 1;

		mu1 = G1 / cnt1;
		mu2 = G2 / cnt2;

		NewTh = (mu1 + mu2) / 2;

		if (abs(NewTh - Th) < ep)
		{
			Th = NewTh;
			break;
		}
		else
		{
			Th = NewTh;
		}
		G1 = G2 = cnt1 = cnt2 = 0;
		//printf("%d\n",Th);
	}
	return Th;

}

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
	
	if (hInfo.biBitCount == 24) {//Ʈ���÷�
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(Output, sizeof(BYTE), W * H * 3, fp);
	}
	else if (hInfo.biBitCount == 8) {//�׷���
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
		fwrite(Output, sizeof(BYTE), W * H, fp);
	}
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

int push(short* stackx, short* stacky, int arr_size, short vx, short vy, int* top)
{
	if (*top >= arr_size) return(-1);
	(*top)++;
	stackx[*top] = vx;
	stacky[*top] = vy;
	return(1);
}

int pop(short* stackx, short* stacky, short* vx, short* vy, int* top)
{
	if (*top == 0) return(-1);
	*vx = stackx[*top];
	*vy = stacky[*top];
	(*top)--;
	return(1);
}


// GlassFire �˰����� �̿��� �󺧸� �Լ�
void m_BlobColoring(BYTE* CutImage, int height, int width)//Cutimage: main���� output�� �޾ƿ� ��
{
	int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
	long k;
	short curColor = 0, r, c;
	//	BYTE** CutImage2;
	Out_Area = 1;


	// �������� ����� �޸� �Ҵ�
	short* stackx = new short[height * width];
	short* stacky = new short[height * width];
	short* coloring = new short[height * width];//�� �� ��������?

	int arr_size = height * width;

	// �󺧸��� �ȼ��� �����ϱ� ���� �޸� �Ҵ�

	for (k = 0; k < height * width; k++) coloring[k] = 0;  // �޸� �ʱ�ȭ

	for (i = 0; i < height; i++)
	{
		index = i * width;
		for (j = 0; j < width; j++)
		{
			// �̹� �湮�� ���̰ų� �ȼ����� 255�� �ƴ϶�� ó�� ����
			if (coloring[index + j] != 0 || CutImage[index + j] != 255) continue;
			r = i; c = j; top = 0; area = 1;
			curColor++;

			while (1)
			{
			GRASSFIRE:
				for (m = r - 1; m <= r + 1; m++)
				{
					index = m * width;
					for (n = c - 1; n <= c + 1; n++)
					{
						//���� �ȼ��� �����踦 ����� ó�� ����
						if (m < 0 || m >= height || n < 0 || n >= width) continue;

						if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
						{
							coloring[index + n] = curColor; // ���� �󺧷� ��ũ
							if (push(stackx, stacky, arr_size, (short)m, (short)n, &top) == -1) continue;
							r = m; c = n; area++;
							goto GRASSFIRE;
						}
					}
				}
				if (pop(stackx, stacky, &r, &c, &top) == -1) break;
			}
			if (curColor < 1000) BlobArea[curColor] = area;
		}
	}

	float grayGap = 255.0f / (float)curColor;//curColor: �� component�� ����
	//���� �� component�� 25���� 10, 20,... �̷������� labeling�� �Ѵ�. 

	// ���� ������ ���� ������ ã�Ƴ��� ����  
	for (i = 1; i <= curColor; i++)//���⿡ �ߴ���, ����Ŀ� BlobArea�� ġ�� ������ �ش��ϴ� ũ�⸦ Ȯ���� �� �ִ�.
	{
		if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;
	}
	// CutImage �迭 Ŭ����~
	for (k = 0; k < width * height; k++) CutImage[k] = 255;//���

	// coloring�� ����� �󺧸� ����� (Out_Area�� �����) ������ ���� ū �͸� CutImage�� ����
	for (k = 0; k < width * height; k++)
	{
		if (coloring[k] == Out_Area) CutImage[k] = 0;  // ���� ū �͸� ����(size filtering)
		//if (BlobArea[coloring[k]] >500) CutImage[k] = 0;  // Ư�� ���� �̻� �Ǵ� ������ ���
		//CutImage[k] = (unsigned char)(coloring[k] * grayGap);//�󺧸� �� ����� �״�� ���
	}

	delete[] coloring;
	delete[] stackx;
	delete[] stacky;
}
// �󺧸� �� ���� ���� ������ ���ؼ��� �̾Ƴ��� �ڵ� ����

void BinaryImageEdgeDetection(BYTE*Bin, BYTE*Out, int W, int H)//���� ���󿡼� Edge ����
{

	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			if (Bin[i*W + j] == 0)//��, �� ǥ��, ���� ȭ�Ҷ��
			{
				//4 �ֺ�ȭ�� �˻�
				if (!(Bin[(i - 1)*W + j] == 0 && Bin[(i + 1)* W + j] == 0 && Bin[i*W + j - 1] == 0 && Bin[i * W + j + 1] == 0))//�����¿�-->4���� ȭ�� �� �ϳ��� ������ �ƴ϶��
				{
					Out[i*W + j] = 255;
				}
			}
		}
	}

}


//***************************************************************//
//����6 �������� �����簢�� �� �����߽�����ϴ� ũ�ν����� �׸���//
void FindCenterOfGravity(BYTE*Img, int W, int H, int* XY)//�����߽� ã��
{
	int cnt = 0;
	for (int i = 0; i < H; i++)//y
	{
		for (int j = 0; j < W; j++)//x
		{
			if (Img[i*W + j] == 0)
			{
				XY[0] += j;
				XY[1] += i;
				cnt++;
			}
		}
	}
	if (cnt == 0)
		cnt = 1;
	XY[0] /= cnt;
	XY[1] /= cnt;
}
void FindLURD(BYTE*Img, int W, int H, int* LRXY)//���簢�� ������, ������ �Ʒ� ��ǥ ã��
{
	//LU_X, LU_Y,RD_X,RD_Y

	for (int j = 0; j < W; j++)//x
	{
		for (int i = 0; i < H; i++)//y
		{
			if (Img[i*W + j] == 0)
			{
				LRXY[0] = j;
				goto LU_Y;
			}
		}
	}
	

	LU_Y:
	for (int i = 0; i < H; i++)//y
	{
		for (int j = 0; j < W; j++)//x
		{
			if (Img[i*W + j]==0)
			{
				LRXY[1] = i;
				goto RD_X;
			}
		}
	}

	RD_X:
	for (int j = W-1; j >= 0; j--)//x
	{
		for (int i = H-1; i >=0 ; i--)//y
		{
			if (Img[i*W + j] == 0)
			{
				LRXY[2] = j;
				goto RD_Y;
			}
		}
	}


	RD_Y:
	for (int i = H - 1; i >= 0; i--)//y
	{
		for (int j = W - 1; j >= 0; j--)//x
		{
			if (Img[i*W + j] == 0)
			{
				LRXY[3] = i;
				goto EXIT;
			}
		}
	}
	EXIT:;

}

void DrawRectOutline(BYTE * Img, BYTE * Out, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y)//���� ���簢�� �׸���,�������XY, �����ϴ� XY
{
	for (int i = 0; i < H; i++)//y��
	{
		for (int j = 0; j < W; j++)//x��
		{
			if ((i == LU_Y - 1 && j >= LU_X - 1 && j <= RD_X + 1) || (j == LU_X - 1 && i >= LU_Y - 1 && i <= RD_Y + 1) || 
				(j == RD_X + 1 && i >= LU_Y - 1 && i <= RD_Y + 1) || (i == RD_Y + 1 && j >= LU_X - 1 && j <= RD_X + 1))//�����̹Ƿ� ������ 1�ȼ� �ٱ��� �簢�� �׸�
			{
				Out[i*W + j] = 255;
			}
			else
			{
				Out[i*W + j] = Img[i*W + j];
			}
		}
	}
}

void DrawCrossLine(BYTE * Img, BYTE * Out, int W, int H, int Cx, int Cy)//�����߽� ����ϴ� ����, ������ �׸���
{
	for (int i = 0; i < H; i++)//y��
	{
		for (int j = 0; j < W; j++)//x��
		{
			if (j == Cx || i == Cy)
			{
				Out[i*W + j] = 255;
			}
			else
			{
				Out[i*W + j] = Img[i*W + j];
			}
		}
	}
}
//����6 �������� �����簢�� �� �����߽�����ϴ� ũ�ν����� �׸���//
//***************************************************************//



void VerticalFlip(BYTE*Img, int W, int H)
{
	for (int i = 0; i < H / 2; i++)//y��ǥ
	{
		for (int j = 0; j < W; j++)//x��ǥ
		{
			swap(&Img[i*W + j], &Img[(H-1-i)*W+j]);
		}
	}
}


void HorizontalFlip(BYTE*Img, int W, int H)
{
	for (int i = 0; i < W / 2; i++)//x��ǥ
	{
		for (int j = 0; j < H; j++)//y��ǥ
		{
			swap(&Img[j*W + i], &Img[j*W+(W-1-i)]);
		}
	}
}

void Translation(BYTE*Image, BYTE*Output, int W, int H, int Tx, int Ty)
{
	Ty *= -1;//�츮�� �����ϴ� ��ǥ�� ���� y�� �̵�
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			if ((i + Ty < H&&i + Ty >= 0) && (j + Tx < W&&j + Tx >= 0)) //������ ������ ���� ������ ����� ������ ���´�.
			{
				//������
				Output[(i + Ty)*W + (j + Tx)] = Image[i*W + j];
			}
		}
	}
}

void Scaling(BYTE*Image, BYTE*Output, int W, int H, double SF_X, double SF_Y)
{
	int tmpX, tmpY;

	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			//������
			//tmpX =(int)(j * SF_X);
			//tmpY = (int)(i * SF_Y);
			//if (tmpY < H&&tmpX < W)
			//	Output[tmpY*W + tmpX] = Image[i*W + j];

			//������
			tmpX = (int)(j / SF_X);
			tmpY = (int)(i / SF_Y);
			if (tmpY < H&&tmpX < W)
				Output[i*W + j] = Image[tmpY*W + tmpX];
		}
	}
}

void Rotation(BYTE*Image, BYTE*Output, int W, int H, int Angle)//ȸ��
{
	int tmpX, tmpY;
	double Radian = Angle * 3.141592 / 180.0;
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			//������
			/*tmpX = (int)(cos(Radian)*j-sin(Radian)*i);
			tmpY = (int)(sin(Radian)*j + cos(Radian)*i);
			if ((tmpY < H&&tmpY >= 0) && (tmpX < W&&tmpX >= 0))
				Output[tmpY*W + tmpX] = Image[i*W + j];*/

				//������
			tmpX = (int)(cos(Radian)*j + sin(Radian)*i);
			tmpY = (int)(-sin(Radian)*j + cos(Radian)*i);
			if ((tmpY < H&&tmpY >= 0) && (tmpX < W&&tmpX >= 0))
				Output[i*W + j] = Image[tmpY*W + tmpX];
		}
	}
}

void Rotation2(BYTE*Image, BYTE*Output, int W, int H, int Angle, int C_X,int C_Y)//ȸ��,C_X, C_Y: ȸ���� �߽���
{
	int tmpX, tmpY;
	double Radian = Angle * 3.141592 / 180.0;
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
				//������
			tmpX = (int)(cos(Radian)*(j-C_X) + sin(Radian)*(i-C_Y)+C_X);
			tmpY = (int)(-sin(Radian)*(j-C_X) + cos(Radian)*(i-C_Y)+C_Y);
			if ((tmpY < H&&tmpY >= 0) && (tmpX < W&&tmpX >= 0))
				Output[i*W + j] = Image[tmpY*W + tmpX];
		}
	}
}

void MedianFiltering(BYTE*Image, BYTE*Output, int W, int H, int size)
{
	// Median filtering

	int Length = size;  // ����ũ�� �� ���� ����

	int Margin = Length / 2; //���� �� ������ ó������ �ʵ��� �ϱ� ���Ͽ� Margin�� �����Ѵ�.

	int WSize = Length * Length;//���� ����� Wsize�� �����Ѵ�. (���ʹ� ���簢���̹Ƿ� �Ѻ��� ���� ����)

	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize);//median filtering�� �ϱ� ���� ���� �����ŭ�� temp��� �迭�� �����Ҵ��Ѵ�.
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

}

void FillColor(BYTE* Image, int X, int Y, int W, int H, BYTE R, BYTE G, BYTE B)
{
	//RGB���ƴ϶� BGR�� ������!!
	Image[Y * W * 3 + X * 3] = B; // Blue ����, *3�� �ϴ� ������ Ʈ���÷� �����̱� ������(���ȼ�-->RGB)
	Image[Y * W * 3 + X * 3 + 1] = G; // Green ����
	Image[Y * W * 3 + X * 3 + 2] = R; // Red ����
}

//======================================================================================
//======================================================================================
int main()
{
	BITMAPFILEHEADER hf; // BMP ������� 14����Ʈ
	BITMAPINFOHEADER hInfo; // BMP ������� 40����Ʈ
	RGBQUAD hRGB[256]; // �ȷ�Ʈ (256*4Bytes) 1024����Ʈ

	FILE* fp;	//fp: �����͸� �аų� ������ �� �ʿ�
	//fp = fopen("coin.bmp", "rb");
	//fp = fopen("LENNA.bmp", "rb");
	//fp = fopen("LENNA_impulse.bmp", "rb");
	//fp = fopen("LENNA_gauss.bmp", "rb");
	//fp = fopen("pupil1.bmp", "rb");
	//fp = fopen("pupil2.bmp", "rb");
	fp = fopen("tcsample.bmp", "rb");
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
	BYTE* Image;
	BYTE* Output;
	int ImgSize = hInfo.biWidth*hInfo.biHeight;	//������ ���� ������*���� ������=������ ��ü ������
	if (hInfo.biBitCount == 24) { // Ʈ���÷�
		Image = (BYTE*)malloc(ImgSize * 3);
		Output = (BYTE*)malloc(ImgSize * 3);
		fread(Image, sizeof(BYTE), ImgSize * 3, fp);
	}
	else { // �ε���(�׷���)
		fread(hRGB, sizeof(RGBQUAD), 256, fp);
		Image = (BYTE*)malloc(ImgSize);
		Output = (BYTE*)malloc(ImgSize);
		fread(Image, sizeof(BYTE), ImgSize, fp);


	}
	//������ ȭ������ �б�
	/*BYTE*Image = (BYTE*)malloc(ImgSize);	//ImgSize��ŭ �����Ҵ�, Image: ���� ���� ȭ�� ����
	BYTE*Output = (BYTE*)malloc(ImgSize);	//Output: ������ ó���� ��� ����
	//malloc->�����Ҵ��� �޸��� ù��° �ּ� ��ȯ, void�� �����Ͷ� ���� ����ȯ�� �ʿ��ϴ�.
	BYTE * Temp = (BYTE*)malloc(ImgSize);//�ӽ� �迭												
	//fread(hRGB, sizeof(RGBQUAD), 256, fp);			//4*256��ŭ �о hRGP�� ����
	//�迭�� �̸� ��ü�� �ּ��̹Ƿ� & �Ⱥ�����., RGPQUAD�� 4bytes�̹Ƿ� 256�� �ҷ��´�.
	*/



	fread(Image, sizeof(BYTE), ImgSize, fp);//������ ȭ������, �� ��ġ�������� 1����Ʈ�� ImgSize��ŭ ���� file pointer���� �о����
	fclose(fp);	//���� �����Ϳ� ���ϰ��� ���踦 ���´�.
	int W = hInfo.biWidth;
	int H = hInfo.biHeight;
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

	//--------------------------------------------------
	//[���� w5]Median filter ����� ���� ��� ��
	/*
	
	MedianFiltering(Image,Output,W,H,11);
	*/
	//[���� w5]Median filter ����� ���� ��� �� ��
	//--------------------------------------------------

	//--------------------------------------------------
	//�ǽ� 6
	/*
	Binarization(Image, Temp, W, H, 50);
	InverseImage(Temp, Temp, W, H);//���� ������ ������ ��
	m_BlobColoring(Temp, H, W);//����ȭ�� ����,����,����
	for (int i = 0; i < ImgSize; i++)
		Output[i] = Image[i];// 255;
	BinaryImageEdgeDetection(Temp, Output, W, H);
	*/
	//--------------------------------------------------

	//--------------------------------------------------
	//[���� w6]�������� �����簢�� �� �����߽�����ϴ� ũ�ν����� �׸���
	/*
	Binarization(Image, Temp, W, H, 50);
	InverseImage(Temp, Temp, W, H);//���� ������ ������ ��
	m_BlobColoring(Temp, H, W);// �󺧸��Ͽ� ���� ���� ū �� ��ȯ
	int XY[2] = { 0,0 };//crossline �׸� X,Y
	FindCenterOfGravity(Temp, W, H, XY); //X,Yã��
	DrawCrossLine(Image, Output, W, H, XY[0], XY[1]);//crossline �׸���

	int LRXY[4] = { 0, };//���簢�� �׸� ���� ��� xy, ������ �ϴ� xy
	FindLURD(Temp, W, H, LRXY);//�� ���� x, y ��ǥ ã��
	//printf("%d %d %d %d", LRXY[0],LRXY[1], LRXY[2], LRXY[3]);
	DrawRectOutline(Output, Output, W, H, LRXY[0], LRXY[1], LRXY[2], LRXY[3]);//���簢�� �׸���
	
	*/									   
	//[���� w6]�������� �����簢�� �� �����߽�����ϴ� ũ�ν����� �׸��� ��
	//--------------------------------------------------

	//--------------------------------------------------
	//[w7]
	
	//3�� ���� �ؼ�
	/*
	BYTE Th;
	ObtainHistogram(Image, Histo, W, H);
	Th = DetermThGonzalez(Histo);
	Binarization(Image, Output, W, H, Th);
	*/
	
	//7���� �ǽ�
	//VerticalFlip(Image, W, H);
	//HorizontalFlip(Image, W, H);
	//Translation(Image, Output, W, H, 100, 40);
	//Scaling(Image, Output, W, H, 0.7, 0.7); //SF_X=SF_Y-->Uniform scaling
	//Rotation(Image, Output, W, H, 60);//������ �߽����� ȸ��
	//Rotation2(Image, Output, W, H, 60,128,128);//128,128�� �߽����� ȸ��

	//--------------------------------------------------

	//--------------------------------------------------
	//10���� �ǽ�
	// (50, 40)��ġ�� Ư�� �������� 
	/*
	FillColor(Image, 100, 200, W, H, 0, 255, 255);
	for (int i = 0; i < W; i++) {
		FillColor(Image, i, 200, W, H, 170, 80, 155);
	}*/

	// (50, 100) ~ (300, 400) �ڽ� ä���
	/*
	for (int i = 100; i <= 400; i++) {		//y��ǥ
		for (int j = 50; j <= 300; j++) {	//x��ǥ
			FillColor(Image, j, i, W, H, 255, 0, 255);
		}
	}*/

	
	// ���� �� �����
	/*
	// �ʱ�ȭ
	for (int i = 0; i <  H; i++) {
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3] = 0;
			Image[i * W * 3 + j * 3 + 1] = 0;
			Image[i * W * 3 + j * 3 + 2] = 0;
		}
	}
	// y��ǥ ���� 0~239 (Red) //����ϸ� ���Ʒ��� �ٲ� �� �Ʒ��� Red�� �ȴ�.
	for (int i = 0; i < 240; i++) {
		for (int j = 0; j < W; j++) {
			Image[i*W*3 + j*3 + 2] = 255; //BGR�����̹Ƿ� +2�Ѱ� red����
		}
	}
	//120~239�� R�� G����ħ(�����)

	// y��ǥ ���� 120 ~ 359 (Green)
	for (int i = 120; i < 360; i++) {
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3 + 1] = 255;
		}
	}
	//240~359�� G�� B�� ��ħ(Cyan?)

	// y��ǥ ���� 240 ~ 479 (Blue)
	for (int i = 240; i < 480; i++) {
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3] = 255;
		}
	}
	*/
	
	//�׶��̼� �����
	// �׶��̼� ����� (B ~ R)
	double wt;	//weight
	for (int a = 0; a < 120; a++) {
		for (int i = 0; i < W; i++) {
			wt = i / (double)(W - 1);
			Image[a * W * 3 + i * 3] = (BYTE)(255 * (1.0 - wt));  // Blue
			Image[a * W * 3 + i * 3 + 1] = (BYTE)(255 * (1.0 - wt)); // Green
			Image[a * W * 3 + i * 3 + 2] = (BYTE)(255 * wt); // Red
		}
	}
	//==================================================================================

	SaveBMPFile(hf, hInfo, hRGB, Image, hInfo.biWidth, hInfo.biHeight, "gradation.bmp");
	free(Image);
	free(Output);
	//free(Temp);
	return 0;
}
