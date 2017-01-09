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
#include <boost\program_options.hpp>

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
using std::deque;

namespace program_options = boost::program_options;

const string sample_01(R"(C:\Users\Administrator\Documents\Visual Studio 2015\Projects\Personal Use\Dictionary for Console\Dictionaries\Cambridge Advanced Learner's Dictionary.txt)");
const string sample_02(R"(C:\Users\Administrator\Documents\Visual Studio 2015\Projects\Personal Use\Dictionary for Console\Dictionaries\Longman Dictionary of Contemporary English.txt)");
const string sample_03(R"(C:\Users\Administrator\Documents\Visual Studio 2015\Projects\Personal Use\Dictionary for Console\Dictionaries\Oxford Advanced Learner's Dictionary.txt)");
const string sample_04(R"(Oxford Advanced Learner's Dictionary.txt)");

Definitions SimpleXmlParser(const string &raw);
Core_Dictionary::Entry_type LingoesParser(istream &raw);
Core_Dictionary::Entry_type LingoesOxfordParser(istream &raw);
string html_to_string(const string &s);
Generic_Dictionary load_dictionary(const string &dictionary_path);
deque<string> file_list_fetcher(const string & input_dir);

int main(int argc, char **argv)
{
	system("chcp 65001");

	program_options::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("import", program_options::value<string>(), "import dictionary file");

	program_options::variables_map vm;
	program_options::store(program_options::parse_command_line(argc, argv, desc), vm);
	program_options::notify(vm);

	deque<string> file_list;
	if (vm.count("help")) 
	{
		cout << desc << "\n";
		return 1;
	}

	if (vm.count("import") && !vm["import"].empty())
	{
		cout << "Import dictionary: "
			<< vm["import"].as<string>() << ".\n";
	}
	else {
		cout << "No dictionary loaded! Reading files from current directory." << endl;
		file_list = file_list_fetcher("");
	}

	Win32ConsoleColor::Initialize_Win32ConsoleColor();

	Generic_Dictionary my_dictionary;

	try
	{
		if (!vm["import"].empty())
			my_dictionary = load_dictionary(vm["import"].as<string>());
		else if (file_list.size() > 2)
		{
			int i = 0;
			for (const auto &dictionary_path : file_list)
				cout << i++ << "\t" << dictionary_path << "\n";
			cout << "Select the dictionary file in folder" << endl;
			int index;
			cin >> index;
			if (index >= 0 && index < file_list.size())
			{
				if (file_list[index].find(".txt") != string::npos)
					my_dictionary = load_dictionary(file_list[index]);
				else
				{
					std::cerr << "Invalid file type!" << endl;
					return -1;
				}			
			}
			else
			{
				std::cerr << "File not in the list" << endl;
				return -1;
			}
		}
	}
	catch (std::runtime_error loading_failure)
	{
		std::cerr << loading_failure.what() << "\n"
			<< vm["import"].as<string>();
		return -1;
	}

	cout << u8"Dictionary initialized!" << endl;
	string word2;

	while (std::getline(cin, word2))
	{
		if (!word2.empty())
		{
			try {
				QueryResult result(my_dictionary.query(word2));
				if (result.size() > 20)
				{
					cout << Win32ConsoleColor(12) << result.size() << u8" entries found." << endl;
					cout << Win32ConsoleColor(12) << u8"Too many results, please query a more detailed wildcard string or word" << endl;
					cout << endl;
				}
				else if (result.size() == 1)
				{
					cout << Win32ConsoleColor(9) << result.size() << u8" entry found." << endl;
					cout << Win32ConsoleColor(13) << result[0].entry_word() << u8'\n'
						<< (result[0].definitions().get_raw()) << endl;
				}
				else if (result.size() != 0)
				{
					cout << Win32ConsoleColor(9) << result.size() << u8" entries found." << endl;
					for (int i = 0; i != result.size(); ++i)
						cout << Win32ConsoleColor(14) << i << u8"." << result[i].entry_word() << endl;
					cout << endl;
					cout << "You may specify a word by its index. Enter a integer between 0 and 19 to show details." << endl;
					while (true)
					{
						size_t index;
						if (!(cin >> index))
						{
							cin.clear();
							cout << "Leaving current query. You can query new words.\n" << endl;
							break;
						}
						if (index >= result.size() || index < 0) 
						{
							cout << "Leaving current query. You can query new words.\n" << endl;
							break;
						}	
						cout << Win32ConsoleColor(13) << result[index].entry_word() << u8'\n'
							<< html_to_string(result[index].definitions().get_raw()) << endl;
					}
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
