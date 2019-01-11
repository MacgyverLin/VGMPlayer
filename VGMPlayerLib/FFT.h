#ifndef __FFT_H__
#define __FFT_H__

typedef struct complex //��������
{
	f32 real;		//ʵ��
	f32 imag;		//�鲿
}complex;

///////////////////////////////////////////
void conjugate_complex(int n, complex in[], complex out[]);
void c_plus(complex a, complex b, complex *c);//������
void c_mul(complex a, complex b, complex *c);//������
void c_sub(complex a, complex b, complex *c);	//��������
void c_div(complex a, complex b, complex *c);	//��������
void fft(int N, complex f[]);//����Ҷ�任 ���Ҳ��������f��
void ifft(int N, complex f[]); // ����Ҷ��任
void c_abs(complex f[], f32 out[], int n); //��������ȡģ
////////////////////////////////////////////

#endif