#include "stdafx.h"
#include "Dictionary.h"
#include "QueryResult.h"
#include "WildcardString.h"

#define DEPRECATE_MARKED_CODE

std::wstring Dictionary::default_category(L"Default");
Dictionary::EntryWord Dictionary::InvalidWord;
Definitions Dictionary::InvalidDefinition;
Dictionary::Entry_type Dictionary::InvalidEntry(InvalidWord, InvalidDefinition);

inline void Dictionary::get_data(std::wistream & is, wchar_t delim1, wchar_t delim2)
{
	std::wstring line;
	size_t line_cnt = 0;
	while (iswspace(is.peek())) is.get();
	if (data_mode == RawDataOnly)
	{
		while (!is.eof())
		{
			std::getline(is, line, delim2);
			++line_cnt;
			temp_entries.push_back(wstring_to_entry(line, delim1));
			while (iswspace(is.peek()) && !is.eof()) is.get();
		}
		is_synchronized = Asynchronous;
	}
	if (data_mode == DictionaryOnly)
	{
		while (!is.eof())
		{
			std::getline(is, line, delim2);
			++line_cnt;
			dictionary->insert(wstring_to_entry(line, delim1));
			while (iswspace(is.peek()) && !is.eof()) is.get();
		}
		is_synchronized = Asynchronous;
	}
	else
	{
		while (!is.eof())
		{
			std::getline(is, line, delim2);
			++line_cnt;
			Entry_type new_entry(wstring_to_entry(line, delim1));
			temp_entries.push_back(new_entry);
			dictionary->insert(new_entry);
			while (iswspace(is.peek()) && !is.eof()) is.get();
		}
		is_synchronized = Synchronous;
	}
//	std::wcout << L"Data loading completed." << std::endl;
}

inline Dictionary::Entry_type Dictionary::wstring_to_entry(std::wstring s, wchar_t delim)
{
	std::wistringstream istrm(s);
	EntryWord newWord;
	std::wstring temp;
	while (iswspace(istrm.peek())) istrm.get();
	if (!istrm.eof())
	{
		try
		{
			std::getline(istrm, newWord, delim);
			newWord = newWord.substr(0, newWord.find_last_not_of(L" \t\n") + 1);
			if (newWord.empty())
				throw std::runtime_error("No Word!");
		}
		catch (std::runtime_error input_fail)
		{
			std::cerr << input_fail.what() << std::endl;
			return InvalidEntry;
		}

		while (iswspace(istrm.peek())) istrm.get();

		try
		{
			while (iswspace(istrm.peek()))	istrm.get();
			std::getline(istrm, temp);
			if (temp.empty())
				throw std::runtime_error("No Definition!");
		}
		catch (std::runtime_error input_fail)
		{
			std::wcerr << newWord << L":\t" << input_fail.what() << std::endl;
			return InvalidEntry;
		}
	}
	Definitions newDef(temp);
	return Entry_type(newWord, newDef);
}

inline size_t Dictionary::check_working_state()
{
	return is_sorted * 4 + is_complete * 2 + is_synchronized * 1;
}

Dictionary::Dictionary(DataMode _mode) :
	temp_entries(),
	dictionary(new Dictionary_type()),
	is_sorted(Sorted),
	is_complete(Complete),
	is_synchronized(Synchronous),
	data_mode(_mode),
	category(default_category),
	wildcard_wstring_mapping(new WildcardWstringSupport())
{
}

Dictionary::Dictionary(std::wistream & is, DataMode _mode) :
	Dictionary(is, default_category, L' ', L'\n', _mode) {}

Dictionary::Dictionary(std::wistream & is, wchar_t delim, DataMode _mode) :
	Dictionary(is, default_category, L' ', delim, _mode) {}

Dictionary::Dictionary(std::wistream & is, wchar_t delim1, wchar_t delim2, DataMode _mode) :
	Dictionary(is, default_category, delim1, delim2, _mode) {}

Dictionary::Dictionary(std::wistream & is, std::wstring _category, DataMode _mode) :
	Dictionary(is, _category, L' ', L'\n', _mode) {}

Dictionary::Dictionary(std::wistream & is, std::wstring _category, wchar_t delim, DataMode _mode) :
	Dictionary(is, _category, L' ', delim, _mode) {}

Dictionary::Dictionary(std::wistream & is, std::wstring _category, wchar_t delim1, wchar_t delim2, DataMode _mode) :
	temp_entries(),
	dictionary(new Dictionary_type()),
	is_complete(Complete),
	data_mode(_mode),
	category(_category),
	wildcard_wstring_mapping(new WildcardWstringSupport())
{
	is_sorted = (data_mode == RawDataOnly) ? Unsorted : Sorted;
	is_complete = (data_mode == DictionaryOnly) ? Incomplete : Complete;
	get_data(is, delim1, delim2);
}

