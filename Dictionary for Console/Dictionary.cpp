#include "stdafx.h"
#include "Dictionary.h"
#include "QueryResult.h"
#include "WildcardStringSupport.h"

#define DEPRECATE_MARKED_CODE

std::string Dictionary::default_category(u8"Default");
Dictionary::EntryWord Dictionary::InvalidWord;
Definitions Dictionary::InvalidDefinition;
Dictionary::Entry_type Dictionary::InvalidEntry(InvalidWord, InvalidDefinition);

inline void Dictionary::get_data(std::istream & is, char delim1, char delim2)
{
	std::string line;
	size_t line_cnt = 0;
	while (iswspace(is.peek())) is.get();
	if (data_mode == RawDataOnly)
	{
		while (!is.eof())
		{
			std::getline(is, line, delim2);
			++line_cnt;
			temp_entries.push_back(string_to_entry(line, delim1));
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
			dictionary->insert(string_to_entry(line, delim1));
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
			Entry_type new_entry(string_to_entry(line, delim1));
			temp_entries.push_back(new_entry);
			dictionary->insert(new_entry);
			while (iswspace(is.peek()) && !is.eof()) is.get();
		}
		is_synchronized = Synchronous;
	}
//	std::cout << u8"Data loading completed." << std::endl;
}

inline Dictionary::Entry_type Dictionary::string_to_entry(std::string s, char delim)
{
	std::istringstream istrm(s);
	EntryWord newWord;
	std::string temp;
	while (iswspace(istrm.peek())) istrm.get();
	if (!istrm.eof())
	{
		try
		{
			std::getline(istrm, newWord, delim);
			newWord = newWord.substr(0, newWord.find_last_not_of(u8" \t\n") + 1);
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
			std::cerr << newWord << u8":\t" << input_fail.what() << std::endl;
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
	wildcard_string_mapping(new WildcardStringSupport())
{
}

Dictionary::Dictionary(std::istream & is, DataMode _mode) :
	Dictionary(is, default_category, u8' ', u8'\n', _mode) {}

Dictionary::Dictionary(std::istream & is, char delim, DataMode _mode) :
	Dictionary(is, default_category, u8' ', delim, _mode) {}

Dictionary::Dictionary(std::istream & is, char delim1, char delim2, DataMode _mode) :
	Dictionary(is, default_category, delim1, delim2, _mode) {}

Dictionary::Dictionary(std::istream & is, std::string _category, DataMode _mode) :
	Dictionary(is, _category, u8' ', u8'\n', _mode) {}

Dictionary::Dictionary(std::istream & is, std::string _category, char delim, DataMode _mode) :
	Dictionary(is, _category, u8' ', delim, _mode) {}

Dictionary::Dictionary(std::istream & is, std::string _category, char delim1, char delim2, DataMode _mode) :
	temp_entries(),
	dictionary(new Dictionary_type()),
	is_complete(Complete),
	data_mode(_mode),
	category(_category),
	wildcard_string_mapping(new WildcardStringSupport())
{
	is_sorted = (data_mode == RawDataOnly) ? Unsorted : Sorted;
	is_complete = (data_mode == DictionaryOnly) ? Incomplete : Complete;
	get_data(is, delim1, delim2);
}

Dictionary::Dictionary(std::ifstream & ifile, DataMode _mode) :
	Dictionary(ifile, default_category, u8' ', u8'\n', _mode) {}

Dictionary::Dictionary(std::ifstream & ifile, char delim, DataMode _mode) :
	Dictionary(ifile, default_category, u8' ', delim, _mode) {}

Dictionary::Dictionary(std::ifstream & ifile, char delim1, char delim2, DataMode _mode) :
	Dictionary(ifile, default_category, delim1, delim2, _mode) {}

Dictionary::Dictionary(std::ifstream & ifile, std::string _category, DataMode _mode) :
	Dictionary(ifile, _category, u8' ', u8'\n', _mode) {}

Dictionary::Dictionary(std::ifstream & ifile, std::string _category, char delim, DataMode _mode) :
	Dictionary(ifile, _category, u8' ', delim, _mode) {}

Dictionary::Dictionary(std::ifstream & ifile, std::string _category, char delim1, char delim2, DataMode _mode) :
	temp_entries(), 
	dictionary(new Dictionary_type()),
	is_complete(Complete),
	data_mode(_mode),
	category(_category),
	wildcard_string_mapping(new WildcardStringSupport())
{
	is_sorted = (data_mode == RawDataOnly) ? Unsorted : Sorted;
	is_complete = (data_mode == DictionaryOnly) ? Incomplete : Complete;
	if (!ifile.eof())
	{
		std::stringstream file_buf;
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
	wildcard_string_mapping(new WildcardStringSupport())
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
	wildcard_string_mapping(origin.wildcard_string_mapping)
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
	std::swap(wildcard_string_mapping, rhs.wildcard_string_mapping);
	return *this;
}

Dictionary::~Dictionary()
{
}

void Dictionary::emplace_back(const std::string &str, char delim)
{
	temp_entries.push_back(string_to_entry(str, delim));

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

void Dictionary::emplace(const std::string &str, char delim, DataMode _mode)
{
	data_mode = _mode;

	if (is_sorted == Unsorted)
		throw std::runtime_error("Unsorted Dictionary!");

	Entry_type newEntry(string_to_entry(str, delim));
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

void Dictionary::add_entry(const std::string &str, char delim)
{
	Entry_type newEntry(string_to_entry(str, delim));
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

void Dictionary::set_category(std::string _category)
{
	category = _category;
}

void Dictionary::enable_wildcard()
{
	wildcard_string_mapping.reset(new WildcardStringSupport(*this));
}

void Dictionary::sort()
{
	if (is_complete == Complete)
		for (const Entry_type &currEntry : temp_entries)
			dictionary->insert(currEntry);

	dictionary->erase(InvalidWord);

	is_sorted = Sorted;
}

bool Dictionary::query_print(std::string word)
{
	if (is_sorted == Sorted)
	{
		size_t entries = 0;
		std::ostringstream query_result;
		for (auto iter = dictionary->lower_bound(word); iter != dictionary->upper_bound(word); ++iter)
		{
			++entries;
//			query_result << iter->first << "\t" << iter->second << std::endl;
		}
		std::cout << "Entries found: " << entries << std::endl;
		std::cout << query_result.str() << std::endl;
		return (entries != 0);
	}
	else
	{
		std::wcerr << u8"Querying with unsorted dictionary is currently unsupported. Please sort first." << std::endl;
		return false;
	}
}

QueryResult Dictionary::query(std::string word)
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

QueryResult Dictionary::query_wildcard(std::string wdc_word)
{
	return wildcard_string_mapping->query_wildcard(wdc_word);
}
