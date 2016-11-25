#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include <map>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <set>
#include <cctype>

#include "Definitions.h"

#define ASSIGN_NOT_ALLOWED	
#define FUTURE_FEATURES_UNIMPLEMENTED

class QueryResult;								//Allows user to query as well as add entries
class Wildcard;
class Dictionary
{
	friend QueryResult;

public:
//	using Definition = std::wstring;
	using EntryWord = std::wstring;
	using Entry = std::pair<EntryWord, Definitions>;
	static std::wstring default_category;
	static EntryWord InvalidWord;
	static Definitions InvalidDefinition;
	static Entry InvalidEntry;
	enum WorkingMode { ReadOnly = 0, ReadAndWrite = 1, Import = 2 };
	enum DictionaryState { Unsorted = 0, Sorted = 1 };


private:
	DictionaryState is_sorted;								//Indicate whether dictionary is built
	std::wstring category;									//User may use different dictionaries
	std::vector<Entry> raw_data;
	std::shared_ptr<std::multimap<EntryWord, Definitions>> dictionary;

private:
//	void get_data(std::wistream &is);					//Put raw text into raw_data
//	void get_data(std::wistream &is, wchar_t delim);	//delim separates entries
	void get_data(std::wistream &is, wchar_t delim1, wchar_t delim2);	//delim2 separate entries, delim1 separate entry word and definition
	inline Entry get_entry(std::wstring s, wchar_t delim);

public:
//Copy Control
	explicit Dictionary();

	//delim separates entries
	//delim2 separate entries, delim1 separate entry word and definition
	explicit Dictionary(std::wistream &is);		
	explicit Dictionary(std::wistream &is, wchar_t delim);
	explicit Dictionary(std::wistream &is, wchar_t delim1, wchar_t delim2);
	explicit Dictionary(std::wistream &is, std::wstring _category);
	explicit Dictionary(std::wistream &is, std::wstring _category, wchar_t delim);
	explicit Dictionary(std::wistream &is, std::wstring _category, wchar_t delim1, wchar_t delim2);

	explicit Dictionary(std::wifstream &ifile);
	explicit Dictionary(std::wifstream &ifile, wchar_t delim);
	explicit Dictionary(std::wifstream &ifile, wchar_t delim1, wchar_t delim2);
	explicit Dictionary(std::wifstream &ifile, std::wstring _category);
	explicit Dictionary(std::wifstream &ifile, std::wstring _category, wchar_t delim);
	explicit Dictionary(std::wifstream &ifile, std::wstring _category, wchar_t delim1, wchar_t delim2);


	explicit Dictionary(const Dictionary &);	//Implicitly call copy constructor is too expensive, users should know what they do.					

//Copy Assignment
#ifdef ASSIGN_NOT_ALLOWED						//Unintended assign cause loss of data!
	Dictionary &operator=(Dictionary) = delete;	
#endif // ASSIGN_NOT_ALLOWED
#ifndef ASSIGN_NOT_ALLOWED
	Dictionary &operator=(Dictionary);			//Copy and Swap
#endif // !ASSIGN_NOT_ALLOWED

	Dictionary &swap(Dictionary &);				//Swap
	~Dictionary();

//STL-like functions
	void pushback(const std::wstring &, wchar_t delim);	//Add lots of entries to raw_data, set is_sorted as Unsorted
	void insert(const std::wstring &, wchar_t delim);	//Directly insert to dictionary, also pushback to raw_data.

public:
//Future features
#ifndef FUTURE_FEATURES_UNIMPLEMENTED
	void operator+=(const Entry &);				//Add Entry
	void operator+=(const Dictionary &);		//Combine Library

	void operator<<();							//Stream operators, I cannot code for them now
	void operator>>();

	//This two are used to deal with complicated formats, which should be handled by user
	Unknown_Type lower_bound(const std::wstring &);
	Unknown_Type upper_bound(const std::wstring &);

#endif // !FUTURE_FEATURES

public:
	void sort();
	bool query_print(std::wstring);				//Marked as deprecated, return false if no entry found.
	QueryResult query(std::wstring word);
};

