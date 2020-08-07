#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>//비트맵 파일헤더 인포헤더 팔레트에 해당하는 구조체 정보를 가짐
#include <math.h>//abs

// 2차원 배열 동적할당 위함
unsigned char** imageMatrix;
// 이진영상에서 
unsigned char blankPixel = 255, imagePixel = 0;

typedef struct {
	int row, col;
}pixel;

int getBlackNeighbours(int row, int col) {

	int i, j, sum = 0;

	for (i = -1; i <= 1; i++) {
		for (j = -1; j <= 1; j++) {
			if (i != 0 || j != 0)
				sum += (imageMatrix[row + i][col + j] == imagePixel);
		}
	}

	return sum;
}

int getBWTransitions(int row, int col) {
	return 	((imageMatrix[row - 1][col] == blankPixel && imageMatrix[row - 1][col + 1] == imagePixel)
		+ (imageMatrix[row - 1][col + 1] == blankPixel && imageMatrix[row][col + 1] == imagePixel)
		+ (imageMatrix[row][col + 1] == blankPixel && imageMatrix[row + 1][col + 1] == imagePixel)
		+ (imageMatrix[row + 1][col + 1] == blankPixel && imageMatrix[row + 1][col] == imagePixel)
		+ (imageMatrix[row + 1][col] == blankPixel && imageMatrix[row + 1][col - 1] == imagePixel)
		+ (imageMatrix[row + 1][col - 1] == blankPixel && imageMatrix[row][col - 1] == imagePixel)
		+ (imageMatrix[row][col - 1] == blankPixel && imageMatrix[row - 1][col - 1] == imagePixel)
		+ (imageMatrix[row - 1][col - 1] == blankPixel && imageMatrix[row - 1][col] == imagePixel));
}

int zhangSuenTest1(int row, int col) { //세션화 알고리즘
	int neighbours = getBlackNeighbours(row, col);

	return ((neighbours >= 2 && neighbours <= 6)
		&& (getBWTransitions(row, col) == 1)
		&& (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel || imageMatrix[row + 1][col] == blankPixel)
		&& (imageMatrix[row][col + 1] == blankPixel || imageMatrix[row + 1][col] == blankPixel || imageMatrix[row][col - 1] == blankPixel));
}

int zhangSuenTest2(int row, int col) {
	int neighbours = getBlackNeighbours(row, col);

	return ((neighbours >= 2 && neighbours <= 6)
		&& (getBWTransitions(row, col) == 1)
		&& (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel || imageMatrix[row][col - 1] == blankPixel)
		&& (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row + 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel));
}

void zhangSuen(unsigned char* image, unsigned char* output, int rows, int cols) {

	int startRow = 1, startCol = 1, endRow, endCol, i, j, count, processed;

	pixel* markers;

	imageMatrix = (unsigned char**)malloc(rows * sizeof(unsigned char*));

	for (i = 0; i < rows; i++) {
		imageMatrix[i] = (unsigned char*)malloc((cols + 1) * sizeof(unsigned char));
		for (int k = 0; k < cols; k++) imageMatrix[i][k] = image[i * cols + k];
	}

	endRow = rows - 2;
	endCol = cols - 2;
	do {
		markers = (pixel*)malloc((endRow - startRow + 1) * (endCol - startCol + 1) * sizeof(pixel));
		count = 0;

		for (i = startRow; i <= endRow; i++) {
			for (j = startCol; j <= endCol; j++) {
				if (imageMatrix[i][j] == imagePixel && zhangSuenTest1(i, j) == 1) {
					markers[count].row = i;
					markers[count].col = j;
					count++;
				}
			}
		}

		processed = (count > 0);

		for (i = 0; i < count; i++) {
			imageMatrix[markers[i].row][markers[i].col] = blankPixel;
		}

		free(markers);
		markers = (pixel*)malloc((endRow - startRow + 1) * (endCol - startCol + 1) * sizeof(pixel));
		count = 0;

		for (i = startRow; i <= endRow; i++) {
			for (j = startCol; j <= endCol; j++) {
				if (imageMatrix[i][j] == imagePixel && zhangSuenTest2(i, j) == 1) {
					markers[count].row = i;
					markers[count].col = j;
					count++;
				}
			}
		}

		if (processed == 0)
			processed = (count > 0);

		for (i = 0; i < count; i++) {
			imageMatrix[markers[i].row][markers[i].col] = blankPixel;
		}

		free(markers);
	} while (processed == 1);


	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			output[i * cols + j] = imageMatrix[i][j];
		}
	}
}

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

//*********************************************************//
//과제3 Gonzalez 자동 이진화 임계치 결정 프로그램 구현 함수//
int DivisionArea(int T, int * Histo)//영역 나누기
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

