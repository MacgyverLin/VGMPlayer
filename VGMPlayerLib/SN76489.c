#include <stdio.h>
#include <math.h>
#include "SN76489.h"

typedef struct
{
	INT32 Current_Channel;
	INT32 Current_Register;
	INT32 Register[8];
	UINT32 Counter[4];
	UINT32 CntStep[4];
	INT32 Volume[4];
	UINT32 Noise_Type;
	UINT32 Noise;
	UINT32 PSG_SIN_Table[16][512];
	UINT32 PSG_Step_Table[1024];
	UINT32 PSG_Volume_Table[16];
	UINT32 PSG_Noise_Step_Table[4];
	UINT32 PSG_Save[8];
}SN76489;

#define MAX_OUTPUT 0x4FFF
#define W_NOISE 0x12000
#define P_NOISE 0x08000
#define NOISE_DEF 0x4000

#define MAX_SN76489 2
SN76489 chips[MAX_SN76489];

INT32 SN76489_Initialize(UINT8 chipID, UINT32 clock, UINT32 sampleRate)
{
	SN76489* ic = &chips[chipID];
	INT32 i, j;
	FLOAT32 out;

	for (i = 1; i < 1024; i++)
	{
		// Step calculation

		out = (double)(clock) / (double)(i << 4);		// out = frequency
		out /= (double)(sampleRate);
		out *= 65536.0;

		ic->PSG_Step_Table[i] = (unsigned int)out;
	}

	ic->PSG_Step_Table[0] = ic->PSG_Step_Table[1];

	for (i = 0; i < 3; i++)
	{
		out = (double)(clock) / (double)(1 << (9 + i));
		out /= (double)(sampleRate);
		out *= 65536.0;

		ic->PSG_Noise_Step_Table[i] = (unsigned int)out;
	}

	ic->PSG_Noise_Step_Table[3] = 0;

	out = (double)MAX_OUTPUT / 3.0;

	for (i = 0; i < 15; i++)
	{
		ic->PSG_Volume_Table[i] = (unsigned int)out;
		out /= 1.258925412;		// = 10 ^ (2/20) = 2dB
	}

	ic->PSG_Volume_Table[15] = 0;

	for (i = 0; i < 512; i++)
	{
		out = sin((2.0 * PI) * ((double)(i) / 512));
		out = sin((2.0 * PI) * ((double)(i) / 512));

		for (j = 0; j < 16; j++)
		{
			ic->PSG_SIN_Table[j][i] = (unsigned int)(out * (double)ic->PSG_Volume_Table[j]);
		}
	}

	ic->Current_Register = 0;
	ic->Current_Channel = 0;
	ic->Noise = 0;
	ic->Noise_Type = 0;

	for (i = 0; i < 4; i++)
	{
		ic->Volume[i] = 0;
		ic->Counter[i] = 0;
		ic->CntStep[i] = 0;
	}

	for (i = 0; i < 8; i += 2)
	{
		ic->PSG_Save[i] = 0;
		ic->PSG_Save[i + 1] = 0x0F;			// volume = OFF
	}

	return -1;
}

void SN76489_Reset(UINT8 chipID)
{
}

void SN76489_Shutdown(void)
{
}

void SN76489_WriteRegister(UINT8 chipID, UINT32 address, UINT8 data)
{
	SN76489* ic = &chips[chipID];

	if (data & 0x80)
	{
		ic->Current_Register = (data & 0x70) >> 4;
		ic->Current_Channel = ic->Current_Register >> 1;

		data &= 0x0F;

		ic->Register[ic->Current_Register] = (ic->Register[ic->Current_Register] & 0x3F0) | data;

		if (ic->Current_Register & 1)
		{
			// Volume
			ic->Volume[ic->Current_Channel] = ic->PSG_Volume_Table[data];
		}
		else
		{
			// Frequency
			if (ic->Current_Channel != 3)
			{
				// Normal channel

				ic->CntStep[ic->Current_Channel] = ic->PSG_Step_Table[ic->Register[ic->Current_Register]];
				if ((ic->Current_Channel == 2) && ((ic->Register[6] & 3) == 3))
				{
					ic->CntStep[3] = ic->CntStep[2] >> 1;
				}
			}
			else
			{
				// Noise channel

				ic->Noise = NOISE_DEF;
				ic->PSG_Noise_Step_Table[3] = ic->CntStep[2] >> 1;
				ic->CntStep[3] = ic->PSG_Noise_Step_Table[data & 3];

				if (data & 4) 
					ic->Noise_Type = W_NOISE;
				else 
					ic->Noise_Type = P_NOISE;
			}
		}
	}
	else
	{
		if (!(ic->Current_Register & 1))
		{
			// Frequency 

			if (ic->Current_Channel != 3)
			{
				ic->Register[ic->Current_Register] = (ic->Register[ic->Current_Register] & 0x0F) | ((data & 0x3F) << 4);
				ic->CntStep[ic->Current_Channel] = ic->PSG_Step_Table[ic->Register[ic->Current_Register]];

				if ((ic->Current_Channel == 2) && ((ic->Register[6] & 3) == 3))
				{
					ic->CntStep[3] = ic->CntStep[2] >> 1;
				}
			}
		}
	}
}

