/******************************************************************************
Ootake
・キューの参照処理をシンプルにした。テンポの安定性および音質の向上。
・オーバーサンプリングしないようにした。（筆者の主観もあるが、PSGの場合、響きの
  美しさが損なわれてしまうケースが多いため。速度的にもアップ）
・ノイズの音質・音量を実機並みに調整した。v0.72
・ノイズの周波数に0x1Fが書き込まれたときは、0x1Eと同じ周波数で音量を半分にして
  鳴らすようにした。v0.68
・現状は再生サンプルレートは44.1KHz固定とした。(CD-DA再生時の速度アップのため)
・DDA音の発声が終了したときにいきなり波形を0にせず、フェードアウトさせるように
  し、ノイズを軽減した。v0.57
・DDAモード(サンプリング発声)のときの波形データのノイズが多く含まれている部分
  をカットしして、音質を上げた。音量も調節した。v0.59
・ノイズ音の音質・音量を調整して、実機の雰囲気に近づけた。v0.68
・waveIndexの初期化とDDAモード時の動作を見直して実機の動作に近づけた。v0.63
・waveIndexの初期化時にwaveテーブルも初期化するようにした。ファイヤープロレス
  リング、Ｆ１トリプルバトルなどの音が実機に近づいた。v0.65
・waveの波形の正負を実機同様にした。v0.74
・waveの最小値が-14になるようにし音質を整えた。v0.74
・クリティカルセクションは必要ない(書き込みが同時に行われるわけではない)ような
  ので、省略し高速化した。v1.09
・キュー処理(ApuQueue.c)をここに統合して高速化した。v1.10
・低音領域のボリュームを上げて実機並みの聞こえやすさに近づけた。v1.46
・LFO処理のの実装。"はにいいんざすかい"のOPや、フラッシュハイダースの効果音が
  実機の音に近づいた。v1.59

Copyright(C)2006-2012 Kitao Nakamura.
	改造版・後継版を公開なさるときは必ずソースコードを添付してください。
	その際に事後でかまいませんので、ひとことお知らせいただけると幸いです。
	商的な利用は禁じます。
	あとは「GNU General Public License(一般公衆利用許諾契約書)」に準じます。

*******************************************************************************
	[PSG.c]
		ＰＳＧを実装します。

	Implements the PSG.

	Copyright (C) 2004 Ki

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	// for memset
#include <math.h>
#include "mamedef.h"
#include "Ootake_PSG.h"


#define N_CHANNEL			6

#define OVERSAMPLE_RATE		1.0
#define PSG_DECLINE			(21.8500*6.0)
#define VOL_TABLE_DECLINE	-1.05809999010
#define NOISE_TABLE_VALUE	-18 : -1
#define SAMPLE_FADE_DECLINE	 0.305998999951

typedef struct
{
	Uint32		frq;
	BOOL		bOn;
	BOOL		bDDA;
	Uint32		volume;
	Uint32		volumeL;
	Uint32		volumeR;
	Sint32		outVolumeL;
	Sint32		outVolumeR;
	Sint32		wave[32];
	Uint32		waveIndex;
	Sint32		ddaSample;
	Uint32		phase;
	Uint32		deltaPhase;
	BOOL		bNoiseOn;
	Uint32		noiseFrq;
	Uint32		deltaNoisePhase;
} PSG;

typedef struct
{
	double SAMPLE_RATE;
	double PSG_FRQ;
	double RESMPL_RATE;

	PSG			Psg[8];				// 6, 7 is unused
	Sint32		DdaFadeOutL[8];
	Sint32		DdaFadeOutR[8];
	Uint32		Channel;				// 0 - 5;
	Uint32		MainVolumeL;			// 0 - 15
	Uint32		MainVolumeR;			// 0 - 15
	Uint32		LfoFrq;
	BOOL		bLfoOn;
	Uint32		LfoCtrl;
	Uint32		LfoShift;
	Sint32		PsgVolumeEffect;
	double		Volume;
	double		VOL;
	BOOL		bPsgMute[8];

	Uint8		Port[16];

	BOOL		bWaveCrash;
	BOOL		bHoneyInTheSky;
} huc6280_state;

static Sint32		_VolumeTable[92];
static Sint32		_NoiseTable[32768];

static BOOL			_bTblInit = FALSE;

static void create_volume_table()
{
	int		i;
	double	v;

	_VolumeTable[0] = 0;
	for (i = 1; i <= 91; i++)
	{
		v = 91 - i;
		_VolumeTable[i] = (Sint32)(32768.0 * pow(10.0, v * VOL_TABLE_DECLINE / 20.0));
	}
}

static void create_noise_table()
{
	Sint32	i;
	Uint32	bit0;
	Uint32	bit1;
	Uint32	bit14;
	Uint32	reg = 0x100;

	for (i = 0; i < 32768; i++)
	{
		bit0 = reg & 1;
		bit1 = (reg & 2) >> 1;
		bit14 = (bit0 ^ bit1);
		reg >>= 1;
		reg |= (bit14 << 14);
		_NoiseTable[i] = (bit0) ? NOISE_TABLE_VALUE;
	}
}


//static inline void
INLINE void
write_reg(
	huc6280_state* info,
	Uint8		reg,
	Uint8		data)
{
	Uint32	i;
	Uint32	frq;
	PSG*	PSGChn;

	info->Port[reg & 15] = data;

	switch (reg & 15)
	{
		case 0:	// register select
			info->Channel = data & 7;
			break;

		case 1:	// main volume
			info->MainVolumeL = (data >> 4) & 0x0F;
			info->MainVolumeR = data & 0x0F;

			for (i = 0; i < N_CHANNEL; i++)
			{
				PSGChn = &info->Psg[i];
				PSGChn->outVolumeL = _VolumeTable[PSGChn->volume + (info->MainVolumeL + PSGChn->volumeL) * 2];
				PSGChn->outVolumeR = _VolumeTable[PSGChn->volume + (info->MainVolumeR + PSGChn->volumeR) * 2];
			}
			break;

		case 2:
			PSGChn = &info->Psg[info->Channel];
			PSGChn->frq &= ~0xFF;
			PSGChn->frq |= data;
			
			frq = (PSGChn->frq - 1) & 0xFFF;
			if (frq)
				PSGChn->deltaPhase = (Uint32)((double)(65536.0 * 256.0 * 8.0 * info->RESMPL_RATE) / (double)frq +0.5);
			else
				PSGChn->deltaPhase = 0;
			break;

		case 3:
			PSGChn = &info->Psg[info->Channel];
			PSGChn->frq &= ~0xF00;
			PSGChn->frq |= (data & 0x0F) << 8;
			
			frq = (PSGChn->frq - 1) & 0xFFF;
			if (frq)
				PSGChn->deltaPhase = (Uint32)((double)(65536.0 * 256.0 * 8.0 * info->RESMPL_RATE) / (double)frq +0.5);
			else
				PSGChn->deltaPhase = 0;
			break;

		case 4:
			PSGChn = &info->Psg[info->Channel];
			if (info->bHoneyInTheSky)
			{
				if ((PSGChn->bOn)&&(data == 0))
				{
					if ((info->MainVolumeL & 1) == 0)
						PSGChn->volumeL = 0;
					if ((info->MainVolumeR & 1) == 0)
						PSGChn->volumeR = 0;
				}
			}

			PSGChn->bOn = ((data & 0x80) != 0);
			if ((PSGChn->bDDA)&&((data & 0x40)==0))
			{
				info->DdaFadeOutL[info->Channel] = (Sint32)((double)(PSGChn->ddaSample * PSGChn->outVolumeL) *
															((1 + (1 >> 3) + (1 >> 4) + (1 >> 5) + (1 >> 7) + (1 >> 12) + (1 >> 14) + (1 >> 15)) * SAMPLE_FADE_DECLINE));
				info->DdaFadeOutR[info->Channel] = (Sint32)((double)(PSGChn->ddaSample * PSGChn->outVolumeR) *
															((1 + (1 >> 3) + (1 >> 4) + (1 >> 5) + (1 >> 7) + (1 >> 12) + (1 >> 14) + (1 >> 15)) * SAMPLE_FADE_DECLINE));

			}
			PSGChn->bDDA = ((data & 0x40) != 0);
			
			if ((data & 0xC0) == 0x40)
			{
				PSGChn->waveIndex = 0;
				if (info->bWaveCrash)
				{
					for (i=0; i<32; i++)
						PSGChn->wave[i] = -14; //Waveデータを最小値で初期化
					info->bWaveCrash = FALSE;
				}
			}

			PSGChn->volume = data & 0x1F;
			PSGChn->outVolumeL = _VolumeTable[PSGChn->volume + (info->MainVolumeL + PSGChn->volumeL) * 2];
			PSGChn->outVolumeR = _VolumeTable[PSGChn->volume + (info->MainVolumeR + PSGChn->volumeR) * 2];
			break;

		case 5:	// LAL, RAL
			PSGChn = &info->Psg[info->Channel];
			PSGChn->volumeL = (data >> 4) & 0xF;
			PSGChn->volumeR = data & 0xF;
			PSGChn->outVolumeL = _VolumeTable[PSGChn->volume + (info->MainVolumeL + PSGChn->volumeL) * 2];
			PSGChn->outVolumeR = _VolumeTable[PSGChn->volume + (info->MainVolumeR + PSGChn->volumeR) * 2];
			break;

		case 6:
			PSGChn = &info->Psg[info->Channel];
			data &= 0x1F;
			info->bWaveCrash = FALSE;
			if (!PSGChn->bOn)
			{
				PSGChn->wave[PSGChn->waveIndex++] = 17 - data;
				PSGChn->waveIndex &= 0x1F;
			}
			if (PSGChn->bDDA)
			{
				if (data < 6)
					data = 6;
				PSGChn->ddaSample = 11 - data;
			
				if (!PSGChn->bOn)
					info->bWaveCrash = TRUE;
			}
			break;

		case 7:	// noise on, noise frq
			if (info->Channel >= 4)
			{
				PSGChn = &info->Psg[info->Channel];
				PSGChn->bNoiseOn = ((data & 0x80) != 0);
				PSGChn->noiseFrq = 0x1F - (data & 0x1F);
				if (PSGChn->noiseFrq == 0)
					PSGChn->deltaNoisePhase = (Uint32)((double)(2048.0 * info->RESMPL_RATE) +0.5); //Kitao更新
				else
					PSGChn->deltaNoisePhase = (Uint32)((double)(2048.0 * info->RESMPL_RATE) / (double)PSGChn->noiseFrq +0.5); //Kitao更新
			}
			break;

		case 8:	// LFO frequency
			info->LfoFrq = data;
			//Kitaoテスト用
			//PRINTF("LFO Frq = %X",info->LfoFrq);
			break;

		case 9:	// LFO control  Kitao更新。シンプルに実装してみた。実機で同じ動作かは未確認。はにいいんざすかいの音が似るように実装。v1.59
			if (data & 0x80) //bit7を立てて呼ぶと恐らくリセット
			{
				info->Psg[1].phase = 0; //LfoFrqは初期化しない。はにいいんざすかい。
				//Kitaoテスト用
				//PRINTF("LFO control = %X",data);
			}
			info->LfoCtrl = data & 7; //ドロップロックほらホラで5が使われる。v1.61更新
			if (info->LfoCtrl & 4)
				info->LfoCtrl = 0; //ドロップロックほらホラ。実機で聴いた感じはLFOオフと同じ音のようなのでbit2が立っていた(負の数扱い？)ら0と同じこととする。
			//Kitaoテスト用
			//PRINTF("LFO control = %X,  Frq =%X",data,info->LfoFrq);
			break;

		default:	// invalid write
			break;
	}

	return;
}


//Kitao追加
static void
set_VOL(huc6280_state* info)
{
	//Sint32	v;

	if (info->PsgVolumeEffect == 0)
		//info->VOL = 0.0; //ミュート
		info->VOL = 1.0 / 128.0;
	else if (info->PsgVolumeEffect == 3)
		info->VOL = info->Volume / (double)(OVERSAMPLE_RATE * 4.0/3.0); // 3/4。v1.29追加
	else
		info->VOL = info->Volume / (double)(OVERSAMPLE_RATE * info->PsgVolumeEffect); //Kitao追加。_PsgVolumeEffect=ボリューム調節効果。

	/*if (!APP_GetCDGame()) //Huカードゲームのときだけ、ボリューム101～120を有効化。v2.62
	{
		v = APP_GetWindowsVolume();
		if (v > 100)
			_VOL *= ((double)(v-100) * 3.0 + 100.0) / 100.0; //101～120は通常の3.0倍の音量変化。3.0倍のVol120でソルジャーブレイド最適。ビックリマンワールドOK。3.1倍以上だと音が薄くなる＆音割れの心配もあり。
	}*/
}

