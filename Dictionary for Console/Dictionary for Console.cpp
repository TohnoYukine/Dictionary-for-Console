// Dictionary for Console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include <map>
#include <vector>
#include <iterator>
#include <string>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <set>
#include <cctype>
#include <codecvt>

#include "Dictionary.h"
#include "Definitions.h"
#include "QueryResult.h"

using std::wifstream;
using std::wofstream;
using std::wistringstream;
using std::wostringstream;
using std::wcin;
using std::wcout;
using std::endl;
using std::string;
using std::wstring;

const wstring sample_02(LR"(C:\Users\Administrator\Documents\Visual Studio 2015\Projects\Personal Use\Dictionary for Console\Dictionaries\Oxford Advanced Learner's Dictionary - Customized.txt)");
const wstring sample_01(LR"(C:\Users\Administrator\Documents\Visual Studio 2015\Projects\Personal Use\Dictionary for Console\Dictionaries\Cambridge Advanced Learner's Dictionary.txt)");

int main(int argc, char *argv[])
{
	wifstream wifile(sample_01, wifstream::in);
	Dictionary my_dictionary(wifile, L'\t', L'\n');
//	string cword(argv[1]);
//	wstring word(cword.begin(), cword.end());
//	wstring temp(my_dictionary.query(word).get_raw());
//	wcout << temp << endl;
	my_dictionary.enable_wildcard();
	wcout << L"Dictionary initialized!" << endl;
	wstring word2;
	while (std::getline(wcin, word2))
		my_dictionary.query_wildcard(word2).print();

    return 0;
}

