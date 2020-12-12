#include <bits/stdc++.h>
using namespace std;
string rsuf[] = {"h.", "pph.", "ppc."};
bool judge(const string &s) {
	string t(s.rbegin(), s.rend());
	for (int i = 0; i < 3; ++i) {
		if (t.size() >= rsuf[i].size()
			&& t.substr(0, rsuf[i].size()) == rsuf[i])
			return 1;
	}
	return 0;
}
int main() {
	system("dir /B >filelist.txt");
	ifstream fin("filelist.txt");
	string s;
	while (getline(fin, s) > 0) {
		if (!judge(s))
			continue;
		cout << "Formatting " << s << endl;
		system(("formatter " + s).c_str());
	}
	return 0;
}
