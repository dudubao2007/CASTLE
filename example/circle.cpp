#include "castle.hpp"
using namespace std;
using namespace CASTLE;
int main() {
	TimeStamp _("Render");
	ColorExtPicture pic(1000, 1000);
	Shape::Circle circle(400.0, 400.0, 200.0);
	Render::render_shape(
		pic, circle, ColorExt(0.0, 1.0, 0.0), ColorExt(1.0, 1.0, 1.0), 10.0, 4);
	BMPFile bmp(pic);
	bmp.output("circle.bmp");
	return 0;
}