int GozalezBinThresh(BYTE * Img, int * Histo, int W, int H,int epsilon)//임계치 찾기
{
	BYTE Low=0 ,High=0;
	BYTE init_T,T;//임계치
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) { //영상이 있음
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
//과제3 Gonzalez 자동 이진화 임계치 결정 프로그램 구현 함수//
//*********************************************************//

BYTE DetermThGonzalez(int*H)//과제 3 예시답안
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

void AverageConv(BYTE*Img, BYTE*Out, int W, int H)//박스 평활화
{
	double Kernel[3][3] = { 0.1111 ,0.1111 ,0.1111 ,
						0.1111 ,0.1111 ,0.1111 ,
						0.1111 ,0.1111 ,0.1111 };

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y좌표,행,세로방향, 위아래왼쪽오른쪽 margin을 위하여 1부터시작, -1함
	{
		for (int j = 1; j < W-1; j++)//X좌표,열, 가로방향
		{
			//i,j:커널의 센터 위치
			for (int m = -1; m <= 1; m++)//커널의 행, m=-1윗행, m=0센터행, m=1아래행
			{
				for (int n = -1; n <= 1; n++)//커널의 열, n=-1왼쪽열 n=1오른쪽열
				{
					//i*W+j: 1차원 배열에서 i,j에 해당하는 위치 indexing
					//i:행 j:열
					//Y*W+X(Y:Y좌표, X:X좌표)
					SumProduct += Img[(i+m)*W+(j+n)] * Kernel[m+1][n+1];//(m과n이 -1부터라서)
				}
			}
			Out[i*W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void GausianAvrConv(BYTE*Img, BYTE*Out, int W, int H)//가우시안 평활화,센터에 가중치 제일 많이 준다.
{
	double Kernel[3][3] = { 0.0625 ,0.125 ,0.0625 ,
						0.125 ,0.25 ,0.125 ,
						0.06251 ,0.125 ,0.0625 };

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y좌표,행,세로방향, 위아래왼쪽오른쪽 margin을 위하여 1부터시작, -1함
	{
		for (int j = 1; j < W - 1; j++)//X좌표,열, 가로방향
		{
			//i,j:커널의 센터 위치
			for (int m = -1; m <= 1; m++)//커널의 행, m=-1윗행, m=0센터행, m=1아래행
			{
				for (int n = -1; n <= 1; n++)//커널의 열, n=-1왼쪽열 n=1오른쪽열
				{
					//i*W+j: 1차원 배열에서 i,j에 해당하는 위치 indexing
					//i:행 j:열
					//Y*W+X(Y:Y좌표, X:X좌표)
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];//(m과n이 -1부터라서)
				}
			}
			Out[i*W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_X_Conv(BYTE*Img, BYTE*Out, int W, int H)//Prewitt 마스크 X, 세로 경계가 더 잘 나온다.
{
	double Kernel[3][3] = { -1.0,0.0,1.0,
							-1.0,0.0,1.0, 
							-1.0,0.0,1.0};

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y좌표,행,세로방향, 위아래왼쪽오른쪽 margin을 위하여 1부터시작, -1함
	{
		for (int j = 1; j < W - 1; j++)//X좌표,열, 가로방향
		{
			//i,j:커널의 센터 위치
			for (int m = -1; m <= 1; m++)//커널의 행, m=-1윗행, m=0센터행, m=1아래행
			{
				for (int n = -1; n <= 1; n++)//커널의 열, n=-1왼쪽열 n=1오른쪽열
				{
					//i*W+j: 1차원 배열에서 i,j에 해당하는 위치 indexing
					//i:행 j:열
					//Y*W+X(Y:Y좌표, X:X좌표)
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];//(m과n이 -1부터라서)
				}
			}
			//0~255사이가 나오지 않을 것임
			//0~765(절댓값씌움)까지의 값-->0~255
			Out[i*W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_Y_Conv(BYTE*Img, BYTE*Out, int W, int H)//Prewitt 마스크 Y, 가로 경계가 더 잘 나온다.
{
	double Kernel[3][3] = { -1.0,-1.0,-1.0,
							0.0,0.0,0.0,
							1.0,1.0,1.0 };

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y좌표,행,세로방향, 위아래왼쪽오른쪽 margin을 위하여 1부터시작, -1함
	{
		for (int j = 1; j < W - 1; j++)//X좌표,열, 가로방향
		{
			//i,j:커널의 센터 위치
			for (int m = -1; m <= 1; m++)//커널의 행, m=-1윗행, m=0센터행, m=1아래행
			{
				for (int n = -1; n <= 1; n++)//커널의 열, n=-1왼쪽열 n=1오른쪽열
				{
					//i*W+j: 1차원 배열에서 i,j에 해당하는 위치 indexing
					//i:행 j:열
					//Y*W+X(Y:Y좌표, X:X좌표)
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];//(m과n이 -1부터라서)
				}
			}
			//0~255사이가 나오지 않을 것임
			//0~765(절댓값씌움)까지의 값-->0~255
			Out[i*W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Sobel_X_Conv(BYTE*Img, BYTE*Out, int W, int H)//Prewitt 마스크 X, 세로 경계가 더 잘 나온다.
{
	double Kernel[3][3] = { -1.0,0.0,1.0,
							-2.0,0.0,2.0,
							-1.0,0.0,1.0 };

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y좌표,행,세로방향, 위아래왼쪽오른쪽 margin을 위하여 1부터시작, -1함
	{
		for (int j = 1; j < W - 1; j++)//X좌표,열, 가로방향
		{
			//i,j:커널의 센터 위치
			for (int m = -1; m <= 1; m++)//커널의 행, m=-1윗행, m=0센터행, m=1아래행
			{
				for (int n = -1; n <= 1; n++)//커널의 열, n=-1왼쪽열 n=1오른쪽열
				{
					//i*W+j: 1차원 배열에서 i,j에 해당하는 위치 indexing
					//i:행 j:열
					//Y*W+X(Y:Y좌표, X:X좌표)
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];//(m과n이 -1부터라서)
				}
			}
			//0~255사이가 나오지 않을 것임
			//0~1020(절댓값씌움)까지의 값-->0~255
			Out[i*W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Sobel_Y_Conv(BYTE*Img, BYTE*Out, int W, int H)//Prewitt 마스크 Y, 가로 경계가 더 잘 나온다.
{
	double Kernel[3][3] = { -1.0,-2.0,-1.0,
							0.0,0.0,0.0,
							1.0,2.0,1.0 };

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y좌표,행,세로방향, 위아래왼쪽오른쪽 margin을 위하여 1부터시작, -1함
	{
		for (int j = 1; j < W - 1; j++)//X좌표,열, 가로방향
		{
			//i,j:커널의 센터 위치
			for (int m = -1; m <= 1; m++)//커널의 행, m=-1윗행, m=0센터행, m=1아래행
			{
				for (int n = -1; n <= 1; n++)//커널의 열, n=-1왼쪽열 n=1오른쪽열
				{
					//i*W+j: 1차원 배열에서 i,j에 해당하는 위치 indexing
					//i:행 j:열
					//Y*W+X(Y:Y좌표, X:X좌표)
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];//(m과n이 -1부터라서)
				}
			}
			//0~255사이가 나오지 않을 것임
			//0~1020(절댓값씌움)까지의 값-->0~255
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
	for (int i = 1; i < H - 1; i++)//Y좌표,행,세로방향, 위아래왼쪽오른쪽 margin을 위하여 1부터시작, -1함
	{
		for (int j = 1; j < W - 1; j++)//X좌표,열, 가로방향
		{
			//i,j:커널의 센터 위치
			for (int m = -1; m <= 1; m++)//커널의 행, m=-1윗행, m=0센터행, m=1아래행
			{
				for (int n = -1; n <= 1; n++)//커널의 열, n=-1왼쪽열 n=1오른쪽열
				{
					//i*W+j: 1차원 배열에서 i,j에 해당하는 위치 indexing
					//i:행 j:열
					//Y*W+X(Y:Y좌표, X:X좌표)
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];//(m과n이 -1부터라서)
				}
			}
			//0~255사이가 나오지 않을 것임
			//0~2040(절댓값씌움)까지의 값-->0~255
			Out[i*W + j] = abs((long)SumProduct) / 8;
			SumProduct = 0.0;
		}
	}
}

void Laplace_Conv_DC(BYTE*Img, BYTE*Out, int W, int H)//Noise 확인차
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							-1.0, 9.0,-1.0,	//커널의 합이1, 원래 영상의 화소 밝기 유지됨
							-1.0, -1.0, -1.0 };

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++)//Y좌표,행,세로방향, 위아래왼쪽오른쪽 margin을 위하여 1부터시작, -1함
	{
		for (int j = 1; j < W - 1; j++)//X좌표,열, 가로방향
		{
			//i,j:커널의 센터 위치
			for (int m = -1; m <= 1; m++)//커널의 행, m=-1윗행, m=0센터행, m=1아래행
			{
				for (int n = -1; n <= 1; n++)//커널의 열, n=-1왼쪽열 n=1오른쪽열
				{
					//i*W+j: 1차원 배열에서 i,j에 해당하는 위치 indexing
					//i:행 j:열
					//Y*W+X(Y:Y좌표, X:X좌표)
					SumProduct += Img[(i + m)*W + (j + n)] * Kernel[m + 1][n + 1];//(m과n이 -1부터라서)
				}
			}
			//0~255사이가 나오지 않을 것임
			//0~2295(절댓값씌움)까지의 값, clipping
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
	//파일 저장
	FILE* fp = fopen(FileName, "wb");		//없는 파일->새로 생성, wb: write binary
	
	if (hInfo.biBitCount == 24) {//트루컬러
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(Output, sizeof(BYTE), W * H * 3, fp);
	}
	else if (hInfo.biBitCount == 8) {//그레이
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

BYTE Median(BYTE*arr, int size)//3*3이면 9 들어오고 5*5이면 25...
{
	// 오름차순 정렬, 버블정렬 이용(?)
	const int S = size;//리터럴 상수
	for (int i = 0; i < size - 1; i++) // pivot index, 마지막껀 pivot이 될 필요가 없다.
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 
				swap(&arr[i], &arr[j]);//&:call by reference, 주소 넘긴다
		}
	}
	return arr[S / 2];//중간에 해당하는 index
}

BYTE MaxPooling(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;//리터럴 상수
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S - 1];//가장 큰 값 출력
}

BYTE MinPooling(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;//리터럴 상수
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[0];//가장 작은 값 출력
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


// GlassFire 알고리즘을 이용한 라벨링 함수
void m_BlobColoring(BYTE* CutImage, int height, int width)//Cutimage: main에서 output을 받아온 것
{
	int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
	long k;
	short curColor = 0, r, c;
	//	BYTE** CutImage2;
	Out_Area = 1;


	// 스택으로 사용할 메모리 할당
	short* stackx = new short[height * width];
	short* stacky = new short[height * width];
	short* coloring = new short[height * width];//몇 번 영역인지?

	int arr_size = height * width;

	// 라벨링된 픽셀을 저장하기 위해 메모리 할당

	for (k = 0; k < height * width; k++) coloring[k] = 0;  // 메모리 초기화

	for (i = 0; i < height; i++)
	{
		index = i * width;
		for (j = 0; j < width; j++)
		{
			// 이미 방문한 점이거나 픽셀값이 255가 아니라면 처리 안함
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
						//관심 픽셀이 영상경계를 벗어나면 처리 안함
						if (m < 0 || m >= height || n < 0 || n >= width) continue;

						if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
						{
							coloring[index + n] = curColor; // 현재 라벨로 마크
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

	float grayGap = 255.0f / (float)curColor;//curColor: 총 component의 개수
	//예를 들어서 component가 25개면 10, 20,... 이런식으로 labeling을 한다. 

	// 가장 면적이 넓은 영역을 찾아내기 위함  
	for (i = 1; i <= curColor; i++)//여기에 중단점, 조사식에 BlobArea를 치면 영역에 해당하는 크기를 확인할 수 있다.
	{
		if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;
	}
	// CutImage 배열 클리어~
	for (k = 0; k < width * height; k++) CutImage[k] = 255;//배경

	// coloring에 저장된 라벨링 결과중 (Out_Area에 저장된) 영역이 가장 큰 것만 CutImage에 저장
	for (k = 0; k < width * height; k++)
	{
		if (coloring[k] == Out_Area) CutImage[k] = 0;  // 가장 큰 것만 저장(size filtering)
		//if (BlobArea[coloring[k]] >500) CutImage[k] = 0;  // 특정 면적 이상 되는 영역만 출력
		//CutImage[k] = (unsigned char)(coloring[k] * grayGap);//라벨링 된 결과를 그대로 출력
	}

	delete[] coloring;
	delete[] stackx;
	delete[] stacky;
}
// 라벨링 후 가장 넓은 영역에 대해서만 뽑아내는 코드 포함

void BinaryImageEdgeDetection(BYTE*Bin, BYTE*Out, int W, int H)//이진 영상에서 Edge 검출
{

	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			if (Bin[i*W + j] == 0)//행, 열 표기, 전경 화소라면
			{
				//4 주변화소 검사
				if (!(Bin[(i - 1)*W + j] == 0 && Bin[(i + 1)* W + j] == 0 && Bin[i*W + j - 1] == 0 && Bin[i * W + j + 1] == 0))//상하좌우-->4방향 화소 중 하나라도 전경이 아니라면
				{
					Out[i*W + j] = 255;
				}
			}
		}
	}

}


//***************************************************************//
//과제6 동공영역 외접사각형 및 무게중심통과하는 크로스라인 그리기//
void FindCenterOfGravity(BYTE*Img, int W, int H, int* XY)//무게중심 찾기
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
void FindLURD(BYTE*Img, int W, int H, int* LRXY)//직사각형 왼쪽위, 오른쪽 아래 좌표 찾기
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

void FindLURDColor(BYTE*Img, int W, int H, int* LRXY)//직사각형 왼쪽위, 오른쪽 아래 좌표 찾기
{
	//LU_X, LU_Y,RD_X,RD_Y

	for (int j = 0; j < W; j++)//x
	{
		for (int i = 0; i < H; i++)//y
		{
			if (Img[i*W * 3 + j * 3] != 0 || Img[i*W * 3 + j * 3 + 1] != 0 || Img[i*W * 3 + j * 3 + 2] != 0)
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
			if (Img[i*W * 3 + j * 3] != 0 || Img[i*W * 3 + j * 3 + 1] != 0 || Img[i*W * 3 + j * 3 + 2] != 0)
			{
				LRXY[1] = i;
				goto RD_X;
			}
		}
	}

RD_X:
	for (int j = W - 1; j >= 0; j--)//x
	{
		for (int i = H - 1; i >= 0; i--)//y
		{
			if (Img[i*W * 3 + j * 3] != 0 || Img[i*W * 3 + j * 3 + 1] != 0 || Img[i*W * 3 + j * 3 + 2] != 0)
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
			if (Img[i*W * 3 + j * 3] != 0 || Img[i*W * 3 + j * 3 + 1] != 0 || Img[i*W * 3 + j * 3 + 2] != 0)
			{
				LRXY[3] = i;
				goto EXIT;
			}
		}
	}
EXIT:;

}

void DrawRectOutline(BYTE * Img, BYTE * Out, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y)//외접 직사각형 그리기,좌측상단XY, 우측하단 XY
{
	for (int i = 0; i < H; i++)//y축
	{
		for (int j = 0; j < W; j++)//x축
		{
			if ((i == LU_Y - 1 && j >= LU_X - 1 && j <= RD_X + 1) || (j == LU_X - 1 && i >= LU_Y - 1 && i <= RD_Y + 1) || 
				(j == RD_X + 1 && i >= LU_Y - 1 && i <= RD_Y + 1) || (i == RD_Y + 1 && j >= LU_X - 1 && j <= RD_X + 1))//외접이므로 점보다 1픽셀 바깥에 사각형 그림
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

void DrawRectOutlineColor(BYTE * Img, BYTE * Out, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y)//외접 직사각형 그리기,좌측상단XY, 우측하단 XY
{
	for (int i = 0; i < H; i++)//y축
	{
		for (int j = 0; j < W; j++)//x축
		{
			if ((i == LU_Y - 1 && j >= LU_X - 1 && j <= RD_X + 1) || (j == LU_X - 1 && i >= LU_Y - 1 && i <= RD_Y + 1) ||
				(j == RD_X + 1 && i >= LU_Y - 1 && i <= RD_Y + 1) || (i == RD_Y + 1 && j >= LU_X - 1 && j <= RD_X + 1))//외접이므로 점보다 1픽셀 바깥에 사각형 그림
			{
				Out[i*W * 3 + j * 3 + 2] = 255;
				
			}
			else
			{
				Out[i*W * 3 + j * 3] = Img[i*W * 3 + j * 3];
				Out[i*W * 3 + j * 3 + 1] = Img[i*W * 3 + j * 3 + 1];
				Out[i*W * 3 + j * 3 + 2] = Img[i*W * 3 + j * 3 + 2];
			}
		}
	}
}


void DrawCrossLine(BYTE * Img, BYTE * Out, int W, int H, int Cx, int Cy)//무게중심 통과하는 수평선, 수직선 그리기
{
	for (int i = 0; i < H; i++)//y축
	{
		for (int j = 0; j < W; j++)//x축
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
//과제6 동공영역 외접사각형 및 무게중심통과하는 크로스라인 그리기//
//***************************************************************//



void VerticalFlip(BYTE*Img, int W, int H)
{
	for (int i = 0; i < H / 2; i++)//y좌표
	{
		for (int j = 0; j < W; j++)//x좌표
		{
			swap(&Img[i*W + j], &Img[(H-1-i)*W+j]);
		}
	}
}


void HorizontalFlip(BYTE*Img, int W, int H)
{
	for (int i = 0; i < W / 2; i++)//x좌표
	{
		for (int j = 0; j < H; j++)//y좌표
		{
			swap(&Img[j*W + i], &Img[j*W+(W-1-i)]);
		}
	}
}

void Translation(BYTE*Image, BYTE*Output, int W, int H, int Tx, int Ty)
{
	Ty *= -1;//우리가 생각하는 좌표계 기준 y축 이동
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			if ((i + Ty < H&&i + Ty >= 0) && (j + Tx < W&&j + Tx >= 0)) //없으면 영상의 원래 범위를 벗어나는 오류가 나온다.
			{
				//순방향
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
			//순방향
			//tmpX =(int)(j * SF_X);
			//tmpY = (int)(i * SF_Y);
			//if (tmpY < H&&tmpX < W)
			//	Output[tmpY*W + tmpX] = Image[i*W + j];

			//역방향
			tmpX = (int)(j / SF_X);
			tmpY = (int)(i / SF_Y);
			if (tmpY < H&&tmpX < W)
				Output[i*W + j] = Image[tmpY*W + tmpX];
		}
	}
}

void Rotation(BYTE*Image, BYTE*Output, int W, int H, int Angle)//회전
{
	int tmpX, tmpY;
	double Radian = Angle * 3.141592 / 180.0;
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			//순방향
			/*tmpX = (int)(cos(Radian)*j-sin(Radian)*i);
			tmpY = (int)(sin(Radian)*j + cos(Radian)*i);
			if ((tmpY < H&&tmpY >= 0) && (tmpX < W&&tmpX >= 0))
				Output[tmpY*W + tmpX] = Image[i*W + j];*/

				//역방향
			tmpX = (int)(cos(Radian)*j + sin(Radian)*i);
			tmpY = (int)(-sin(Radian)*j + cos(Radian)*i);
			if ((tmpY < H&&tmpY >= 0) && (tmpX < W&&tmpX >= 0))
				Output[i*W + j] = Image[tmpY*W + tmpX];
		}
	}
}

void Rotation2(BYTE*Image, BYTE*Output, int W, int H, int Angle, int C_X,int C_Y)//회전,C_X, C_Y: 회전할 중심점
{
	int tmpX, tmpY;
	double Radian = Angle * 3.141592 / 180.0;
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
				//역방향
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

	int Length = size;  // 마스크의 한 변의 길이

	int Margin = Length / 2; //영상 밖 영역은 처리되지 않도록 하기 위하여 Margin을 설정한다.

	int WSize = Length * Length;//필터 사이즈를 Wsize에 저장한다. (필터는 정사각형이므로 한변의 길이 제곱)

	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize);//median filtering을 하기 위해 필터 사이즈만큼의 temp라는 배열을 동적할당한다.
	int i, j, m, n;//변수선언

	for (i = Margin; i < H - Margin; i++) { //영상의 행(Y좌표),세로 방향으로 이동, Margin을 고려하여 시작점과 끝나는 지점 지정

		for (j = Margin; j < W - Margin; j++) { //영상의 열(X좌표),가로 방향으로 이동, Margin을 고려하여 시작점과 끝나는 지점 지정

			for (m = -Margin; m <= Margin; m++) {//temp 배열에서의 행, m=0일 때 center행, m<0일때 center의 위쪽 행, m>0일 때는 아래쪽 행

				for (n = -Margin; n <= Margin; n++) { //temp 배열에서의 열, n=0일 때 center열, n<0일때 center의 왼쪽 열, n>0일 때는 오른쪽 열

					temp[(m + Margin) * Length + (n + Margin)] = Image[(i + m)*W + j + n];//영상의 i행 j열(m=0,n=0일 때)과 그 주변의 값들을 temp 배열에 복사

				}

			}

			Output[i * W + j] = Median(temp, WSize);//Median 함수를 통해 반환된 Median 값을 Output 배열의 i행j열 위치에 저장

		}
	}

}

void FillColor(BYTE* Image, int X, int Y, int W, int H, BYTE R, BYTE G, BYTE B)
{
	//RGB가아니라 BGR로 들어가있음!!
	Image[Y * W * 3 + X * 3] = B; // Blue 성분, *3을 하는 이유는 트루컬러 영상이기 때문임(한픽셀-->RGB)
	Image[Y * W * 3 + X * 3 + 1] = G; // Green 성분
	Image[Y * W * 3 + X * 3 + 2] = R; // Red 성분
}

void RGB2YCbCr(BYTE* Image, BYTE* Y, BYTE* Cb, BYTE* Cr, int W, int H)//컬러이미지, y,cb,cr성분 받음. 가로세로사이즈
{
	for (int i = 0; i < H; i++) { // Y좌표
		for (int j = 0; j < W; j++) { // X좌표
			Y[i * W + j] = (BYTE)(0.299 * Image[i * W * 3 + j * 3 + 2] + 0.587 * Image[i * W * 3 + j * 3 + 1] + 0.114 * Image[i * W * 3 + j * 3]);
			Cb[i * W + j] = (BYTE)(-0.16874 * Image[i * W * 3 + j * 3 + 2] - 0.3313 * Image[i * W * 3 + j * 3 + 1] + 0.5 * Image[i * W * 3 + j * 3] + 128.0); //128:0~255로 맞춰준다.
			Cr[i * W + j] = (BYTE)(0.5 * Image[i * W * 3 + j * 3 + 2] - 0.4187 * Image[i * W * 3 + j * 3 + 1] - 0.0813 * Image[i * W * 3 + j * 3] + 128.0);
		}
	}
}

void Erosion(BYTE* Image, BYTE* Output, int W, int H)
{
	for (int i = 1; i < H - 1; i++) { // y좌표 위아래왼쪽오른쪽: 1부터 검사, 마진을 위해 -1함
		for (int j = 1; j < W - 1; j++) {//x좌표
			if (Image[i * W + j] == 255) // 전경화소라면(전경화소가 흰색이었다)
			{
				if (!(Image[(i - 1) * W + j] == 255 &&
					Image[(i + 1) * W + j] == 255 &&
					Image[i * W + j - 1] == 255 &&
					Image[i * W + j + 1] == 255)) // 전경인지 검사, 4주변화소중에 하나라도 전경화소가 아니라면
					Output[i * W + j] = 0;
				else Output[i * W + j] = 255;
			}
			else Output[i * W + j] = 0;//전경화소가 아니라면->배경으로
		}
	}
}

void Dilation(BYTE* Image, BYTE* Output, int W, int H)
{
	for (int i = 1; i < H - 1; i++) {
		for (int j = 1; j < W - 1; j++) {
			if (Image[i * W + j] == 0) // 배경화소라면
			{
				if (!(Image[(i - 1) * W + j] == 0 &&
					Image[(i + 1) * W + j] == 0 &&
					Image[i * W + j - 1] == 0 &&
					Image[i * W + j + 1] == 0)) // 4주변화소가 모두 배경화소가 아니라면
					Output[i * W + j] = 255;
				else Output[i * W + j] = 0;
			}
			else Output[i * W + j] = 255;
		}
	}
}
//======================================================================================
//======================================================================================
int main()
{
	BITMAPFILEHEADER hf; // BMP 파일헤더 14바이트
	BITMAPINFOHEADER hInfo; // BMP 인포헤더 40바이트
	RGBQUAD hRGB[256]; // 팔레트 (256*4Bytes) 1024바이트

	FILE* fp;	//fp: 데이터를 읽거나 내보낼 때 필요
	//fp = fopen("coin.bmp", "rb");
	//fp = fopen("LENNA.bmp", "rb");
	//fp = fopen("LENNA_impulse.bmp", "rb");
	//fp = fopen("LENNA_gauss.bmp", "rb");
	//fp = fopen("pupil1.bmp", "rb");
	//fp = fopen("pupil2.bmp", "rb");
	//fp = fopen("tcsample.bmp", "rb");
	//fp = fopen("fruit.bmp", "rb");
	//fp = fopen("face.bmp", "rb");
	//fp = fopen("erosion.bmp", "rb");
	//fp = fopen("erosion2.bmp", "rb");
	fp = fopen("dilation.bmp", "rb");
	//rb: read binary, 파일에서 정보를 읽어오겠다. (txt일땐 rt이용)
	//파일 포인터는 항상 주소나 위치를 가리킨다. 맨 첫번째 가리킴.

	if (fp == NULL) //예외검사
	{
		printf("File not found!\n");
		return -1;
	}

	//fread함수로 정보를 읽어서 메모리 변수에 담음
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);	//14Bytes만큼 읽어서 hf에 담음. ->이제 다음 위치 가리킴
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);	//40bytes만큼 읽어서 hinfo에 담음
	BYTE* Image;
	BYTE* Output;
	BYTE * Temp;//임시 배열
	int ImgSize = hInfo.biWidth*hInfo.biHeight;	//영상의 가로 사이즈*세로 사이즈=영상의 전체 사이즈
	
	if (hInfo.biBitCount == 24) { // 트루컬러
		Image = (BYTE*)malloc(ImgSize * 3);
		Output = (BYTE*)malloc(ImgSize * 3);
		Temp = (BYTE*)malloc(ImgSize * 3);
		fread(Image, sizeof(BYTE), ImgSize * 3, fp);
	}
	else { // 인덱스(그레이)
		fread(hRGB, sizeof(RGBQUAD), 256, fp);
		Image = (BYTE*)malloc(ImgSize);
		Output = (BYTE*)malloc(ImgSize);
		Temp = (BYTE*)malloc(ImgSize);
		fread(Image, sizeof(BYTE), ImgSize, fp);


	}
	//영상의 화소정보 읽기
	/*BYTE*Image = (BYTE*)malloc(ImgSize);	//ImgSize만큼 동적할당, Image: 원래 영상 화소 정보
	BYTE*Output = (BYTE*)malloc(ImgSize);	//Output: 영상이 처리된 결과 담음
	//malloc->동적할당한 메모리의 첫번째 주소 반환, void의 포인터라서 강제 형변환이 필요하다.
	BYTE * Temp = (BYTE*)malloc(ImgSize);//임시 배열												
	//fread(hRGB, sizeof(RGBQUAD), 256, fp);			//4*256만큼 읽어서 hRGP에 담음
	//배열의 이름 자체가 주소이므로 & 안붙힌다., RGPQUAD가 4bytes이므로 256번 불러온다.
	*/



	fread(Image, sizeof(BYTE), ImgSize, fp);//영상의 화소정보, 그 위치에서부터 1바이트씩 ImgSize만큼 현재 file pointer부터 읽어들임
	fclose(fp);	//파일 포인터와 파일간의 관계를 끊는다.
	int W = hInfo.biWidth;
	int H = hInfo.biHeight;
//==================================================================================
	
	//--------------------------------------------------
	//[실습]w2+[과제 2] 3개 이미지파일 생성하는 프로그램
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
	//[실습]w2+[과제 2] 3개 이미지파일 생성하는 프로그램
	//--------------------------------------------------

	//--------------------------------------------------
	//[실습]w3
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
	//[실습]w3
	//--------------------------------------------------
	
	//--------------------------------------------------
	//[과제 3] Gonzalez 자동 이진화 임계치 결정 프로그램 구현
	/*
	//ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	//int Thres = GozalezBinThresh(Image, Histo, hInfo.biWidth, hInfo.biHeight, 3);//엡실론:3
	//Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);*/
	//[과제 3] Gonzalez 자동 이진화 임계치 결정 프로그램 구현
	//--------------------------------------------------

	//--------------------------------------------------
	//[실습]w4
	/*
	//AverageConv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//GausianAvrConv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//Prewitt_X_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//Prewitt_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	
	//---X,Y둘다 하고싶을 때
	Prewitt_X_Conv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
	Prewitt_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	for (int i = 0; i < ImgSize; i++)
	{
		if (Temp[i] > Output[i])
			Output[i] = Temp[i];//Temp의 경계(X돌린 결과)가 더 뚜렷하면 이를 반영
	}
	
	//Laplace_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);

	//GausianAvrConv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
	//Laplace_Conv_DC(Temp, Output, hInfo.biWidth, hInfo.biHeight);//Noise 제거 후 강화
	//Binarization(Output, Output, hInfo.biWidth, hInfo.biHeight, 40);
	*/
	//[실습]w4
	//--------------------------------------------------

	//--------------------------------------------------
	//[실습]w5- Median filtering
	//Median Filtering
	/*
	BYTE temp[9];
	int W = hInfo.biWidth, H = hInfo.biHeight;
	int i, j;
	for (i = 1; i < H - 1; i++) {		//i: 현재 행
		for (j = 1; j < W - 1; j++) {	//j: 현재 열
			temp[0] = Image[(i - 1) * W + j-1];
			temp[1] = Image[(i - 1) * W + j];
			temp[2] = Image[(i - 1) * W + j+1];
			temp[3] = Image[i * W + j-1];
			temp[4] = Image[i * W + j]; //center, 현재 위치 i,j
			temp[5] = Image[i * W + j+1];
			temp[6] = Image[(i + 1) * W + j-1];
			temp[7] = Image[(i + 1) * W + j];
			temp[8] = Image[(i + 1) * W + j+1];

			//Output[i * W + j] = Median(temp, 9); //Median: 중간에 해당하는 값 반환
			//Output[i * W + j] = MaxPooling(temp, 9);//밝은 화소값들(salt noise)이 늘어날 것이다.
			Output[i * W + j] = MinPooling(temp, 9);//pepper noise가 증가할 것이다.
		}
	}
	*/
	
	//AverageConv(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//[실습] w5 끝
	//--------------------------------------------------

	//--------------------------------------------------
	//[과제 w5]Median filter 사이즈에 따른 결과 비교
	/*
	
	MedianFiltering(Image,Output,W,H,11);
	*/
	//[과제 w5]Median filter 사이즈에 따른 결과 비교 끝
	//--------------------------------------------------

	//--------------------------------------------------
	//실습 6
	/*
	Binarization(Image, Temp, W, H, 50);
	InverseImage(Temp, Temp, W, H);//진한 영역이 전경일 때
	m_BlobColoring(Temp, H, W);//이진화된 영상,세로,가로
	for (int i = 0; i < ImgSize; i++)
		Output[i] = Image[i];// 255;
	BinaryImageEdgeDetection(Temp, Output, W, H);
	*/
	//--------------------------------------------------

	//--------------------------------------------------
	//[과제 w6]동공영역 외접사각형 및 무게중심통과하는 크로스라인 그리기
	/*
	Binarization(Image, Temp, W, H, 50);
	InverseImage(Temp, Temp, W, H);//진한 영역이 전경일 때
	m_BlobColoring(Temp, H, W);// 라벨링하여 가장 영역 큰 애 반환
	int XY[2] = { 0,0 };//crossline 그릴 X,Y
	FindCenterOfGravity(Temp, W, H, XY); //X,Y찾기
	DrawCrossLine(Image, Output, W, H, XY[0], XY[1]);//crossline 그리기

	int LRXY[4] = { 0, };//직사각형 그릴 왼쪽 상단 xy, 오른쪽 하단 xy
	FindLURD(Temp, W, H, LRXY);//두 점의 x, y 좌표 찾기
	//printf("%d %d %d %d", LRXY[0],LRXY[1], LRXY[2], LRXY[3]);
	DrawRectOutline(Output, Output, W, H, LRXY[0], LRXY[1], LRXY[2], LRXY[3]);//직사각형 그리기
	
	*/									   
	//[과제 w6]동공영역 외접사각형 및 무게중심통과하는 크로스라인 그리기 끝
	//--------------------------------------------------

	//--------------------------------------------------
	//[w7]
	
	//3주 과제 해설
	/*
	BYTE Th;
	ObtainHistogram(Image, Histo, W, H);
	Th = DetermThGonzalez(Histo);
	Binarization(Image, Output, W, H, Th);
	*/
	
	//7주차 실습
	//VerticalFlip(Image, W, H);
	//HorizontalFlip(Image, W, H);
	//Translation(Image, Output, W, H, 100, 40);
	//Scaling(Image, Output, W, H, 0.7, 0.7); //SF_X=SF_Y-->Uniform scaling
	//Rotation(Image, Output, W, H, 60);//원점을 중심으로 회전
	//Rotation2(Image, Output, W, H, 60,128,128);//128,128을 중심으로 회전

	//--------------------------------------------------

	//--------------------------------------------------
	//10주차 실습
	// (50, 40)위치를 특정 색상으로 
	/*
	FillColor(Image, 100, 200, W, H, 0, 255, 255);
	for (int i = 0; i < W; i++) {
		FillColor(Image, i, 200, W, H, 170, 80, 155);
	}*/

	// (50, 100) ~ (300, 400) 박스 채우기
	/*
	for (int i = 100; i <= 400; i++) {		//y좌표
		for (int j = 50; j <= 300; j++) {	//x좌표
			FillColor(Image, j, i, W, H, 255, 0, 255);
		}
	}*/

	
	// 가로 띠 만들기
	/*
	// 초기화
	for (int i = 0; i <  H; i++) {
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3] = 0;
			Image[i * W * 3 + j * 3 + 1] = 0;
			Image[i * W * 3 + j * 3 + 2] = 0;
		}
	}
	// y좌표 기준 0~239 (Red) //출력하면 위아래가 바뀌어서 맨 아래가 Red가 된다.
	for (int i = 0; i < 240; i++) {
		for (int j = 0; j < W; j++) {
			Image[i*W*3 + j*3 + 2] = 255; //BGR순서이므로 +2한게 red영역
		}
	}
	//120~239는 R과 G가겹침(노란색)

	// y좌표 기준 120 ~ 359 (Green)
	for (int i = 120; i < 360; i++) {
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3 + 1] = 255;
		}
	}
	//240~359는 G와 B가 겹침(Cyan?)

	// y좌표 기준 240 ~ 479 (Blue)
	for (int i = 240; i < 480; i++) {
		for (int j = 0; j < W; j++) {
			Image[i * W * 3 + j * 3] = 255;
		}
	}
	*/
	//--------------------------------------------------
	//--------------------------------------------------
	//10주 과제그라데이션 만들기
	// 그라데이션 만들기 (B ~ R)
	/*
	double wt;	//weight
	for (int a = 0; a < 160; a++) {
		for (int i = 0; i < W; i++) {
			wt = i / (double)(W - 1);
			Image[a * W * 3 + i * 3] = (BYTE)(255 * (1.0 - wt));  // Blue
			Image[a * W * 3 + i * 3 + 1] = (BYTE)(255 * wt); // Green
			Image[a * W * 3 + i * 3 + 2] = (BYTE)(255 * wt); // Red
		}
	}
	for (int a = 160; a < 320; a++) {
		for (int i = 0; i < W; i++) {
			wt = i / (double)(W - 1);
			Image[a * W * 3 + i * 3] = (BYTE)(255 * wt);  // Blue
			Image[a * W * 3 + i * 3 + 1] = (BYTE)(255 * (1.0 - wt)); // Green
			Image[a * W * 3 + i * 3 + 2] = (BYTE)(255 * wt); // Red
		}
	}
	for (int a = 320; a < 480; a++) {
		for (int i = 0; i < W; i++) {
			wt = i / (double)(W - 1);
			Image[a * W * 3 + i * 3] = (BYTE)(255 * wt);  // Blue
			Image[a * W * 3 + i * 3 + 1] = (BYTE)(255 * wt); // Green
			Image[a * W * 3 + i * 3 + 2] = (BYTE)(255 * (1.0 - wt)); // Red
		}
	}
	*/
	//--------------------------------------------------
	//--------------------------------------------------
	//11주 실습 컬러영상 영역 추출
	/*
	BYTE* Y = (BYTE *)malloc(ImgSize);
	BYTE* Cb = (BYTE*)malloc(ImgSize);
	BYTE* Cr = (BYTE*)malloc(ImgSize);

	RGB2YCbCr(Image, Y, Cb, Cr, W, H);

	// 빨간색 딸기영역만 masking (Y, Cb, Cr 모델 기준)
	
	for (int i = 0; i < H; i++) { //y좌표
		for (int j = 0; j < W; j++) { //x좌표
			//Y, Cb, Cr 모델 기준
			if (Cb[i * W + j] < 140 && Cr[i * W + j] > 190) 
			//(RGB모델 기준)
			//if (Image[i*W * 3 + j * 3 + 2] > 130&& Image[i*W * 3 + j * 3 + 1]<50&&Image[i*W * 3 + j * 3 ]<100)
			{
				Output[i * W * 3 + j * 3] = Image[i * W * 3 + j * 3];			//B
				Output[i * W * 3 + j * 3 + 1] = Image[i * W * 3 + j * 3 + 1];	//G
				Output[i * W * 3 + j * 3 + 2] = Image[i * W * 3 + j * 3 + 2];	//R
			}
			else //딸기가 아니다: 0으로
				Output[i * W * 3 + j * 3] = Output[i * W * 3 + j * 3 + 1] = Output[i * W * 3 + j * 3 + 2] = 0;
			
		}
	}
	free(Y);
	free(Cb);
	free(Cr);
	*/
	//--------------------------------------------------
	//--------------------------------------------------
	//11주차 과제 얼굴영역 검출 후 박스그리기
	/*
	BYTE* Y = (BYTE *)malloc(ImgSize);
	BYTE* Cb = (BYTE*)malloc(ImgSize);
	BYTE* Cr = (BYTE*)malloc(ImgSize);

	RGB2YCbCr(Image, Y, Cb, Cr, W, H);

	for (int i = 0; i < H; i++) { //y좌표
		for (int j = 0; j < W; j++) { //x좌표
			if (Image[i*W * 3 + j * 3 + 2] > 95 && Image[i*W * 3 + j * 3 + 1] > 40 && Image[i*W * 3 + j * 3] > 20 &&
				Image[i*W * 3 + j * 3 + 2] > Image[i*W * 3 + j * 3 + 1] &&
				Image[i*W * 3 + j * 3 + 2] > Image[i*W * 3 + j * 3]&&
				abs(Image[i*W * 3 + j * 3 + 2] - Image[i*W * 3 + j * 3 + 1]) > 15&&
				Cr[i * W + j] > 145 &&
				Cb[i * W + j] > 85 && Y[i * W + j] > 80 && Cr[i * W + j] <= (1.5862*Cb[i * W + j]) + 20 &&
				Cr[i * W + j] >= (0.3448*Cb[i * W + j]) + 76.2069 &&
				Cr[i * W + j] >= (-4.5652*Cb[i * W + j]) + 234.5652 &&
				Cr[i * W + j] <= (-1.15*Cb[i * W + j]) + 301.75 &&
				Cr[i * W + j] <= (-2.2857*Cb[i * W + j]) + 432.85)
			{
				Output[i * W * 3 + j * 3] = Image[i * W * 3 + j * 3];			//B
				Output[i * W * 3 + j * 3 + 1] = Image[i * W * 3 + j * 3 + 1];	//G
				Output[i * W * 3 + j * 3 + 2] = Image[i * W * 3 + j * 3 + 2];	//R
			}
			else //얼굴이 아니다: 0으로
				Output[i * W * 3 + j * 3] = Output[i * W * 3 + j * 3 + 1] = Output[i * W * 3 + j * 3 + 2] = 0;

		}
	}
	int LRXY[4] = { 0, };//직사각형 그릴 왼쪽 상단 xy, 오른쪽 하단 xy
	FindLURDColor(Output, W, H, LRXY);//두 점의 x, y 좌표 찾기
	printf("%d %d %d %d", LRXY[0],LRXY[1], LRXY[2], LRXY[3]);
	DrawRectOutlineColor(Image, Output, W, H, LRXY[0], LRXY[1], LRXY[2], LRXY[3]);//직사각형 그리기

	free(Y);
	free(Cb);
	free(Cr);
	*/
	//--------------------------------------------------
	//--------------------------------------------------
	//12주차 실습 모폴로지 연산
	
	
	Dilation(Image, Output, W, H);
	Dilation(Output, Image, W, H);
	Dilation(Image, Output, W, H);
	Erosion(Output, Image, W, H);
	Erosion(Image, Output, W, H);
	Erosion(Output, Image, W, H); //팽창->침식: closing, 침식->팽창: opening
	InverseImage(Image, Image, W, H);
	zhangSuen(Image, Output, H, W);//thining 지문에서 분기점 끝점 구할때 씀 분기점: 나눠지는곳(H의 가운데 두점 등)
	//--------------------------------------------------
	//==================================================================================

	SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "Thining.bmp");
	free(Image);
	free(Output);
	free(Temp);
	return 0;
}