void
PSG_Mix(
	void*		chip,
	Sint32**	pDst,
	Sint32		nSample)
{
	huc6280_state* info = (huc6280_state*)chip;
	PSG*		PSGChn;
	Sint32		i;
	Sint32		j;
	Sint32		sample;
	Sint32		lfo;
	Sint32		sampleAllL;
	Sint32		sampleAllR;
	Sint32		smp;
	Sint32*		bufL = pDst[0];
	Sint32*		bufR = pDst[1];

	for (j=0; j<nSample; j++)
	{
		sampleAllL = 0;
		sampleAllR = 0;
		for (i=0; i<N_CHANNEL; i++)
		{
			PSGChn = &info->Psg[i];
			
			if ((PSGChn->bOn)&&((i != 1)||(info->LfoCtrl == 0))&&(!info->bPsgMute[i]))
			{
				if (PSGChn->bDDA)
				{
					smp = PSGChn->ddaSample * PSGChn->outVolumeL;
					sampleAllL += smp + (smp >> 3) + (smp >> 4) + (smp >> 5) + (smp >> 7) + (smp >> 12) + (smp >> 14) + (smp >> 15);
					smp = PSGChn->ddaSample * PSGChn->outVolumeR;
					sampleAllR += smp + (smp >> 3) + (smp >> 4) + (smp >> 5) + (smp >> 7) + (smp >> 12) + (smp >> 14) + (smp >> 15);
				}
				else if (PSGChn->bNoiseOn)
				{
					sample = _NoiseTable[PSGChn->phase >> 17];
					
					if (PSGChn->noiseFrq == 0)
					{
						smp = sample * PSGChn->outVolumeL;
						sampleAllL += (smp >> 1) + (smp >> 12) + (smp >> 14);
						smp = sample * PSGChn->outVolumeR;
						sampleAllR += (smp >> 1) + (smp >> 12) + (smp >> 14);
					}
					else
					{
						smp = sample * PSGChn->outVolumeL;
						sampleAllL += smp + (smp >> 11) + (smp >> 14) + (smp >> 15);
						smp = sample * PSGChn->outVolumeR;
						sampleAllR += smp + (smp >> 11) + (smp >> 14) + (smp >> 15);
					}
					
					PSGChn->phase += PSGChn->deltaNoisePhase;
				}
				else if (PSGChn->deltaPhase)
				{
					sample = PSGChn->wave[PSGChn->phase >> 27];
					if (PSGChn->frq < 128)
						sample -= sample >> 2;

					sampleAllL += sample * PSGChn->outVolumeL;
					sampleAllR += sample * PSGChn->outVolumeR;
					
					if ((i==0)&&(info->LfoCtrl>0))
					{
						lfo = info->Psg[1].wave[info->Psg[1].phase >> 27] << ((info->LfoCtrl-1) << 1);
						info->Psg[0].phase += (Uint32)((double)(65536.0 * 256.0 * 8.0 * info->RESMPL_RATE) / (double)(info->Psg[0].frq + lfo) +0.5);
						info->Psg[1].phase += (Uint32)((double)(65536.0 * 256.0 * 8.0 *info-> RESMPL_RATE) / (double)(info->Psg[1].frq * info->LfoFrq) +0.5);
					}
					else
						PSGChn->phase += PSGChn->deltaPhase;
				}
			}

			if (info->DdaFadeOutL[i] > 0)
				--info->DdaFadeOutL[i];
			else if (info->DdaFadeOutL[i] < 0)
				++info->DdaFadeOutL[i];
			if (info->DdaFadeOutR[i] > 0)
				--info->DdaFadeOutR[i];
			else if (info->DdaFadeOutR[i] < 0)
				++info->DdaFadeOutR[i];
			sampleAllL += info->DdaFadeOutL[i];
			sampleAllR += info->DdaFadeOutR[i];
		}

		sampleAllL = (Sint32)((double)sampleAllL * info->VOL);
		sampleAllR = (Sint32)((double)sampleAllR * info->VOL);
		*bufL++ = sampleAllL;
		*bufR++ = sampleAllR;
		
	}
}