UINT32 SN76489_ReadStatus(UINT8 chipID)
{
	return 0;
}

void SN76489_Update_Sin(UINT8 chipID, INT32 **buffers, UINT32 length)
{
	SN76489* ic = &chips[chipID];

	int i, j, out;
	int cur_cnt, cur_step, cur_vol;
	unsigned int *sin_t;

	for (j = 2; j >= 0; j--)
	{
		if (ic->Volume[j])
		{
			cur_cnt = ic->Counter[j];
			cur_step = ic->CntStep[j];
			sin_t = ic->PSG_SIN_Table[ic->Register[(j << 1) + 1]];

			for (i = 0; i < length; i++)
			{
				out = sin_t[(cur_cnt = (cur_cnt + cur_step) & 0x1FFFF) >> 8];

				buffers[0][i] += out;
				buffers[1][i] += out;
			}

			ic->Counter[j] = cur_cnt;
		}
		else
			ic->Counter[j] += ic->CntStep[j] * length;
	}
	
	// Channel 3 - Noise
	if (cur_vol = ic->Volume[3])
	{
		cur_cnt = ic->Counter[3];
		cur_step = ic->CntStep[3];

		for (i = 0; i < length; i++)
		{
			cur_cnt += cur_step;

			if (ic->Noise & 1)
			{
				buffers[0][i] += cur_vol;
				buffers[1][i] += cur_vol;

				if (cur_cnt & 0x10000)
				{
					cur_cnt &= 0xFFFF;
					ic->Noise = (ic->Noise ^ ic->Noise_Type) >> 1;
				}
			}
			else if (cur_cnt & 0x10000)
			{
				cur_cnt &= 0xFFFF;
				ic->Noise >>= 1;
			}
		}

		ic->Counter[3] = cur_cnt;
	}
	else
		ic->Counter[3] += ic->CntStep[3] * length;
}

void SN76489_Update(UINT8 chipID, INT32 **buffers, UINT32 length)
{
	int i, j;
	int cur_cnt, cur_step, cur_vol;
	SN76489* ic = &chips[chipID];

	for (j = 2; j >= 0; j--)
	{
		if (cur_vol = ic->Volume[j])
		{
			if ((cur_step = ic->CntStep[j]) < 0x10000)
			{
				cur_cnt = ic->Counter[j];

				for (i = 0; i < length; i++)
				{
					if ((cur_cnt += cur_step) & 0x10000)
					{
						buffers[0][i] += cur_vol;
						buffers[1][i] += cur_vol;
					}
				}

				ic->Counter[j] = cur_cnt;
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
			ic->Counter[j] += ic->CntStep[j] * length;
		}
	}

	// Channel 3 - Noise

	if (cur_vol = ic->Volume[3])
	{
		cur_cnt = ic->Counter[3];
		cur_step = ic->CntStep[3];

		for (i = 0; i < length; i++)
		{
			cur_cnt += cur_step;

			if (ic->Noise & 1)
			{
				buffers[0][i] += cur_vol;
				buffers[1][i] += cur_vol;

				if (cur_cnt & 0x10000)
				{
					cur_cnt &= 0xFFFF;
					ic->Noise = (ic->Noise ^ ic->Noise_Type) >> 1;
				}
			}
			else if (cur_cnt & 0x10000)
			{
				cur_cnt &= 0xFFFF;
				ic->Noise >>= 1;
			}
		}

		ic->Counter[3] = cur_cnt;
	}
	else
		ic->Counter[3] += ic->CntStep[3] * length;
}