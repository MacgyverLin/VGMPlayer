#include "Array.h"
#include "Platform.h"
#include "VGMData.h"
#include "VGMViewer.h"
#include "VGMAudioPlayer.h"

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
	if (dirSearchHandle != INVALID_HANDLE_VALUE)
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
	if (!res)
	{
		if (GetLastError() == ERROR_NO_MORE_FILES)
			return SR_NO_FILE;
		else
			return SR_ERROR;
	}

	if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // skip if is file
		return GetNextDirectory(directory, size);

	if (strcmp(ffd.cFileName, "..") == 0 || strcmp(ffd.cFileName, ".") == 0) // skip if .. or .
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

	if (strstr(ffd.cFileName, ext) == 0)
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

void EnumDirectories(vector<string>& directories, const string& parentDir)
{
	WIN32_FIND_DATA ffd;
	string buf = parentDir + "/*";
	HANDLE hFind = FindFirstFile(buf.c_str(), &ffd);      // DIRECTORY

	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			if((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
			{
				if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0)
				{
					string directory = parentDir + string("/") + ffd.cFileName;
					
					directories.push_back(directory);
				}
			}
		}while(FindNextFile(hFind, &ffd));

		FindClose(hFind);
	}
}

void EnumerateFiles(vector<string>& files, const string& directory, const string& extension)
{
	WIN32_FIND_DATA ffd;
	string buf = directory + string("/*.") + extension;
	HANDLE hFind = FindFirstFile(buf.c_str(), &ffd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
			{
				if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0)
				{
					string file = directory + string("/") + ffd.cFileName;

					files.push_back(file);
				}
			}
		} while (FindNextFile(hFind, &ffd));

		FindClose(hFind);
	}
}

int main()
{
	if (!Platform::Initialize())
		return false;

	vector<string> directories;
	EnumDirectories(directories, "../Musics");
	for (auto& directory : directories)
	{
		vector<string> musicFiles;
		vector<string> imageFiles;

		EnumerateFiles(musicFiles, directory, "vgm");
		EnumerateFiles(musicFiles, directory, "vgz");

		EnumerateFiles(imageFiles, directory, "png");
		EnumerateFiles(imageFiles, directory, "bmp");
		EnumerateFiles(imageFiles, directory, "jpg");

		if (musicFiles.size() == 0 && imageFiles.size() == 0)
			continue;

		for (auto& musicFile : musicFiles)
		{
			// printf("%s/%s %s\n", path, fno.fname, fno.lfname);
			VGMData vgmData(musicFile.c_str(), imageFiles[0].c_str(), 2, 16, 44100);

			VGMAudioPlayer vgmAudioPlayer;
			vgmData.Attach(vgmAudioPlayer);

#define SCALE 0.5
#define WIDTH (1080 * SCALE)
#define HEIGHT (2400 * SCALE)

			VGMViewer vgmViewer("Mac's Retro Video Game Music", 40, 40, WIDTH, HEIGHT);
			vgmData.Attach(vgmViewer);

			if (!vgmData.Open())
			{
				return -1;
			}

			//ErrorHappened = false;
			//FadeTime = FadeTimeN;
			//PauseTime = PauseTimeL;
			//PrintMSHours = (VGMHead.lngTotalSamples >= 158760000);	// 44100 smpl * 60 sec * 60 min
			//ShowVGMTag();
			//NextPLCmd = 0x80;

			vgmData.Play();

			bool done = false;
			while (!done)
			{
				if (!Platform::Update())
					return -1;

				if (!vgmData.Update(vgmAudioPlayer.GetQueued() < VGM_OUTPUT_BUFFER_COUNT / 2))
				{
					break;
				}

				if (!vgmData.IsPlaying() || Platform::GetKeyDown(Platform::ScanCode::SCANCODE_F1))
					break;

				if (Platform::GetKeyDown(Platform::ScanCode::SCANCODE_F2))
					done = true;

				//for (int i = 0; i < 10; i++)
				//{
					//if (Platform::GetKeyDown(Platform::ScanCode::SCANCODE_1 + i))
						//vgmFile.SetChannelEnable(i, !vgmFile.GetChannelEnable(i));
				//}
			}

			vgmData.Close();
		}
	}

	Platform::Terminate();
#if 0

	char path[256];
	char bmpPath[256];
	int len;
	SearchResult sr;

	printf("Init\n");

	if (!Platform::Initialize())
		return false;

	strcpy(path, "../Musics");
	if (BeginSearchDirectory(path) == SR_OK)
	{
		bool done = false;
		while (!done)
		{
			strcpy(path, "../Musics/");
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
					VGMData vgmFile(path, bmpPath, 2, 16, 48000);

#define SCALE 0.5
#define WIDTH (1080 * SCALE)
#define HEIGHT (2400 * SCALE)

					VGMAudioPlayer vgmAudioPlayer;
					vgmFile.Attach(vgmAudioPlayer);
					VGMViewer vgmViewer("Mac's Retro Video Game Music",
						40,
						40,
						WIDTH,
						HEIGHT);
					vgmFile.Attach(vgmViewer);

					if (!vgmFile.Open())
					{
						return -1;
					}

					//ErrorHappened = false;
					//FadeTime = FadeTimeN;
					//PauseTime = PauseTimeL;
					//PrintMSHours = (VGMHead.lngTotalSamples >= 158760000);	// 44100 smpl * 60 sec * 60 min
					//ShowVGMTag();
					//NextPLCmd = 0x80;

					vgmFile.Play();

					while (!done)
					{
						if (!Platform::Update())
							return -1;

						if (!vgmFile.Update(vgmAudioPlayer.GetQueued() < VGM_OUTPUT_BUFFER_COUNT / 2))
						{
							break;
						}

						if (!vgmFile.IsPlaying() || Platform::GetKeyDown(Platform::ScanCode::SCANCODE_F1))
							break;

						if (Platform::GetKeyDown(Platform::ScanCode::SCANCODE_F2))
							done = true;

						//for (int i = 0; i < 10; i++)
						//{
							//if (Platform::GetKeyDown(Platform::ScanCode::SCANCODE_1 + i))
								//vgmFile.SetChannelEnable(i, !vgmFile.GetChannelEnable(i));
						//}
					}

					vgmFile.Close();
				}


				EndSearchFile();
			}
		};

		EndSearchDirectory();
	}

	Platform::Terminate();
#endif
}