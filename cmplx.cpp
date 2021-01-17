#include "castle.hpp"
#include <bits/stdc++.h>
#include <windows.h>
using namespace std;
Float alpha = 1.0;
Axis axis = Axis(960, 540, 500, 500);
inline Color renderer(Dword i, Dword j) {
	complex<Float> z = axis.to_point(j, i);
	Float p = pow(abs(z), alpha) * 2000, t = alpha * arg(z);
	while (t < 0)
		t += Const::tau;
	while (t > Const::tau)
		t -= Const::tau;
	ColorHSV c(t, 1, 0.8 * p / (1 + p));
	return Color(ColorExt(c));
}
void output_video() {
	system("ffmpeg -y -r 25 -f rawvideo -pix_fmt rgb24 -s 1920x1080 -i "
		   "\\\\.\\Pipe\\cmplx -f mp4 -pix_fmt yuv420p -s 1920x1080 cmplx.mp4 "
		   "2>ffmpeg_cmplx.log");
}
PictureFixed<1080, 1920> pic;
int main() {
	HANDLE hPipe = CreateNamedPipe("\\\\.\\Pipe\\cmplx", PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, 0, 0, NMPWAIT_WAIT_FOREVER, 0);
	cout << "Connecting..." << endl;
	thread ffmpeg(output_video);
	if (ConnectNamedPipe(hPipe, NULL) == TRUE) {
		cout << "Connected!" << endl;
	}
	for (int i = 1; i <= 150; ++i) {
		Render::render(pic, function<Color(Dword, Dword)>(renderer), 4u);
		LPDWORD wlen = 0;
		WriteFile(hPipe, &pic.data[0][0][0], sizeof(pic.data), wlen, 0);
		printf("Frame %d ok.\n", i);
		alpha += 0.03;
	}
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	ffmpeg.join();
	return 0;
}