//Kitao更新
static void
psg_reset(huc6280_state* info)
{
	int		i,j;

	memset(info->Psg, 0, sizeof(info->Psg));
	memset(info->DdaFadeOutL, 0, sizeof(info->DdaFadeOutL)); //Kitao追加
	memset(info->DdaFadeOutR, 0, sizeof(info->DdaFadeOutR)); //Kitao追加
	info->MainVolumeL = 0;
	info->MainVolumeR = 0;
	info->LfoFrq = 0;
	info->LfoCtrl = 0;
	info->Channel = 0; //Kitao追加。v2.65
	info->bWaveCrash = FALSE; //Kitao追加

	//Kitao更新。v0.65．waveデータを初期化。
	for (i=0; i<N_CHANNEL; i++)
		for (j=0; j<32; j++)
			info->Psg[i].wave[j] = -14; //最小値で初期化。ファイプロ，フォーメーションサッカー'90，F1トリプルバトルで必要。
	for (j=0; j<32; j++)
		info->Psg[3].wave[j] = 17; //ch3は最大値で初期化。F1トリプルバトル。v2.65

	//Kitao更新。v1.10。キュー処理をここに統合
//	_QueueWriteIndex = 0;
//	_QueueReadIndex  = 0;
}


static void PSG_SetVolume(huc6280_state* info);
/*-----------------------------------------------------------------------------
	[Init]
		ＰＳＧを初期化します。
-----------------------------------------------------------------------------*/
//Sint32
void*
PSG_Init(
	Sint32		clock,
	Sint32		sampleRate)
{
	huc6280_state* info;
	
	info = (huc6280_state*)malloc(sizeof(huc6280_state));
	if (info == NULL)
		return NULL;
	
	info->PSG_FRQ = clock & 0x7FFFFFFF;
	PSG_SetHoneyInTheSky(info, (clock >> 31) & 0x01);
//	PSG_SetHoneyInTheSky(0x01);
	
	info->PsgVolumeEffect = 0;
	info->Volume = 0;
	info->VOL = 0.0;
	
	PSG_SetVolume(info);

	psg_reset(info);

	if (! _bTblInit)
	{
		create_volume_table();
		create_noise_table();
		_bTblInit = TRUE;
	}

	//PSG_SetSampleRate(sampleRate);
	info->SAMPLE_RATE = sampleRate;
	info->RESMPL_RATE = info->PSG_FRQ / OVERSAMPLE_RATE / info->SAMPLE_RATE;

//	_bPsgInit = TRUE;

	return info;
}

