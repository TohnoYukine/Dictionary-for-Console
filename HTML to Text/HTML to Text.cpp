// HTML to Text.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>
#include <msxml.h>

using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::ostringstream;
using std::cin;
using std::cout;
using std::endl;
using std::string;

const string sample_01(R"(C:\Users\Administrator\Documents\Visual Studio 2015\Projects\Personal Use\Dictionary for Console\Dictionaries\Oxford Advanced Learner's Dictionary.txt)");
const string sample_02(R"(C:\Users\Administrator\Documents\Visual Studio 2015\Projects\Personal Use\Dictionary for Console\Dictionaries\Oxford Advanced Learner's Dictionary - Customized.txt)");

string html_to_string(const string &s)
{
	string ret;
	string::size_type pos1 = 0;
	string::size_type pos2 = 0;
	while (true)
	{
		pos2 = s.find(u8'<', pos1);
		ret += s.substr(pos1, pos2 - pos1);
		if (s.find(u8"<br />", pos2) == pos2 || s.find(u8"<br/>", pos2) == pos2)
			ret += u8'\n';
		pos1 = s.find(u8'>', pos2);
		if (pos1 != string::npos)
			++pos1;
		else
			break;
	}
	return ret;
}

int main()
{
	string s;
	while (cin)
	{
		std::getline(cin, s);
		cout << html_to_string(s);
	}
	return 0;
}