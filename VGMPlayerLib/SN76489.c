#include <stdio.h>
#include <math.h>
#include "SN76489.h"

typedef struct
{
	int Current_Channel;
	int Current_Register;
	int Register[8];
	unsigned int Counter[4];
	unsigned int CntStep[4];
	int Volume[4];
	unsigned int Noise_Type;
	unsigned int Noise;
}SN76489;

/* Defines */
#ifndef PI
#define PI 3.14159265358979323846
#endif

// Change MAX_OUTPUT to change PSG volume (default = 0x7FFF)

#define MAX_OUTPUT 0x4FFF

#define W_NOISE 0x12000
#define P_NOISE 0x08000

//#define NOISE_DEF 0x0f35
//#define NOISE_DEF 0x0001
#define NOISE_DEF 0x4000

#define PSG_DEBUG_LEVEL 0

#if PSG_DEBUG_LEVEL > 0

#define PSG_DEBUG_0(x)								\
fprintf(psg_debug_file, (x));
#define PSG_DEBUG_1(x, a)							\
fprintf(psg_debug_file, (x), (a));
#define PSG_DEBUG_2(x, a, b)						\
fprintf(psg_debug_file, (x), (a), (b));
#define PSG_DEBUG_3(x, a, b, c)						\
fprintf(psg_debug_file, (x), (a), (b), (c));
#define PSG_DEBUG_4(x, a, b, c, d)					\
fprintf(psg_debug_file, (x), (a), (b), (c), (d));

#else

#define PSG_DEBUG_0(x)
#define PSG_DEBUG_1(x, a)
#define PSG_DEBUG_2(x, a, b)
#define PSG_DEBUG_3(x, a, b, c)
#define PSG_DEBUG_4(x, a, b, c, d)

#endif


/* Variables */

unsigned int PSG_SIN_Table[16][512];
unsigned int PSG_Step_Table[1024];
unsigned int PSG_Volume_Table[16];
unsigned int PSG_Noise_Step_Table[4];
unsigned int PSG_Save[8];

SN76489 PSG;

#if PSG_DEBUG_LEVEL > 0
FILE *psg_debug_file = NULL;
#endif

void SN76489_WriteRegister(UINT8 chipID, UINT32 address, UINT8 data)
{
	if (data & 0x80)
	{
		PSG.Current_Register = (data & 0x70) >> 4;
		PSG.Current_Channel = PSG.Current_Register >> 1;

		data &= 0x0F;

		PSG.Register[PSG.Current_Register] = (PSG.Register[PSG.Current_Register] & 0x3F0) | data;

		if (PSG.Current_Register & 1)
		{
			// Volume
			PSG.Volume[PSG.Current_Channel] = PSG_Volume_Table[data];

			PSG_DEBUG_2("channel %d    volume = %.8X\n", PSG.Current_Channel, PSG.Volume[PSG.Current_Channel]);
		}
		else
		{
			// Frequency
			if (PSG.Current_Channel != 3)
			{
				// Normal channel

				PSG.CntStep[PSG.Current_Channel] = PSG_Step_Table[PSG.Register[PSG.Current_Register]];

				if ((PSG.Current_Channel == 2) && ((PSG.Register[6] & 3) == 3))
				{
					PSG.CntStep[3] = PSG.CntStep[2] >> 1;
				}

				PSG_DEBUG_2("channel %d    step = %.8X\n", PSG.Current_Channel, PSG.CntStep[PSG.Current_Channel]);
			}
			else
			{
				// Noise channel

				PSG.Noise = NOISE_DEF;
				PSG_Noise_Step_Table[3] = PSG.CntStep[2] >> 1;
				PSG.CntStep[3] = PSG_Noise_Step_Table[data & 3];

				if (data & 4) PSG.Noise_Type = W_NOISE;
				else PSG.Noise_Type = P_NOISE;

				PSG_DEBUG_1("channel N    type = %.2X\n", data);
			}
		}
	}
	else
	{
		if (!(PSG.Current_Register & 1))
		{
			// Frequency 

			if (PSG.Current_Channel != 3)
			{
				PSG.Register[PSG.Current_Register] = (PSG.Register[PSG.Current_Register] & 0x0F) | ((data & 0x3F) << 4);

				PSG.CntStep[PSG.Current_Channel] = PSG_Step_Table[PSG.Register[PSG.Current_Register]];

				if ((PSG.Current_Channel == 2) && ((PSG.Register[6] & 3) == 3))
				{
					PSG.CntStep[3] = PSG.CntStep[2] >> 1;
				}

				PSG_DEBUG_2("channel %d    step = %.8X\n", PSG.Current_Channel, PSG.CntStep[PSG.Current_Channel]);
			}
		}
	}
}