void
PSG_Deinit(void* chip)
{
	huc6280_state* info = (huc6280_state*)chip;
	
	free(info);
}


Uint8
PSG_Read(
	void*	chip,
	Uint32	regNum)
{
	huc6280_state* info = (huc6280_state*)chip;
	
	if (regNum == 0)
		return (Uint8)info->Channel;

	return info->Port[regNum & 15];
}


void
PSG_Write(
	void*		chip,
	Uint32		regNum,
	Uint8		data)
{
	huc6280_state* info = (huc6280_state*)chip;
	
	write_reg(chip, regNum, data);
}

static void PSG_SetVolume(huc6280_state* info)
{
	info->Volume = 1.0 / PSG_DECLINE;
	set_VOL(info);
}

void
PSG_ResetVolumeReg(void* chip)
{
	huc6280_state* info = (huc6280_state*)chip;
	int	i;

	info->MainVolumeL = 0;
	info->MainVolumeR = 0;
	for (i = 0; i < N_CHANNEL; i++)
	{
		info->Psg[i].volume = 0;
		info->Psg[i].outVolumeL = 0;
		info->Psg[i].outVolumeR = 0;
		info->DdaFadeOutL[i] = 0;
		info->DdaFadeOutR[i] = 0;
	}
}


//Kitao追加
void
PSG_SetMutePsgChannel(
	void*	chip,
	Sint32	num,
	BOOL	bMute)
{
	huc6280_state* info = (huc6280_state*)chip;
	
	info->bPsgMute[num] = bMute;
	if (bMute)
	{
		info->DdaFadeOutL[num] = 0;
		info->DdaFadeOutR[num] = 0;
	}
}

