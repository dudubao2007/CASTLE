#pragma GCC optimize(2)
#include "bmp_old.h"
#include <thread>
#include <windows.h>
using namespace std;
const uint Height = 1500;
const uint Width = 1500;
uc BitMap[Height][Width][3];
void output_video() {
	system("ffmpeg -y -r 30 -f rawvideo -pix_fmt bgr24 -s 1500x1500 -i "
		   "\\\\.\\Pipe\\MyPipe -f mp4 -s 1500x1500 trial_rgb.mp4 "
		   "2>ffmpeg_rgb.log");
}

int main() {
	HANDLE hPipe = CreateNamedPipe("\\\\.\\Pipe\\MyPipe", PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, 0, 0, NMPWAIT_WAIT_FOREVER, 0);
	cout << "Connecting..." << endl;
	thread ffmpeg(output_video);
	if (ConnectNamedPipe(hPipe, NULL) == TRUE) {
		cout << "Connected!" << endl;
	}
	for (int i = 0; i < 1500; ++i) {
		int s = i / 500;
		for (int j = 0; j < 1500; ++j) {
			BitMap[i][j][s] = 255;
		}
	}
	for (int i = 1; i <= 240; ++i) {
		LPDWORD wlen = 0;
		WriteFile(hPipe, &BitMap[0][0][0], sizeof(BitMap), wlen, 0);
		printf("Frame %d ok.\n", i);
	}
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	ffmpeg.join();
	return 0;
}
