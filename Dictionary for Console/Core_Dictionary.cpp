#include "stdafx.h"
#include "Core_Dictionary.h"
#include "QueryResult.h"

std::string Core_Dictionary::default_category(u8"Default");
Core_Dictionary::EntryWord_type Core_Dictionary::InvalidWord;
Core_Dictionary::Definition_type Core_Dictionary::InvalidDefinition;
Core_Dictionary::Entry_type Core_Dictionary::InvalidEntry(InvalidWord, InvalidDefinition);

inline void Core_Dictionary::initialize_dictionary(std::istream & is, char delim1, char delim2)
{
	std::string line;
	size_t line_cnt = 0;
	while (iswspace(is.peek())) is.get();
	while (!is.eof())
	{
		std::getline(is, line, delim2);
		++line_cnt;
		dictionary->insert(string_to_entry(line, delim1));
		while (iswspace(is.peek()) && !is.eof()) is.get();
	}
}

inline Core_Dictionary::Entry_type Core_Dictionary::string_to_entry(std::string s, char delim)
{
	std::istringstream istrm(s);
	EntryWord_type newWord;
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
	Definition_type newDef(temp);
	return Entry_type(newWord, newDef);
}

Core_Dictionary::Core_Dictionary() :
	dictionary(new Dictionary_type()),
	category(default_category)
{
}

Core_Dictionary::Core_Dictionary(std::istream &is, Category_type _category) :
	Core_Dictionary(is, u8'\t', u8'\n', _category) {}

Core_Dictionary::Core_Dictionary(std::istream & is, char delim, Category_type _category) :
	Core_Dictionary(is, u8'\t', delim, _category) {}

Core_Dictionary::Core_Dictionary(std::istream & is, char delim1, char delim2, Category_type _category) :
	dictionary(new Dictionary_type()),
	category(_category)
{
	initialize_dictionary(is, delim1, delim2);
}

Core_Dictionary::Core_Dictionary(std::initializer_list<Entry_type> entries) :
	dictionary(new Dictionary_type()),
	category(default_category)
{
	for (auto &entry : entries)
		dictionary->insert(entry);
}

Core_Dictionary::Core_Dictionary(const Core_Dictionary &origin) : 
	dictionary(origin.dictionary),
	category(origin.category)
{
}

Core_Dictionary::Core_Dictionary(Core_Dictionary && origin) :
	category(std::move(origin.category)),
	dictionary(origin.dictionary)
{
	origin.dictionary = nullptr;
}

Core_Dictionary & Core_Dictionary::operator=(const Core_Dictionary & rhs)
{
	category = rhs.category;
	dictionary = rhs.dictionary;
	return *this;
}

Core_Dictionary & Core_Dictionary::operator=(Core_Dictionary && rhs)
{
	category = std::move(rhs.category);
	dictionary = rhs.dictionary;
	rhs.dictionary = nullptr;
	return *this;
}

Core_Dictionary & Core_Dictionary::swap(Core_Dictionary &rhs)
{
	dictionary.swap(rhs.dictionary);
	category.swap(rhs.category);
	return *this;
}

Core_Dictionary::~Core_Dictionary()
{
}

Core_Dictionary::Entry_iterator Core_Dictionary::emplace(const std::string &str, char delim)
{
	return insert(string_to_entry(str, delim));
}

Core_Dictionary::Entry_iterator Core_Dictionary::emplace(const EntryWord_type & _entryword, const Definition_type & _definitions)
{
	return dictionary->emplace(_entryword, _definitions);
}

Core_Dictionary::Entry_iterator Core_Dictionary::insert(const Entry_type & new_entry)
{
	return dictionary->insert(new_entry);
}

void Core_Dictionary::clear()
{
	dictionary = nullptr;
	category.clear();
}

std::shared_ptr<Core_Dictionary::Dictionary_type> Core_Dictionary::erase(QueryResult & words)
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

std::shared_ptr<Core_Dictionary::Entry_type> Core_Dictionary::erase_entry(QueryResult & words, Dictionary_type::size_type pos)
{
	if (pos >= words.size())
		return std::shared_ptr<Entry_type>();
	std::shared_ptr<Entry_type> deleted_entry(new Entry_type(*words[pos]));
	dictionary->erase(words[pos]);
	words.erase(pos);
	return deleted_entry;
}

void Core_Dictionary::operator+=(const Entry_type & entry)
{
	insert(entry);
}

Core_Dictionary::Entry_iterator Core_Dictionary::lower_bound(const EntryWord_type & str)
{
	return dictionary->lower_bound(str);
}

Core_Dictionary::Entry_iterator Core_Dictionary::upper_bound(const EntryWord_type & str)
{
	return dictionary->upper_bound(str);
}

Core_Dictionary::Entry_iterator Core_Dictionary::begin()
{
	return dictionary->begin();
}

Core_Dictionary::Entry_iterator Core_Dictionary::end()
{
	return dictionary->end();
}

Core_Dictionary::const_Entry_iterator Core_Dictionary::begin() const
{
	return cbegin();
}

Core_Dictionary::const_Entry_iterator Core_Dictionary::end() const
{
	return cend();
}

Core_Dictionary::const_Entry_iterator Core_Dictionary::lower_bound(const EntryWord_type & str) const
{
	return dictionary->lower_bound(str);
}

Core_Dictionary::const_Entry_iterator Core_Dictionary::upper_bound(const EntryWord_type & str) const
{
	return dictionary->upper_bound(str);
}

Core_Dictionary::const_Entry_iterator Core_Dictionary::cbegin() const
{
	return dictionary->cbegin();
}

Core_Dictionary::const_Entry_iterator Core_Dictionary::cend() const
{
	return dictionary->cend();
}

void Core_Dictionary::set_category(Category_type _category)
{
	category = _category;
}

bool Core_Dictionary::query_print(const EntryWord_type &word) const
{
	size_t entries = 0;
	std::ostringstream query_result;
	for (auto iter = lower_bound(word); iter != upper_bound(word); ++iter)
	{
		++entries;
		query_result << *iter << u8"\n";
	}
	std::cout << "Entries found: " << entries << std::endl;
	std::cout << query_result.str() << std::flush;
	return (entries != 0);
}

QueryResult Core_Dictionary::query(const EntryWord_type &word)
{
	std::shared_ptr<QueryResult::result_type> result(new QueryResult::result_type());
	for (auto iter = lower_bound(word); iter != upper_bound(word); ++iter)
		result->push_back(iter);
	return QueryResult(word, dictionary, result);
}

std::ostream & operator<<(std::ostream & os, const Core_Dictionary::Entry_type & entry)
{
	os << entry.first << u8"\n";
	os << entry.second;
	return os;
}

bool operator<(Core_Dictionary::Entry_iterator lhs, Core_Dictionary::Entry_iterator rhs)
{
	return (lhs->first < rhs->first);
}

bool operator<(Core_Dictionary::const_Entry_iterator lhs, Core_Dictionary::const_Entry_iterator rhs)
{
	return (lhs->first < rhs->first);
}