void PSG_SetMuteMask(void* chip, Uint32 MuteMask)
{
	Uint8 CurChn;
	
	for (CurChn = 0x00; CurChn < N_CHANNEL; CurChn ++)
		PSG_SetMutePsgChannel(chip, CurChn, (MuteMask >> CurChn) & 0x01);
	
	return;
}

//Kitao追加
BOOL
PSG_GetMutePsgChannel(
	void*	chip,
	Sint32	num)
{
	huc6280_state* info = (huc6280_state*)chip;
	
	return info->bPsgMute[num];
}

//Kitao追加。v2.60
void
PSG_SetHoneyInTheSky(
	void*	chip,
	BOOL	bHoneyInTheSky)
{
	huc6280_state* info = (huc6280_state*)chip;
	
	info->bHoneyInTheSky = bHoneyInTheSky;
}


/*// save variable
#define SAVE_V(V)	if (fwrite(&V, sizeof(V), 1, p) != 1)	return FALSE
#define LOAD_V(V)	if (fread(&V, sizeof(V), 1, p) != 1)	return FALSE
// save array
#define SAVE_A(A)	if (fwrite(A, sizeof(A), 1, p) != 1)	return FALSE
#define LOAD_A(A)	if (fread(A, sizeof(A), 1, p) != 1)		return FALSE*/
/*-----------------------------------------------------------------------------
	[SaveState]
		状態をファイルに保存します。 
-----------------------------------------------------------------------------*/
/*BOOL
PSG_SaveState(
	FILE*		p)
{
	BOOL	bFlashHiders = FALSE; //Kitao更新。現在非使用。旧バージョンのステートセーブとの互換のため

	if (p == NULL)
		return FALSE;

	SAVE_A(_Psg);

	SAVE_V(_Channel);
	SAVE_V(_MainVolumeL);
	SAVE_V(_MainVolumeR);
	SAVE_V(_LfoFrq);
	SAVE_V(_bLfoOn); //v1.59から非使用に。
	SAVE_V(_LfoCtrl);
	SAVE_V(_LfoShift); //v1.59から非使用に。
	SAVE_V(_bWaveCrash); //Kitao追加。v0.65

	SAVE_V(bFlashHiders); //Kitao追加。v0.62

	//v1.10追加。キュー処理をここへ統合。
	SAVE_A(_Queue); //v1.61からサイズが２倍になった。
	SAVE_V(_QueueWriteIndex);
	SAVE_V(_QueueReadIndex);

	return TRUE;
}*/


