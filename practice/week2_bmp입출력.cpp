#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>	//파일 입출력
#include <stdlib.h>	//파일 입출력

#include <Windows.h>//비트맵 파일헤더 인포헤더 팔레트에 해당하는 구조체 정보를 가짐
void main(void)
{
	BITMAPFILEHEADER hf;	//BMP 파일헤더 14Bytes
	BITMAPINFOHEADER hInfo; //BMP 인포헤더 40Bytes
	RGBQUAD hRGB[256];		//팔레트 (256*4Bytes)

	FILE *fp;	//fp: 데이터를 읽거나 내보낼 때 필요
	fp = fopen("lenna.bmp", "rb");
	//rb: read binary, 파일에서 정보를 읽어오겠다. (txt일땐 rt이용)
	//파일 포인터는 항상 주소나 위치를 가리킨다. 맨 첫번째 가리킴.

	if (fp == NULL)	//lenna.bmp가 있는가? 예외검사
		return;

	//fread함수로 정보를 읽어서 메모리 변수에 담음
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);	//14Bytes만큼 읽어서 hf에 담음. ->이제 다음 위치 가리킴
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);	//40bytes만큼 읽어서 hinfo에 담음
	fread(hRGB, sizeof(RGBQUAD), 256, fp);			//4*256만큼 읽어서 hRGP에 담음
	//배열의 이름 자체가 주소이므로 & 안붙힌다., RGPQUAD가 4bytes이므로 256번 불러온다.

	//영상의 화소정보 읽기
	int ImgSize = hInfo.biWidth*hInfo.biHeight;	//영상의 가로 사이즈*세로 사이즈=영상의 전체 사이즈
	BYTE*Image = (BYTE*)malloc(ImgSize);	//ImgSize만큼 동적할당, Image: 원래 영상 화소 정보
	BYTE*Output = (BYTE*)malloc(ImgSize);	//Output: 영상이 처리된 결과 담음
	//malloc->동적할당한 메모리의 첫번째 주소 반환, void의 포인터라서 강제 형변환이 필요하다.

	fread(Image, sizeof(BYTE), ImgSize, fp);//영상의 화소정보, 그 위치에서부터 1바이트씩 ImgSize만큼 현재 file pointer부터 읽어들임
	fclose(fp);	//파일 포인터와 파일간의 관계를 끊는다.

	//----------------------------------------------영상처리부분

	for (int i = 0; i < ImgSize; i++)	//밝은곳: 255, 어두운 곳: 0
		Output[i] = 255 - Image[i];		//반전시키는 코드, 처리한 결과 Output에 저장

	//-----------------------------------------------

	fp = fopen("output.bmp", "wb");		//없는 파일->새로 생성, wb: write binary
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);		//기록: fwrite 기록할때는 1byte단위로
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp);	//영상의 화소 정보 출력, 영상 처리한 결과

	fclose(fp);
	free(Image); //해제하지 않으면 프로그램이 종료되어도 그 프로그램이 사용중인 것으로 인식된다.(메모리 누수, 메모리 leak현상 발생)
	free(Output);

}
