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
#include <locale>
#include <Windows.h>
#include <MsXml6.h>

#include "Core_Dictionary.h"
#include "Definitions.h"
#include "QueryResult.h"
#include "Win32ConsoleColor.h"

using std::ifstream;
using std::ofstream;
using std::istream;
using std::ostream;
using std::istringstream;
using std::ostringstream;
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

const string sample_01(R"(C:\Users\Administrator\Documents\Visual Studio 2015\Projects\Personal Use\Dictionary for Console\Dictionaries\Cambridge Advanced Learner's Dictionary.txt)");
const string sample_02(R"(C:\Users\Administrator\Documents\Visual Studio 2015\Projects\Personal Use\Dictionary for Console\Dictionaries\Longman Dictionary of Contemporary English.txt)");
const string sample_03(R"(C:\Users\Administrator\Documents\Visual Studio 2015\Projects\Personal Use\Dictionary for Console\Dictionaries\Oxford Advanced Learner's Dictionary.txt)");

Definitions SimpleXmlParser(const string &raw);
Core_Dictionary::Entry_type LingoesParser(istream &raw);
Core_Dictionary::Entry_type LingoesOxfordParser(istream &raw);

//HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

int main()
{
	system("chcp 65001");

	Win32ConsoleColor::Initialize_Win32ConsoleColor();

	Core_Dictionary my_dictionary;
	ifstream ifile(sample_03);
	while (!ifile.eof())
		my_dictionary.insert(LingoesOxfordParser(ifile));

	cout << u8"Dictionary initialized!" << endl;
	string word2;

	while (std::getline(cin, word2))
	{
		if (!word2.empty())
		{
			try {
//				SetConsoleTextAttribute(console, 13);
				QueryResult result(my_dictionary.query(word2));
				if (result.size() > 20)
				{
					cout << Win32ConsoleColor(12) << result.size() << u8" entries found." << endl;
					cout << Win32ConsoleColor(12) << u8"Too many results, please query a more detailed wildcard string or word" << endl;
				}
				else if (result.size() == 1)
				{
					cout << Win32ConsoleColor(9) << result.size() << u8" entry found." << endl;
					cout << Win32ConsoleColor(13);
					QueryResult raw(result);
					while (!raw.empty())
						cout << Win32ConsoleColor(13) << raw << endl;
				}
				else if (result.size() != 0)
				{
					cout << Win32ConsoleColor(9) << result.size() << u8" entries found." << endl;
					QueryResult raw(result);
					for (int i = 0; i != raw.size(); ++i)
						//using Entry_type = std::pair<EntryWord_type, Definition_type>
						//this somehow leaks out some data from class.
						cout << Win32ConsoleColor(14) << i << u8"." << raw[i]->first << endl;
					cout << endl;
				}
				else
				{
					cout << Win32ConsoleColor(12) << "No result found." << endl;
					cout << endl;
				}
			}
			catch (std::runtime_error query_failure)
			{
				std::cerr << query_failure.what() << endl;
			}

		}
	}
}


Definitions SimpleXmlParser(const string & raw)
{
	istringstream line(raw);
	string str;
	while (!line.eof())
	{
		if (line.peek() == u8'<')
		{
			while (line.peek() != u8'>')
				line.get();
			line.get();
			continue;
		}
		if (isspace(line.peek()))
		{
			line.get();
			continue;
		}
		string temp_str;
		std::getline(line, temp_str, u8'<');
		str += temp_str;
		str += u8' ';
		if (!line.eof())
			line.unget();
	}
	return Definitions(str);
}

Core_Dictionary::Entry_type LingoesParser(istream & raw)
{
	string word;
//	string def;
	if (!raw.eof())
		std::getline(raw, word, u8'\t');
	string temp;
	if (!raw.eof())
		std::getline(raw, temp);
	string def;
	while (temp.find(u8'<') != string::npos)
	{
		def += temp.substr(0, temp.find(u8'<'));
		if (temp.find(u8'>') != string::npos)
		{
			temp = temp.substr(temp.find(u8'>') + 1);
		}
	}
	def += temp;
	return Core_Dictionary::Entry_type(word, def);
}

Core_Dictionary::Entry_type LingoesOxfordParser(istream & raw)
{
	string word;
	if (!raw.eof())
		std::getline(raw, word, u8'\t');
	string temp;
	if (!raw.eof())
		std::getline(raw, temp);
	vector<string> lines;
	string line;
	while (!temp.empty() && temp.find(u8'<') != string::npos)
	{
		line += temp.substr(0, temp.find(u8'<'));
		temp = temp.substr(temp.find(u8'<'));
		if (temp.find(u8"<br />") == 0 || temp.find(u8"<br/>") == 0)
		{
			if (!line.empty())
			{
				line = line.substr(line.find_first_not_of(u8' '));
				if (!line.empty())
					lines.push_back(line);
			}
			line.clear();
		}
		temp = temp.substr(temp.find(u8'>') + 1);
	}
	if (!line.empty())
	{
		line = line.substr(line.find_first_not_of(u8' '));
		if (!line.empty())
			lines.push_back(line);
	}
	return Core_Dictionary::Entry_type(word, Definitions(lines));
}