/*-----------------------------------------------------------------------------
	[LoadState]
		状態をファイルから読み込みます。 
-----------------------------------------------------------------------------*/
/*BOOL
PSG_LoadState(
	FILE*		p)
{
	Uint32			i;
	double			clockCounter; //Kitao更新。現在非使用。旧バージョンのステートセーブとの互換のため
	BOOL			bInit; //Kitao更新。現在非使用。旧バージョンのステートセーブとの互換のため
	Sint32			totalClockAdvanced; //Kitao更新。現在非使用。旧バージョンのステートセーブとの互換のため
	BOOL			bFlashHiders; //Kitao更新。現在非使用。旧バージョンのステートセーブとの互換のため
	OldApuQueue		oldQueue[65536]; //v1.60以前のステートを読み込み用。
	

	if (p == NULL)
		return FALSE;

	LOAD_A(_Psg);

	LOAD_V(_Channel);
	LOAD_V(_MainVolumeL);
	LOAD_V(_MainVolumeR);
	LOAD_V(_LfoFrq);
	LOAD_V(_bLfoOn); //v1.59から非使用に。
	LOAD_V(_LfoCtrl);
	if (MAINBOARD_GetStateVersion() >= 3) //Kitao追加。v0.57以降のセーブファイルなら
		LOAD_V(_LfoShift); //v1.59から非使用に。
	if (MAINBOARD_GetStateVersion() >= 9) //Kitao追加。v0.65以降のセーブファイルなら
	{
		LOAD_V(_bWaveCrash);
	}
	else
		_bWaveCrash = FALSE;

	if (MAINBOARD_GetStateVersion() >= 7) //Kitao追加。v0.62以降のセーブファイルなら
		LOAD_V(bFlashHiders);

	//v1.10追加。キュー処理をここへ統合。v1.61更新
	if (MAINBOARD_GetStateVersion() >= 34) //v1.61beta以降のセーブファイルなら
	{
		LOAD_A(_Queue); //v1.61からサイズが２倍＆clock部分を削除した。
		LOAD_V(_QueueWriteIndex);
		LOAD_V(_QueueReadIndex);
	}
	else //v1.60以前のキュー(旧)バージョンのステートの場合、新バージョンの方に合うように変換。
	{
		LOAD_A(oldQueue);
		LOAD_V(_QueueWriteIndex);
		LOAD_V(_QueueReadIndex);
		if (_QueueWriteIndex >= _QueueReadIndex)
		{
			for (i=_QueueReadIndex; i<=_QueueWriteIndex; i++)
			{
				_Queue[i].reg  = oldQueue[i].reg;
				_Queue[i].data = oldQueue[i].data;
			}
		}
		else //Writeの位置がReadの位置よりも前（65536地点をまたいでデータが存在しているとき）の場合
		{
			for (i=_QueueReadIndex; i<=65535; i++)
			{
				_Queue[i].reg  = oldQueue[i].reg;
				_Queue[i].data = oldQueue[i].data;
			}
			for (i=0; i<=_QueueWriteIndex; i++)
			{
				_Queue[65536+i].reg  = oldQueue[i].reg;
				_Queue[65536+i].data = oldQueue[i].data;
			}
			_QueueWriteIndex += 65536;
		}
	}
	if (MAINBOARD_GetStateVersion() < 26) //Kitao追加。v1.11より前のセーブファイルなら
	{
		LOAD_V(clockCounter); //現在非使用。v0.95
		LOAD_V(bInit); //現在非使用。v1.10
		LOAD_V(totalClockAdvanced); //現在非使用。v0.95
	}

	return TRUE;
}

#undef SAVE_V
#undef SAVE_A
#undef LOAD_V
#undef LOAD_A*/

