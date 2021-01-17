#pragma GCC optimize(2)
#include "bmp_old.h"
#include <bits/stdc++.h>
#include <windows.h>
using namespace std;
struct Comp {
    double x, y;
    Comp(double x, double y)
        : x(x)
        , y(y) {}
};
Comp operator+(Comp a, Comp b) { return Comp(a.x + b.x, a.y + b.y); }
Comp operator*(Comp a, Comp b) {
    return Comp(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}
uc ColorMap[256][3];
uc BitMap[720][1280][3];
uint frame;
const uint N = 2016;
const double zoom = pow(1000000.0, 1.0 / (N - 1));
double ZOOM = 1.0;
inline void to_co(int a, int b, double &x, double &y) {
    x = (b - 640) / 240.0 / ZOOM + 0.39;
    y = (a - 359) / 240.0 / ZOOM + 0.36331172955;
}
inline uint is_bounded(double x, double y) {
    Comp C(x, y);
    Comp z(C);
    for (int i = 0; i < 2048; i++) {
        z = z * z + C;
        if (z.x * z.x + z.y * z.y > 4)
            return (i >> 3);
    }
    return 255;
}
void to_RGB(uint i, uc *dest) {
    double H = i * 6.0 / 256.0 / 32.0 + 4;
    while (H >= 6.0)
        H -= 6.0;
    if (i == 255)
        H = 4.6;
    double V = 1 - 1 * pow(1 - i / 256.0, 24);
    double R, G, B;
    switch (static_cast<uint>(H)) {
    case 0:
        B = 0, G = (H - 0) * V, R = V;
        break;
    case 1:
        B = 0, R = (2 - H) * V, G = V;
        break;
    case 2:
        R = 0, B = (H - 2) * V, G = V;
        break;
    case 3:
        R = 0, G = (4 - H) * V, B = V;
        break;
    case 4:
        G = 0, R = (H - 4) * V, B = V;
        break;
    case 5:
        G = 0, B = (6 - H) * V, R = V;
        break;
    }
    dest[0] = R * 255.0 + 0.5, dest[1] = G * 255.0 + 0.5,
    dest[2] = B * 255.0 + 0.5;
}
void init_color() {
    for (uint i = 0; i < 256; ++i)
        to_RGB(i, ColorMap[i]);
}
void row(int s, int t) {
    for (int i = s; i < t; i++)
        for (int j = 0; j < 1280; ++j) {
            double x, y;
            to_co(i, j, x, y);
            memcpy(BitMap[i][j], ColorMap[is_bounded(x, y)], 3);
        }
}
void process(int thread_num = 1) {
    vector<thread> thd_pool(thread_num);
    int p = 720 / thread_num;
    for (int i = 0; i < thread_num; i++)
        thd_pool[i] = thread(row, i * p, i * p + p);
    for (int i = 0; i < thread_num; i++)
        thd_pool[i].join();
}
void output_video() {
    system("ffmpeg -y -r 25 -f rawvideo -pix_fmt rgb24 -s 1280x720 -i "
           "\\\\.\\Pipe\\MandelbrotPipe -f mp4 -pix_fmt yuv420p -s 1280x720 "
           "mandelbrot.mp4 2>ffmpeg_mandelbrot.log");
}
int main() {
    init_color();
    HANDLE hPipe
        = CreateNamedPipe("\\\\.\\Pipe\\MandelbrotPipe", PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES, 0, 0, NMPWAIT_WAIT_FOREVER, 0);

    thread ffmpeg(output_video);
    if (ConnectNamedPipe(hPipe, NULL) == TRUE) {
        cout << "Connected!" << endl;
    }

    for (frame = 1; frame <= N; ++frame) {
        process(4);
        DWORD wlen = 0;
        WriteFile(hPipe, &BitMap[0][0][0], sizeof(BitMap), &wlen, 0);
        printf("Frame %d ok.\n", frame);
        ZOOM *= zoom;
    }
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    ffmpeg.join();
    MessageBox(NULL, "Finished!", "Mandelbrot", MB_OK);
    return 0;
}
