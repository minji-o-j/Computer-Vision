#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>	//���� �����
#include <stdlib.h>	//���� �����

#include <Windows.h>//��Ʈ�� ������� ������� �ȷ�Ʈ�� �ش��ϴ� ����ü ������ ����
void main(void)
{
	BITMAPFILEHEADER hf;	//BMP ������� 14Bytes
	BITMAPINFOHEADER hInfo; //BMP ������� 40Bytes
	RGBQUAD hRGB[256];		//�ȷ�Ʈ (256*4Bytes)

	FILE *fp;	//fp: �����͸� �аų� ������ �� �ʿ�
	fp = fopen("lenna.bmp", "rb");
	//rb: read binary, ���Ͽ��� ������ �о���ڴ�. (txt�϶� rt�̿�)
	//���� �����ʹ� �׻� �ּҳ� ��ġ�� ����Ų��. �� ù��° ����Ŵ.

	if (fp == NULL)	//lenna.bmp�� �ִ°�? ���ܰ˻�
		return;

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

	fread(Image, sizeof(BYTE), ImgSize, fp);//������ ȭ������, �� ��ġ�������� 1����Ʈ�� ImgSize��ŭ ���� file pointer���� �о����
	fclose(fp);	//���� �����Ϳ� ���ϰ��� ���踦 ���´�.

	//----------------------------------------------����ó���κ�

	for (int i = 0; i < ImgSize; i++)	//������: 255, ��ο� ��: 0
		Output[i] = 255 - Image[i];		//������Ű�� �ڵ�, ó���� ��� Output�� ����

	//-----------------------------------------------

	fp = fopen("output.bmp", "wb");		//���� ����->���� ����, wb: write binary
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);		//���: fwrite ����Ҷ��� 1byte������
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp);	//������ ȭ�� ���� ���, ���� ó���� ���

	fclose(fp);
	free(Image); //�������� ������ ���α׷��� ����Ǿ �� ���α׷��� ������� ������ �νĵȴ�.(�޸� ����, �޸� leak���� �߻�)
	free(Output);

}
