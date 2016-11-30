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

using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::ostringstream;
using std::cin;
using std::cout;
using std::endl;
using std::string;

const string sample_02(R"(C:\Users\Administrator\Documents\Visual Studio 2015\Projects\Personal Use\Dictionary for Console\Dictionaries\Oxford Advanced Learner's Dictionary.txt)");
const string sample_01(R"(C:\Users\Administrator\Documents\Visual Studio 2015\Projects\Personal Use\Dictionary for Console\Dictionaries\Cambridge Advanced Learner's Dictionary.txt)");

int main(int argc, char *argv[])
{
	cout << u8"Choose a dictionary(1/2): \n"
		<< "1.Cambridge Advanced Learner's Dictionary\n"
		<< "2.Oxford Advanced Learner's Dictionary" << endl;

	ifstream ifile;
	size_t chosen;
	while (cin >> chosen)
	{
		if (chosen == 1)
		{
			ifile.open(sample_01);
			break;
		}
		else if (chosen == 2)
		{
			ifile.open(sample_02);
			break;
		}
		else
			continue;
	}
	
	Dictionary my_dictionary(ifile, u8'\t', u8'\n');
//	string cword(argv[1]);
//	wstring word(cword.begin(), cword.end());
//	wstring temp(my_dictionary.query(word).get_raw());
//	wcout << temp << endl;
	my_dictionary.enable_wildcard();
	cout << u8"Dictionary initialized!" << endl;
	string word2;
	while (std::getline(cin, word2))
		cout << my_dictionary.query_wildcard(word2).get_raw();

    return 0;
}

