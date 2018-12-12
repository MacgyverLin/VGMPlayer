#include <stdio.h>
#include <iostream>
#include "VGMPlayerLib.h"
#include <windows.h>

#include "SoundDevice.h"
#include <vector>
using namespace std;

/*
SoundDevice* soundDevice;
bool testOpenALSound(const char *filename)
{
	int channels = 2;
	int bitPerSamples = 16;
	int sampleRate = 22050;
	int bufferSegmentCount = 8;
	int frameRate = 30;
	int bufferSegmentSize = channels * (bitPerSamples / 8) * sampleRate / frameRate; // 1 / bufferSegmentCount second
	if (!SoundDevice_Create(&soundDevice, channels, bitPerSamples, sampleRate, bufferSegmentSize, bufferSegmentCount))
		return false;

	FILE *fp = fopen("2.raw", "rb");
	if (!fp)
		return false;

	int batchSize = bufferSegmentSize;
	vector<char> rawData;
	rawData.resize(batchSize);
	memset(&rawData[0], 0, batchSize);

	int readSize = 0;
	int WP = 0;
	do
	{
		while (SoundDevice_GetQueuedAudioCount(soundDevice) < 8)
		{
			readSize = fread(&rawData[0], 1, batchSize, fp);
			if (readSize == 0)
				break;

			if (!SoundDevice_AddAudioToQueue(soundDevice, WP, &rawData[0], readSize))
				break;

			WP = (WP + 1) & (bufferSegmentCount - 1);
		};

		if (GetDeviceState(soundDevice) != 3)
		{
			SoundDevice_PlaySound(soundDevice);

			SoundDevice_SetVolume(soundDevice, 1.0);
			SoundDevice_SetPlayRate(soundDevice, 1.0);
		}

		if (!SoundDevice_UpdataQueueBuffer(soundDevice))
			break;
	} while (readSize != 0);

	SoundDevice_StopSound(soundDevice);

	SoundDevice_Release(soundDevice);

	fclose(fp);

	return true;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

// gzCompress: do the compressing
int gzCompress(const char *src, int srcLen, char *dest, int destLen)
{
	z_stream c_stream;
	int err = 0;
	int windowBits = 15;
	int GZIP_ENCODING = 16;

	if (src && srcLen > 0)
	{
		c_stream.zalloc = (alloc_func)0;
		c_stream.zfree = (free_func)0;
		c_stream.opaque = (voidpf)0;
		if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
			windowBits | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
		c_stream.next_in = (Bytef *)src;
		c_stream.avail_in = srcLen;
		c_stream.next_out = (Bytef *)dest;
		c_stream.avail_out = destLen;
		while (c_stream.avail_in != 0 && c_stream.total_out < destLen)
		{
			if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
		}
		if (c_stream.avail_in != 0) return c_stream.avail_in;
		for (;;) {
			if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
			if (err != Z_OK) return -1;
		}
		if (deflateEnd(&c_stream) != Z_OK) return -1;
		return c_stream.total_out;
	}
	return -1;
}

// gzDecompress: do the decompressing
int gzDecompress(const char *src, int srcLen, const char *dst, int dstLen) {
	z_stream strm;
	strm.zalloc = NULL;
	strm.zfree = NULL;
	strm.opaque = NULL;

	strm.avail_in = srcLen;
	strm.avail_out = dstLen;
	strm.next_in = (Bytef *)src;
	strm.next_out = (Bytef *)dst;

	int err = -1, ret = -1;
	err = inflateInit2(&strm, MAX_WBITS + 16);
	if (err == Z_OK) {
		err = inflate(&strm, Z_FINISH);
		if (err == Z_STREAM_END) {
			ret = strm.total_out;
		}
		else {
			inflateEnd(&strm);
			return err;
		}
	}
	else {
		inflateEnd(&strm);
		return err;
	}
	inflateEnd(&strm);
	return err;
}

int testZlib()
{
	const char* src = "just for test, dd, dd, dd";
	int size_src = strlen(src);
	char* compressed = (char *)malloc(size_src * 2);
	memset(compressed, 0, size_src * 2);
	printf("to compress src: %s\n", src);
	printf("to compress src size: %d\n", size_src);

	int gzSize = gzCompress(src, size_src, compressed, size_src * 2);
	if (gzSize <= 0)
	{
		printf("compress error.\n");
		return -1;
	}
	printf("compressed: ");
	int i = 0;
	for (; i < gzSize; ++i)
	{
		printf("%02x ", compressed[i]);
	}
	printf("\ncompressed size: %d\n", gzSize);

	char* uncompressed = (char *)malloc(size_src * 2);
	memset(uncompressed, 0, size_src * 2);
	int ret = gzDecompress(compressed, gzSize, uncompressed, size_src * 2);
	printf("uncompressed: %s\n", uncompressed);
	printf("uncompressed size: %d\n", strlen(uncompressed));

	free(compressed);
	free(uncompressed);
	return 0;
}

#include <zlib.h>

gzFile VGM_GZOpen(const char* path)
{
	return gzopen(path, "r");
}

int VGM_GZClose(gzFile file)
{
	return gzclose(file);
}

int VGM_GZRead(gzFile file, void *buffer, unsigned int size)
{
	return gzread(file, buffer, size);
}

int VGM_GZSeekSet(gzFile file, int offset)
{
	return gzseek(file, offset, SEEK_SET);
}

int VGM_GZSeekCur(gzFile file, int offset)
{
	return gzseek(file, offset, SEEK_CUR);
}



int testGZ()
{
	VGMData vgmData;

	vgmData.data = (gzFile)VGM_GZOpen("Life_Force(NES)/01 Coin.vgz");
	if (!vgmData.data)
	{
		VGMData_Release(&vgmData);
		return 0;
	}

	if (VGM_GZRead((gzFile)vgmData.data, &vgmData.header, 256) != 256)
	{
		VGMData_Release(&vgmData);
		return 0;
	}

	if (vgmData.header.ID[0] != 'V' ||
		vgmData.header.ID[1] != 'g' ||
		vgmData.header.ID[2] != 'm' ||
		vgmData.header.ID[3] != ' ')
	{
		fprintf(stderr, "Data is not a .vgm format");

		VGMData_Release(&vgmData);

		return 0;
	}

	// seek to first data
	if (vgmData.header.version < 0x150)
	{
		// assert(vgmData.header.VGMDataOffset == 0);
	}
	else if (vgmData.header.version >= 0x150)
	{
		if (vgmData.header.VGMDataOffset == 0x0c)
		{
			VGM_GZSeekSet((gzFile)vgmData.data, 0x40);		// point to 0x40
		}
		else
		{
			VGM_GZSeekSet((gzFile)vgmData.data, 0x34 + vgmData.header.VGMDataOffset);		// point to 0x40
		}
	}

	return -1;
}
*/

