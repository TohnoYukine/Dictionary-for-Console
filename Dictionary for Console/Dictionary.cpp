#include "stdafx.h"
#include "Dictionary.h"
#include "QueryResult.h"

#define DEPRECATE_MARKED_CODE

std::wstring Dictionary::default_category(L"Default");
Dictionary::EntryWord Dictionary::InvalidWord;
Definitions Dictionary::InvalidDefinition;
Dictionary::Entry Dictionary::InvalidEntry(InvalidWord, InvalidDefinition);

void Dictionary::get_data(std::wistream & is, wchar_t delim1, wchar_t delim2)
{
	std::wstring line;
	size_t line_cnt = 0;
	while (!iswalnum(is.peek())) is.get();
	while (!is.eof()) 
	{
		std::getline(is, line, delim2);
		++line_cnt;
		raw_data.push_back(get_entry(line, delim1));
		while (!iswalnum(is.peek()) && !is.eof()) is.get();
	}
	std::cout << "Data loading completed." << std::endl;
}

Dictionary::Entry Dictionary::get_entry(std::wstring s, wchar_t delim)
{
	std::wistringstream istrm(s);
	EntryWord newWord;
	std::wstring temp;
	while (!iswalnum(istrm.peek())) istrm.get();
	if (!istrm.eof())
	{
		try
		{
			std::getline(istrm, newWord, delim);
			if (newWord.empty())
				throw std::runtime_error("No Word!");
		}
		catch (std::runtime_error input_fail)
		{
			std::cerr << input_fail.what() << std::endl;
			return InvalidEntry;
		}

		while (!iswalnum(istrm.peek())) istrm.get();

		try
		{
			while (iswspace(istrm.peek()))	istrm.get();
			std::getline(istrm, temp);
			if (temp.empty())
				throw std::runtime_error("No Definition!");
		}
		catch (std::runtime_error input_fail)
		{
			std::wcerr << newWord << ":\t" << input_fail.what() << std::endl;
			return InvalidEntry;
		}
	}
	Definitions newDef(temp);
	return Entry(newWord, newDef);
}

Dictionary::Dictionary() : 
	raw_data(), 
	dictionary(), 
	is_sorted(Unsorted),
	category(default_category)
{
}

Dictionary::Dictionary(std::wistream & is) :
	Dictionary(is, default_category, ' ', '\n') {}

Dictionary::Dictionary(std::wistream & is, wchar_t delim) :
	Dictionary(is, default_category, ' ', delim) {}

Dictionary::Dictionary(std::wistream & is, wchar_t delim1, wchar_t delim2) :
	Dictionary(is, default_category, delim1, delim2) {}

Dictionary::Dictionary(std::wistream & is, std::wstring _category) :
	Dictionary(is, _category, ' ', '\n') {}

Dictionary::Dictionary(std::wistream & is, std::wstring _category, wchar_t delim) :
	Dictionary(is, _category, ' ', delim) {}

Dictionary::Dictionary(std::wistream & is, std::wstring _category, wchar_t delim1, wchar_t delim2) :
	raw_data(),
	dictionary(new std::multimap<EntryWord, Definitions>()),
	is_sorted(Unsorted),
	category(_category)
{
	get_data(is, delim1, delim2);
//	get_data(is);
}

Dictionary::Dictionary(std::wifstream & ifile) :
	Dictionary(ifile, default_category, ' ', '\n') {}

Dictionary::Dictionary(std::wifstream & ifile, wchar_t delim) :
	Dictionary(ifile, default_category, ' ', delim) {}

Dictionary::Dictionary(std::wifstream & ifile, wchar_t delim1, wchar_t delim2) :
	Dictionary(ifile, default_category, delim1, delim2) {}

Dictionary::Dictionary(std::wifstream & ifile, std::wstring _category) :
	Dictionary(ifile, _category, ' ', '\n') {}

Dictionary::Dictionary(std::wifstream & ifile, std::wstring _category, wchar_t delim) :
	Dictionary(ifile, _category, ' ', delim) {}

Dictionary::Dictionary(std::wifstream & ifile, std::wstring _category, wchar_t delim1, wchar_t delim2) :
	raw_data(), 
	dictionary(new std::multimap<EntryWord, Definitions>()),
	is_sorted(Unsorted), 
	category(_category)
{
	if (!ifile.eof())
	{
		std::wstringstream file_buf;
		file_buf << ifile.rdbuf();
		get_data(file_buf, delim1, delim2);
//		get_data(file_buf);
	}
}

Dictionary::Dictionary(const Dictionary &origin) : 
	raw_data(origin.raw_data),
	dictionary(origin.dictionary),
	is_sorted(origin.is_sorted),
	category(default_category)
{
}

#ifndef ASSIGN_NOT_ALLOWED

Dictionary & Dictionary::operator=(Dictionary rhs)
{
	this->swap(rhs);
	return *this;
}

#endif // !ASSIGN_NOT_ALLOWED

Dictionary & Dictionary::swap(Dictionary &rhs)
{
	raw_data.swap(rhs.raw_data);
	dictionary.swap(rhs.dictionary);
	std::swap(is_sorted, rhs.is_sorted);
	return *this;
}

Dictionary::~Dictionary()
{
}

void Dictionary::pushback(const std::wstring &wstr, wchar_t delim)
{
	raw_data.push_back(get_entry(wstr, delim));
	is_sorted = Unsorted;
}

void Dictionary::insert(const std::wstring &wstr, wchar_t delim)
{
	if (is_sorted == Unsorted)
		throw std::runtime_error("Unsorted Dictionary!");
	Entry newEntry(get_entry(wstr, delim));
	raw_data.push_back(newEntry);
	dictionary->insert(newEntry);
}

#ifndef FUTURE_FEATURES_UNIMPLEMENTED
void Dictionary::operator+=(const Dictionary &rhs)
{
	for (const Entry &newEntry : rhs.raw_data)
		raw_data.push_back(newEntry);
	if (is_sorted && rhs.is_sorted)
		for (const Entry &newEntry : rhs.dictionary)
			dictionary.insert(newEntry);
	else
		is_sorted = Unsorted;
}
#endif // !FUTURE_FEATURES


void Dictionary::sort()
{
	dictionary->clear();
	for (const Entry &currEntry : raw_data)
		dictionary->insert(currEntry);

	dictionary->erase(InvalidWord);

	is_sorted = Sorted;
}

bool Dictionary::query_print(std::wstring word)
{
	if (is_sorted == Sorted)
	{
		size_t entries = 0;
		std::wostringstream query_result;
		for (auto iter = dictionary->lower_bound(word); iter != dictionary->upper_bound(word); ++iter)
		{
			++entries;
//			query_result << iter->first << "\t" << iter->second << std::endl;
		}
		std::wcout << "Entries found: " << entries << std::endl;
		std::wcout << query_result.str() << std::endl;
		return static_cast<bool>(entries);
	}
	else
	{
		std::wcerr << L"Querying with unsorted dictionary is currently unsupported. Please sort first." << std::endl;
		return false;
	}
}

QueryResult Dictionary::query(std::wstring word)
{
	std::shared_ptr<std::vector<std::multimap<EntryWord, Definitions>::iterator>> 
		result(new std::vector<std::multimap<EntryWord, Definitions>::iterator>);
	if (is_sorted == Unsorted)
		throw std::runtime_error("Unsorted Dictionary!");
	else
		for (auto iter = dictionary->lower_bound(word);
			iter != dictionary->upper_bound(word);
			++iter)
			result->push_back(iter);
	return QueryResult(word, dictionary, result);
}