Dictionary::Dictionary(std::wifstream & ifile, DataMode _mode) :
	Dictionary(ifile, default_category, L' ', L'\n', _mode) {}

Dictionary::Dictionary(std::wifstream & ifile, wchar_t delim, DataMode _mode) :
	Dictionary(ifile, default_category, L' ', delim, _mode) {}

Dictionary::Dictionary(std::wifstream & ifile, wchar_t delim1, wchar_t delim2, DataMode _mode) :
	Dictionary(ifile, default_category, delim1, delim2, _mode) {}

Dictionary::Dictionary(std::wifstream & ifile, std::wstring _category, DataMode _mode) :
	Dictionary(ifile, _category, L' ', L'\n', _mode) {}

Dictionary::Dictionary(std::wifstream & ifile, std::wstring _category, wchar_t delim, DataMode _mode) :
	Dictionary(ifile, _category, L' ', delim, _mode) {}

Dictionary::Dictionary(std::wifstream & ifile, std::wstring _category, wchar_t delim1, wchar_t delim2, DataMode _mode) :
	temp_entries(), 
	dictionary(new Dictionary_type()),
	is_complete(Complete),
	data_mode(_mode),
	category(_category),
	wildcard_wstring_mapping(new WildcardWstringSupport())
{
	is_sorted = (data_mode == RawDataOnly) ? Unsorted : Sorted;
	is_complete = (data_mode == DictionaryOnly) ? Incomplete : Complete;
	if (!ifile.eof())
	{
		std::wstringstream file_buf;
		file_buf << ifile.rdbuf();
		get_data(file_buf, delim1, delim2);
	}
	else
		throw std::runtime_error("Empty input file!");
}

Dictionary::Dictionary(std::initializer_list<Entry_type> entries, DataMode _mode) :
	temp_entries(entries),
	dictionary(new Dictionary_type()),
	is_complete(Complete),
	data_mode(_mode),
	category(default_category),
	wildcard_wstring_mapping(new WildcardWstringSupport())
{
	is_sorted = (data_mode == RawDataOnly) ? Unsorted : Sorted;
	is_complete = (data_mode == DictionaryOnly) ? Incomplete : Complete;

	if (data_mode == RawDataOnly)
	{
		for (auto &entry : entries)
			temp_entries.push_back(entry);
		is_synchronized = Asynchronous;
	}
	if (data_mode == DictionaryOnly)
	{
		for (auto &entry : entries)
			dictionary->insert(entry);
		is_synchronized = Asynchronous;
	}
	else
	{
		for (auto &entry : entries)
		{
			temp_entries.push_back(entry);
			dictionary->insert(entry);
		}
		is_synchronized = Synchronous;
	}
}

Dictionary::Dictionary(const Dictionary &origin) : 
	temp_entries(origin.temp_entries),
	dictionary(origin.dictionary),
	is_sorted(origin.is_sorted),
	is_complete(origin.is_complete),
	is_synchronized(origin.is_synchronized),
	data_mode(origin.data_mode),
	category(default_category),
	wildcard_wstring_mapping(origin.wildcard_wstring_mapping)
{
}

Dictionary & Dictionary::swap(Dictionary &rhs)
{
	temp_entries.swap(rhs.temp_entries);
	dictionary.swap(rhs.dictionary);
	category.swap(rhs.category);
	std::swap(is_sorted, rhs.is_sorted);
	std::swap(is_complete, rhs.is_complete);
	std::swap(is_synchronized, rhs.is_synchronized);
	std::swap(data_mode, rhs.data_mode);
	std::swap(wildcard_wstring_mapping, rhs.wildcard_wstring_mapping);
	return *this;
}

Dictionary::~Dictionary()
{
}

void Dictionary::emplace_back(const std::wstring &wstr, wchar_t delim)
{
	temp_entries.push_back(wstring_to_entry(wstr, delim));

	is_sorted = Unsorted;
	is_complete = is_complete;
	is_synchronized = Asynchronous;
}

void Dictionary::emplace_back(const EntryWord & _entryword, const Definitions & _definitions)
{
	temp_entries.emplace_back(_entryword, _definitions);

	is_sorted = Unsorted;
	is_complete = is_complete;
	is_synchronized = Asynchronous;
}

void Dictionary::push_back(const Entry_type & new_entry)
{
	temp_entries.push_back(new_entry);

	is_sorted = Unsorted;
	is_complete = is_complete;
	is_synchronized = Asynchronous;
}

void Dictionary::emplace(const std::wstring &wstr, wchar_t delim, DataMode _mode)
{
	data_mode = _mode;

	if (is_sorted == Unsorted)
		throw std::runtime_error("Unsorted Dictionary!");

	Entry_type newEntry(wstring_to_entry(wstr, delim));
	dictionary->insert(newEntry);

	if (data_mode == RawAndDictionary)
		temp_entries.push_back(newEntry);

	is_sorted = is_sorted;
	is_complete = is_complete;
	is_synchronized = is_synchronized;
}