void SN76489_Update_Sin(UINT8 chipID, INT32 **buffers, UINT32 length)
{
	int i, j, out;
	int cur_cnt, cur_step, cur_vol;
	unsigned int *sin_t;

	for (j = 2; j >= 0; j--)
	{
		if (PSG.Volume[j])
		{
			cur_cnt = PSG.Counter[j];
			cur_step = PSG.CntStep[j];
			sin_t = PSG_SIN_Table[PSG.Register[(j << 1) + 1]];

			for (i = 0; i < length; i++)
			{
				out = sin_t[(cur_cnt = (cur_cnt + cur_step) & 0x1FFFF) >> 8];

				buffers[0][i] += out;
				buffers[1][i] += out;
			}

			PSG.Counter[j] = cur_cnt;
		}
		else
			PSG.Counter[j] += PSG.CntStep[j] * length;
	}


	// Channel 3 - Noise

	if (cur_vol = PSG.Volume[3])
	{
		cur_cnt = PSG.Counter[3];
		cur_step = PSG.CntStep[3];

		for (i = 0; i < length; i++)
		{
			cur_cnt += cur_step;

			if (PSG.Noise & 1)
			{
				buffers[0][i] += cur_vol;
				buffers[1][i] += cur_vol;

				if (cur_cnt & 0x10000)
				{
					cur_cnt &= 0xFFFF;
					PSG.Noise = (PSG.Noise ^ PSG.Noise_Type) >> 1;
				}
			}
			else if (cur_cnt & 0x10000)
			{
				cur_cnt &= 0xFFFF;
				PSG.Noise >>= 1;
			}
		}

		PSG.Counter[3] = cur_cnt;
	}
	else
		PSG.Counter[3] += PSG.CntStep[3] * length;
}

void SN76489_Update(UINT8 chipID, INT32 **buffers, UINT32 length)
{
	int i, j;
	int cur_cnt, cur_step, cur_vol;

	for (j = 2; j >= 0; j--)
	{
		if (cur_vol = PSG.Volume[j])
		{
			if ((cur_step = PSG.CntStep[j]) < 0x10000)
			{
				cur_cnt = PSG.Counter[j];

				for (i = 0; i < length; i++)
				{
					if ((cur_cnt += cur_step) & 0x10000)
					{
						buffers[0][i] += cur_vol;
						buffers[1][i] += cur_vol;
					}
				}

				PSG.Counter[j] = cur_cnt;
			}
			else
			{
				for (i = 0; i < length; i++)
				{
					buffers[0][i] += cur_vol;
					buffers[1][i] += cur_vol;
				}
			}
		}
		else
		{
			PSG.Counter[j] += PSG.CntStep[j] * length;
		}
	}

	// Channel 3 - Noise

	if (cur_vol = PSG.Volume[3])
	{
		cur_cnt = PSG.Counter[3];
		cur_step = PSG.CntStep[3];

		for (i = 0; i < length; i++)
		{
			cur_cnt += cur_step;

			if (PSG.Noise & 1)
			{
				buffers[0][i] += cur_vol;
				buffers[1][i] += cur_vol;

				if (cur_cnt & 0x10000)
				{
					cur_cnt &= 0xFFFF;
					PSG.Noise = (PSG.Noise ^ PSG.Noise_Type) >> 1;
				}
			}
			else if (cur_cnt & 0x10000)
			{
				cur_cnt &= 0xFFFF;
				PSG.Noise >>= 1;
			}
		}

		PSG.Counter[3] = cur_cnt;
	}
	else
		PSG.Counter[3] += PSG.CntStep[3] * length;
}

INT32 SN76489_Initialize(UINT8 chipCount, UINT32 clock, UINT32 rate)
{
	int i, j;
	double out;

	for (i = 1; i < 1024; i++)
	{
		// Step calculation

		out = (double)(clock) / (double)(i << 4);		// out = frequency
		out /= (double)(rate);
		out *= 65536.0;

		PSG_Step_Table[i] = (unsigned int)out;
	}

	PSG_Step_Table[0] = PSG_Step_Table[1];

	for (i = 0; i < 3; i++)
	{
		out = (double)(clock) / (double)(1 << (9 + i));
		out /= (double)(rate);
		out *= 65536.0;

		PSG_Noise_Step_Table[i] = (unsigned int)out;
	}

	PSG_Noise_Step_Table[3] = 0;

	out = (double)MAX_OUTPUT / 3.0;

	for (i = 0; i < 15; i++)
	{
		PSG_Volume_Table[i] = (unsigned int)out;
		out /= 1.258925412;		// = 10 ^ (2/20) = 2dB
	}

	PSG_Volume_Table[15] = 0;

	/*
		for(i = 0; i < 256; i++)
		{
			out = (i + 1.0) / 256.0;

			for(j = 0; j < 16; j++)
			{
				PSG_SIN_Table[j][i] = (unsigned int) (out * (double) PSG_Volume_Table[j]);
			}
		}

		for(i = 0; i < 256; i++)
		{
			out = 1.0 - ((i + 1.0) / 256.0);

			for(j = 0; j < 16; j++)
			{
				PSG_SIN_Table[j][i + 256] = (unsigned int) (out * (double) PSG_Volume_Table[j]);
			}
		}
	*/
	for (i = 0; i < 512; i++)
	{
		out = sin((2.0 * PI) * ((double)(i) / 512));
		out = sin((2.0 * PI) * ((double)(i) / 512));

		for (j = 0; j < 16; j++)
		{
			PSG_SIN_Table[j][i] = (unsigned int)(out * (double)PSG_Volume_Table[j]);
		}
	}

	PSG.Current_Register = 0;
	PSG.Current_Channel = 0;
	PSG.Noise = 0;
	PSG.Noise_Type = 0;

	for (i = 0; i < 4; i++)
	{
		PSG.Volume[i] = 0;
		PSG.Counter[i] = 0;
		PSG.CntStep[i] = 0;
	}

	for (i = 0; i < 8; i += 2)
	{
		PSG_Save[i] = 0;
		PSG_Save[i + 1] = 0x0F;			// volume = OFF
	}

	return -1;
}

void SN76489_Reset(UINT8 chipID)
{
}


void SN76489_Shutdown(void)
{
}

UINT32 SN76489_ReadStatus(UINT8 chipID)
{
	return 0;
}