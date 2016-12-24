// HTML to Text.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>
#include <msxml.h>

using std::wifstream;
using std::wofstream;
using std::wistringstream;
using std::wostringstream;
using std::wcin;
using std::wcout;
using std::endl;
using std::wstring;
using std::string;

const string sample_01(R"(C:\Users\Administrator\Documents\Visual Studio 2015\Projects\Personal Use\Dictionary for Console\Dictionaries\Oxford Advanced Learner's Dictionary.txt)");
const string sample_02(R"(C:\Users\Administrator\Documents\Visual Studio 2015\Projects\Personal Use\Dictionary for Console\Dictionaries\Oxford Advanced Learner's Dictionary - Customized.txt)");



/*
int main()
{
	wifstream wifile(sample_01, wifstream::in);
	wofstream wofile(sample_02, wofstream::trunc);
	wstring wline;
	int line_cnt = 0;
	while (!wifile.eof())
	{
		std::getline(wifile, wline);
		wistringstream itemp(wline);
		wostringstream otemp;
		wchar_t c;
		bool delim_switch = false;
		while (!itemp.eof())
		{
			if (!itemp.get(c))
				break;
			if (c != L'<')
			{
				otemp << c;
				delim_switch = true;
			}
			else
			{
				while (true)
				{
					itemp.get(c);
					if (c == L'>')
					{
						if (delim_switch)
							otemp << L'|';
						delim_switch = false;
						while (iswspace(itemp.peek()))
							itemp.get();
						break;
					}
					else
						continue;
				}

			}
		}
		wofile << otemp.str() << endl;
		wcout << line_cnt++ << endl;
	}
	wifile.close();
	wofile.close();
    return 0;
}

*/