#include <iostream>
#include <cstring>
#include <png.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
using namespace std;

int FRAME_COUNT = 0;
int WIDTH;
int HEIGHT; 
int STRIDE;

std::string pngToText(const std::string& pngFile) {
	png_image image;
	memset((void*)&image, 0, sizeof(image));
	image.version = PNG_IMAGE_VERSION;

	if (png_image_begin_read_from_file(&image, pngFile.c_str())) {
		image.format = PNG_FORMAT_RGBA;
		std::vector<uint8_t> buffer(PNG_IMAGE_SIZE(image));

		if (png_image_finish_read(&image, nullptr, buffer.data(), 0, nullptr) != 0) {
			std::string result = "";

			for (int h = 0; h < HEIGHT; h += 2) {				
				const uint8_t* pixelPtr = buffer.data() + h * STRIDE;

				for (int w = 0; w < STRIDE; w += 4) {
					if ((pixelPtr[w] + pixelPtr[w + 1] + pixelPtr[w + 2]) < 382) {
						result += "  ";
					}
					else {
						result += "**";
					}
				}
				result += '\n';
			}
			return result;
		}
	}
	return "";
}

void extractEachFrame(const string& fileName) {
	cv::VideoCapture cap(fileName);
	if (!cap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return;
	}

	cv::Mat frame;
	while (1) {
		cap >> frame;
		if (frame.empty())
			break;
		string name = "frames\\frame" + to_string(FRAME_COUNT) + ".png";
		cv::imwrite(name, frame);
		++FRAME_COUNT;
	}
	// get videos width and height
	HEIGHT = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	WIDTH = cap.get(cv::CAP_PROP_FRAME_WIDTH);
	STRIDE = 4 * WIDTH;
	cap.release();
}

void extractAudio(const string& fileName) {
	string command = "ffmpeg -i " + fileName + " -vn audio.wav";
	system(command.c_str());
	system("cls");
}

void deleteAllFiles() {
	system("del /Q frames\\*");
	system("del /Q audio.wav");
}

void goToxy(const int &x, const int &y) {
	static COORD coord;
	coord.X = x; coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	return;
}

void hideConsoleCursor() {
	HANDLE hStdOut = NULL;
	CONSOLE_CURSOR_INFO curInfo;

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleCursorInfo(hStdOut, &curInfo);
	curInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hStdOut, &curInfo);
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	string videoName = "bacut.mp4"; // for testing purpose

	extractEachFrame(videoName); 
	extractAudio(videoName);
	hideConsoleCursor();

	int count = 0;
	auto start = std::chrono::system_clock::now();
	PlaySound(TEXT("audio.wav"), NULL, SND_FILENAME | SND_ASYNC);
	while (true) {
		if (count >= FRAME_COUNT)
			break;
		auto now = std::chrono::system_clock::now();
		auto miliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
		if (miliseconds.count() % 27 == 0) {			// around 30 fps	
			goToxy(0, 0);
			string name = "frames\\frame" + to_string(count) + ".png";
			cout << pngToText(name);
			++count;
		}
	}

	deleteAllFiles();
	return 0;
}


