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
#include "Generic_Dictionary.h"
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

#ifdef HIGH_PERFORMANCE

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

#endif // HIGH_PERFORMANCE

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

Generic_Dictionary load_dictionary(const string & dictionary_path)
{
	ifstream ifile(dictionary_path);
	if (!ifile.is_open())	throw std::runtime_error("File not exist!");

#ifdef HIGH_PERFORMANCE
	Generic_Dictionary my_dictionary;
	while (!ifile.eof())
		my_dictionary.insert(LingoesOxfordParser(ifile));
#endif // HIGH_PERFORMANCE

#ifdef LOW_MEMORY_REQUIREMENT
	Generic_Dictionary my_dictionary(ifile, u8'\t', u8'\n');
	std::shared_ptr<std::string> dict_path(new std::string(dictionary_path));
	my_dictionary.set_dictionary_path(dict_path);
#endif // LOW_MEMORY_REQUIREMENT

	ifile.close();

	return my_dictionary;
}