void Dictionary::emplace(const EntryWord & _entryword, const Definitions & _definitions, DataMode _mode)
{
	data_mode = _mode;

	if (is_sorted == Unsorted)
		throw std::runtime_error("Unsorted Dictionary!");

	dictionary->emplace(_entryword, _definitions);

	if (data_mode == RawAndDictionary)
		temp_entries.emplace_back(_entryword, _definitions);

	is_sorted = is_sorted;
	is_complete = is_complete;
	is_synchronized = is_synchronized;
}

void Dictionary::insert(const Entry_type & new_entry, DataMode _mode)
{
	data_mode = _mode;

	if (is_sorted == Unsorted)
		throw std::runtime_error("Unsorted Dictionary!");

	dictionary->insert(new_entry);

	if (data_mode == RawAndDictionary)
		temp_entries.push_back(new_entry);

	is_sorted = is_sorted;
	is_complete = is_complete;
	is_synchronized = is_synchronized;
}

void Dictionary::clear_raw_data()
{
	temp_entries.clear();

	//throw std::runtime_error("Nowhere holding data: Trying to empty temp_entries!");

	is_sorted = is_sorted;
	is_complete = Incomplete;
	is_synchronized = (dictionary->empty() ? Synchronous : Asynchronous);
}

void Dictionary::shrink_to_fit()
{
	temp_entries.shrink_to_fit();
}

void Dictionary::clear_dictionary()
{
	dictionary->clear();
//	throw std::runtime_error("Trying to clear the sorted dictionary while temp_entries is incomplete!");
//	throw std::runtime_error("Nowhere holding data: Trying to empty dictionary!");

	is_sorted = (temp_entries.empty() ? Sorted : Unsorted);
	is_complete = is_complete;
	is_synchronized = (temp_entries.empty() ? Synchronous : Asynchronous);
}

void Dictionary::clear_all()
{
	temp_entries.clear();
	dictionary->clear();
	category.clear();

	is_sorted = Sorted;
	is_complete = Complete;
	is_synchronized = Synchronous;
}

std::shared_ptr<Dictionary::Dictionary_type> Dictionary::erase(QueryResult & words)
{
	std::shared_ptr<Dictionary_type> deleted_entries(new Dictionary_type());
	for (auto iter = words.begin(); iter != words.end(); ++iter)
	{
		deleted_entries->insert(**iter);
		dictionary->erase(*iter);
	}
	words.clear();
	return deleted_entries;
}

std::shared_ptr<Dictionary::Entry_type> Dictionary::erase_entry(QueryResult & words, Dictionary_type::size_type pos)
{
	if (pos >= words.size())
		return std::shared_ptr<Entry_type>();
	std::shared_ptr<Entry_type> deleted_entry(new Entry_type(*words[pos]));
	dictionary->erase(words[pos]);
	words.erase(pos);
	return deleted_entry;
}

void Dictionary::add_entry(const std::wstring &wstr, wchar_t delim)
{
	Entry_type newEntry(wstring_to_entry(wstr, delim));
	temp_entries.push_back(newEntry);
	if (is_sorted == Sorted)
		dictionary->insert(newEntry);
}

void Dictionary::add_entry(const EntryWord & _entryword, const Definitions & _definitions)
{
	Entry_type newEntry(_entryword, _definitions);
	temp_entries.push_back(newEntry);
	if (is_sorted == Sorted)
		dictionary->insert(newEntry);
}

void Dictionary::set_category(std::wstring _category)
{
	category = _category;
}

void Dictionary::enable_wildcard()
{
	wildcard_wstring_mapping.reset(new WildcardWstringSupport(*this));
}

void Dictionary::sort()
{
	if (is_complete == Complete)
		for (const Entry_type &currEntry : temp_entries)
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
		return (entries != 0);
	}
	else
	{
		std::wcerr << L"Querying with unsorted dictionary is currently unsupported. Please sort first." << std::endl;
		return false;
	}
}

QueryResult Dictionary::query(std::wstring word)
{
	std::shared_ptr<QueryResult::result_type> result(new QueryResult::result_type());
	if (is_sorted == Unsorted)
		throw std::runtime_error("Unsorted Dictionary!");
	else
		for (auto iter = dictionary->lower_bound(word);
			iter != dictionary->upper_bound(word);
			++iter)
			result->push_back(iter);
	return QueryResult(word, dictionary, result);
}

QueryResult Dictionary::query_wildcard(std::wstring wdc_word)
{
	return wildcard_wstring_mapping->query_wildcard(wdc_word);
}