int main(int argc, const char** argv)
{
	//testGZ();
	//testOpenALSound();

	// test ym2612, k053260
	const char* file1s[] =
	{
		"ThunderCross/01 The Thunder Fighters (Title).vgz",
		"ThunderCross/02 Coin.vgz",
		"ThunderCross/03 First Attack (1st.BGM).vgz",
		"ThunderCross/04 Skywalker (2st.BGM).vgz",
		"ThunderCross/05 Machine Graveyard (3st.BGM).vgz",
		"ThunderCross/06 Great Battleship (4st.BGM).vgz",
		"ThunderCross/07 Endless Labyrinth (5st.BGM).vgz",
		"ThunderCross/08 Fire Cavern (6st.BGM).vgz",
		"ThunderCross/09 Final Base (7st.BGM).vgz",
		"ThunderCross/10 Gloidential Mechanism (Boss BGM).vgz",
		"ThunderCross/11 Stage Clear.vgz",
		"ThunderCross/12 Ranking.vgz",
		"ThunderCross/13 Game Over.vgz",
		"ThunderCross/14 A Long Way (Ending).vgz",
		"ThunderCross/15 Start Demo - First Attack.vgz",
		"ThunderCross/16 Appear Demo - Great Battleship.vgz",
		"ThunderCross/17 Stage Clear (with Effect).vgz"
	};

	// test ym2612, k053260
	const char* file2s[] =
	{
		"ThunderCross2/01 Credit.vgz",
		"ThunderCross2/02 Rise in Arms (Opening Demo).vgz",
		"ThunderCross2/03 Air Battle = Thunder Cross II (Stage 1 BGM).vgz",
		"ThunderCross2/04 Approach the Boss (Boss Approach BGM).vgz",
		"ThunderCross2/05 He is not dead = Evil Eye (Stage 1, 5 Boss BGM).vgz",
		"ThunderCross2/06 Clear Demo (Stage Clear).vgz",
		"ThunderCross2/07 Kartus-Part 2 (Stage 2 BGM).vgz",
		"ThunderCross2/08 Theme of Ragamuffin (Stage 2 Boss BGM).vgz",
		"ThunderCross2/09 Heavy Metal Bomber (Stage 3 BGM).vgz",
		"ThunderCross2/10 Dark Force (Stage 3, 6 Boss BGM).vgz",
		"ThunderCross2/11 Dog Fight III (Stage 4 BGM).vgz",
		"ThunderCross2/12 Theme of Dinosaur Jr. (Stage 4 Boss BGM).vgz",
		"ThunderCross2/13 Battles of Battleship (Stage 5 BGM).vgz",
		"ThunderCross2/14 Star Light (Stage 6 BGM).vgz",
		"ThunderCross2/15 At The Front (Stage 7 BGM).vgz",
		"ThunderCross2/16 An Ominous Silence (Boss Approach BGM).vgz",
		"ThunderCross2/17 Theme of Sprouts Layber (Stage 7 Boss BGM).vgz",
		"ThunderCross2/18 A Shooting Star (Ending).vgz",
		"ThunderCross2/19 Game Over.vgz",
		"ThunderCross2/20 A Victory March (Ranking).vgz"
	};

	// test ym2151, SN76489
	const char* file3s[] =
	{
		"ThunderForce3/01 - The Wind Blew All Day Long (Opening Theme).vgz",
		"ThunderForce3/02 - Beyond the Peace (Stage Select).vgz",
		"ThunderForce3/03 - Back to the Fire (Stage 1 - Hydra).vgz",
		"ThunderForce3/04 - Gargoyle (Stage 1 Boss).vgz",
		"ThunderForce3/05 - Venus Fire (Stage 2 - Gorgon).vgz",
		"ThunderForce3/06 - Twin Vulcan (Stage 2 Boss).vgz",
		"ThunderForce3/07 - The Grubby Dark Blue (Stage 3 - Seiren).vgz",
		"ThunderForce3/08 - King Fish (Stage 3 Boss).vgz",
		"ThunderForce3/09 - Truth (Stage 4 - Haides).vgz",
		"ThunderForce3/10 - G Lobster (Stage 4 Boss).vgz",
		"ThunderForce3/11 - Final Take a Chance (Stage 5 - Ellis).vgz",
		"ThunderForce3/12 - Mobile Fort (Stage 5 Boss).vgz",
		"ThunderForce3/13 - His Behavior Inspired Us With Distrust (Stage 6 - Cerberus).vgz",
		"ThunderForce3/14 - Hunger Made Them Desperate (Stage 7 - Orn Base).vgz",
		"ThunderForce3/15 - Off Luck (Stage 7 Boss).vgz",
		"ThunderForce3/16 - Final Moment (Stage 8 - Orn Core).vgz",
		"ThunderForce3/17 - Be Menaced by Orn (Stage 8 Boss).vgz",
		"ThunderForce3/18 - Stage Clear.vgz",
		"ThunderForce3/19 - A War Without the End (Ending).vgz",
		"ThunderForce3/20 - Present (Staff Roll).vgz",
		"ThunderForce3/21 - Continue.vgz",
		"ThunderForce3/22 - Game Over.vgz"
	};

	const char* file4s[] =
	{
		"Life_Force(NES)/01 Coin.vgz",
		"Life_Force(NES)/02 Power of Anger.vgz",
		"Life_Force(NES)/03 Poison of Snake.vgz",
		"Life_Force(NES)/04 Thunderbolt.vgz",
		"Life_Force(NES)/05 Planet Ratis.vgz",
		"Life_Force(NES)/06 Starfield.vgz",
		"Life_Force(NES)/07 Burn the Wind.vgz",
		"Life_Force(NES)/08 Destroy Them All.vgz",
		"Life_Force(NES)/09 Aircraft Carrier.vgz",
		"Life_Force(NES)/10 Peace Again.vgz",
		"Life_Force(NES)/11 Crystal Forever.vgz"
	};

	const char* file5s[] =
	{
		"Excitebike(NES)/01 Title BGM 1.vgz",
		"Excitebike(NES)/02 Title BGM 2.vgz",
		"Excitebike(NES)/03 Title BGM 3.vgz",
		"Excitebike(NES)/04 Title BGM 4.vgz",
		"Excitebike(NES)/05 Start BGM.vgz",
		"Excitebike(NES)/06 Victory Stand BGM.vgz"
	};

	const char* file6s[] =
	{
		"Super_Mario_Bros._(NES)/01 Running About.vgz",
		"Super_Mario_Bros._(NES)/02 Underground.vgz",
		"Super_Mario_Bros._(NES)/03 Swimming Around.vgz",
		"Super_Mario_Bros._(NES)/04 Bowser's Castle.vgz",
		"Super_Mario_Bros._(NES)/05 Invincible.vgz",
		"Super_Mario_Bros._(NES)/06 Level Complete.vgz",
		"Super_Mario_Bros._(NES)/07 Bowser's Castle Complete.vgz",
		"Super_Mario_Bros._(NES)/08 Miss.vgz",
		"Super_Mario_Bros._(NES)/09 Game Over.vgz",
		"Super_Mario_Bros._(NES)/10 Into the Pipe.vgz",
		"Super_Mario_Bros._(NES)/11 Saved the Princess.vgz",
		"Super_Mario_Bros._(NES)/12 Running About (Hurry!).vgz",
		"Super_Mario_Bros._(NES)/13 Underground (Hurry!).vgz",
		"Super_Mario_Bros._(NES)/14 Swimming Around (Hurry!).vgz",
		"Super_Mario_Bros._(NES)/15 Bowser's Castle (Hurry!).vgz",
		"Super_Mario_Bros._(NES)/16 Invincible (Hurry!).vgz",
		"Super_Mario_Bros._(NES)/17 Into the Pipe (Hurry!).vgz",
		"Super_Mario_Bros._(NES)/18 Saved the Princess.vgz",
		"Super_Mario_Bros._(NES)/19 Name Entry.vgz",
		"Super_Mario_Bros._(NES)/20 Unused.vgz"
	};

	int musicIdx = 0;

#define MUSIC_SELECT(n) while (musicIdx < sizeof(file##n##s) / sizeof(file##n##s[0])) { VGMData *vgmData = VGMData_Create(file##n##s[musicIdx]);

	MUSIC_SELECT(4)
		VGMPlayer *vgmPlayer = VGMPlayer_Create(vgmData, 44100, -1);

		while (true)
		{
			if (!VGMPlayer_Update(vgmPlayer))
				break;
		}

		VGMPlayer_Release(vgmPlayer);
		VGMData_Release(vgmData);

		musicIdx++;
	}

	return -1;
}