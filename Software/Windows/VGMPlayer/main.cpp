#include "Array.h"
#include "Platform.h"
#include "VGMData.h"
#include "VGMViewer.h"
#include "VGMAudioPlayer.h"

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

#define SCALE 0.4
#define WIDTH ((UINT32)(1080 * SCALE))
#define HEIGHT ((UINT32)(2400 * SCALE))

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

			bool skipDir = false;
			while (true)
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
				{
					skipDir = true;
					break;
				}
				//for (int i = 0; i < 10; i++)
				//{
					//if (Platform::GetKeyDown(Platform::ScanCode::SCANCODE_1 + i))
						//vgmFile.SetChannelEnable(i, !vgmFile.GetChannelEnable(i));
				//}
			};

			vgmData.Close();

			if (skipDir)
			{
				break;
			}

		}
	}

	Platform::Terminate();
}