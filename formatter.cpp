#include <bits/stdc++.h>
using namespace std;
int main(int argc, char **argv) {
	if (argc <= 1) {
		printf("No file chosen!");
		return 1;
	}
	char str[256];
	sprintf(str, "clang-format %s >temp.out", argv[1]);
	system(str);
	sprintf(str, "clang-format temp.out >%s", argv[1]);
	system(str);
	return 0;
}
