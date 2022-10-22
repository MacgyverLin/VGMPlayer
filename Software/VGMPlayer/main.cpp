#include <Array.h>
#include "Platform.h"
#include "VGMFile.h"
#include "VGMAudioPlayer.h"
#include "VGMWaveFormViewer.h"
#include "VGMSpectrumViewer.h"
#include "VGMMultiChannelWaveFormViewer.h"
#include "VGMMultiChannelNoteViewer.h"

enum SearchResult
{
	SR_OK = 0,
	SR_ERROR = 1,
	SR_NO_FILE = 2,
};

HANDLE dirSearchHandle;
SearchResult BeginSearchDirectory(char* path)
{
	char buf[256];
	strcpy(buf, path);
	strcat(buf, "/*.*");

	WIN32_FIND_DATA ffd;
	dirSearchHandle = FindFirstFile(buf, &ffd);
	if(dirSearchHandle != INVALID_HANDLE_VALUE)
	{
		return SR_OK;
	}
	else
	{
		return SR_ERROR;
	}
}

SearchResult GetNextDirectory(char* directory, int size)
{
	WIN32_FIND_DATA ffd;
	HRESULT res = FindNextFile(dirSearchHandle, &ffd);
	if(!res)
	{
		if(GetLastError() == ERROR_NO_MORE_FILES)
			return SR_NO_FILE;
		else
			return SR_ERROR;
	}
	
	if(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // skip if is file
		return GetNextDirectory(directory, size);

	if(strcmp(ffd.cFileName, "..")==0 || strcmp(ffd.cFileName, ".") == 0) // skip if .. or .
		return GetNextDirectory(directory, size);

	strncpy(directory, ffd.cFileName, size);
	return SR_OK;
}

SearchResult RewindDirectory()
{
	//f_readdir(&dirSearchHandle, 0); // rewind
	return SR_OK;
}

void EndSearchDirectory()
{
	FindClose(dirSearchHandle);
}

////////////////////////////////////////////////
HANDLE fileSearchHandle;
SearchResult BeginSearchFile(char* path)
{
	char buf[256];
	strcpy(buf, path);
	strcat(buf, "/*.*");

	WIN32_FIND_DATA ffd;
	fileSearchHandle = FindFirstFile(buf, &ffd);
	if (fileSearchHandle != INVALID_HANDLE_VALUE)
	{
		return SR_OK;
	}
	else
	{
		return SR_ERROR;
	}
}

SearchResult GetNextFile(char* directory, int size, const char* ext)
{
	WIN32_FIND_DATA ffd;
	HRESULT res = FindNextFile(fileSearchHandle, &ffd);
	if (!res)
	{
		if (GetLastError() == ERROR_NO_MORE_FILES)
			return SR_NO_FILE;
		else
			return SR_ERROR;
	}

	if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return GetNextFile(directory, size, ext);

	if(strstr(ffd.cFileName, ext)==0)
		return GetNextFile(directory, size, ext);

	if (strcmp(ffd.cFileName, "..") == 0 || strcmp(ffd.cFileName, ".") == 0)
		return GetNextFile(directory, size, ext);

	strncpy(directory, ffd.cFileName, size);
	return SR_OK;
}

void EndSearchFile()
{
	FindClose(fileSearchHandle);
}

void LoadBmp(const char* directory)
{
	char path[256];

	strncpy(path, directory, 256);
	strcat(path, "/Title.bmp");

	//LCD_DrawImage(0, 0, path);
}

int main()
{
	char path[256];
	char bmpPath[256];
	int len;
	SearchResult sr;

	if (!Platform::Initialize())
		return false;

	strcpy(path, "../Musics");
	if (BeginSearchDirectory(path) == SR_OK)
	{
		bool done = false;
		while (!done)
		{
			strcpy(path, "../Musics");
			strcat(path, "/");
			len = strlen(path);

			SearchResult sr = GetNextDirectory(&path[len], 256 - len);
			if (sr == SR_ERROR)
			{
				EndSearchDirectory();
				return -1;
			}
			if (sr == SR_NO_FILE)
			{
				RewindDirectory();
				continue;
			}

			strncpy(bmpPath, path, 256);
			if (BeginSearchFile(bmpPath) == SR_OK)
			{
				strcat(bmpPath, "/");
				len = strlen(bmpPath);
				SearchResult sr = GetNextFile(&bmpPath[len], 256 - len, ".png");
				
				if (sr != SR_OK)
				{
					EndSearchFile();
					return -1;
				}
			}

			if (BeginSearchFile(path) == SR_OK)
			{
				strcat(path, "/");
				len = strlen(path);

				while (!done)
				{
					SearchResult sr = GetNextFile(&path[len], 256 - len, ".vgz");
					if (sr == SR_ERROR)
					{
						EndSearchFile();
						return -1;
					}
					if (sr == SR_NO_FILE)
					{
						break;
					}

					// printf("%s/%s %s\n", path, fno.fname, fno.lfname);
					VGMFile vgmFile(path, bmpPath, 2, 16, 44100);
					VGMAudioPlayer vgmAudioPlayer;
					vgmFile.Attach(vgmAudioPlayer);
#define FACTOR 0.9
#define WIDTH  (640 * FACTOR)
#define HEIGHT (480 * FACTOR)
#define TITLE_BAR 0
#define GAP 5
					VGMWaveFormViewer vgmWaveFormViewer("Waveform", 0        , TITLE_BAR, WIDTH, HEIGHT, 5.0f, VGMWaveFormViewer::Skin());
					VGMSpectrumViewer vgmSpectrumViewer("Spectrum", 0 + WIDTH, TITLE_BAR, WIDTH, HEIGHT, 5.0f, VGMSpectrumViewer::Skin());
					VGMMultiChannelWaveFormViewer vgmMultiChannelWaveFormViewer("MultiChannel Waveform" , 0 + WIDTH + WIDTH, TITLE_BAR, WIDTH, HEIGHT, 1.0f, VGMMultiChannelWaveFormViewer::Skin::Skin());
					VGMMultiChannelNoteViewer vgmMultiChannelNoteViewer("MultiChannel Note", 0, HEIGHT, 1920, 700, 1.0f, VGMMultiChannelNoteViewer::Skin::Skin());
					vgmFile.Attach(vgmWaveFormViewer);
					vgmFile.Attach(vgmSpectrumViewer);
					vgmFile.Attach(vgmMultiChannelWaveFormViewer);
					vgmFile.Attach(vgmMultiChannelNoteViewer);

					if (!vgmFile.Open())
					{
						return -1;
					}

					while (!done)
					{
						if (!Platform::Update())
							return -1;

						if (vgmAudioPlayer.GetQueued() < VGM_OUTPUT_BUFFER_COUNT / 2)
						{
							// printf("vgmDataOpenALPlayer.getQueued() %d\n", vgmAudioPlayer.getQueued());
							vgmFile.RequestUpdateData();
						}

						if (!vgmFile.Update())
						{
							break;
						}

						if (Platform::getKeyDown(Platform::ScanCode::SCANCODE_F1))
							break;

						if (Platform::getKeyDown(Platform::ScanCode::SCANCODE_F2))
							done = true;

						for (int i = 0; i < 10; i++)
						{
							if (Platform::getKeyDown(Platform::ScanCode::SCANCODE_1 + i))
								vgmFile.SetChannelEnable(i, !vgmFile.GetChannelEnable(i));
						}
					}

					vgmFile.Close();
				}


				EndSearchFile();
			}
		};

		EndSearchDirectory();
	}

	Platform::Terminate();
}