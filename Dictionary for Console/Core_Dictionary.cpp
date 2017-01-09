#include "stdafx.h"
#include "Core_Dictionary.h"
#include "QueryResult.h"

Core_Dictionary::EntryWord_type Core_Dictionary::InvalidWord;
Core_Dictionary::Definition_type Core_Dictionary::InvalidDefinition;
Core_Dictionary::Entry_type Core_Dictionary::InvalidEntry(InvalidWord, InvalidDefinition);

inline void Core_Dictionary::initialize_dictionary(std::istream & is, char delim1, char delim2)
{
	std::string line;
	size_t line_cnt = 0;
	while (!is.eof())
	{
		++line_cnt;
		dictionary->insert(Entry_type(is, delim1, delim2));
	}
}

Core_Dictionary::Core_Dictionary() :
	dictionary(new Dictionary_type()) {}

Core_Dictionary::Core_Dictionary(std::istream &is) :
	Core_Dictionary(is, u8'\t', u8'\n') {}

Core_Dictionary::Core_Dictionary(std::istream & is, char delim) :
	Core_Dictionary(is, u8'\t', delim) {}

Core_Dictionary::Core_Dictionary(std::istream & is, char delim1, char delim2) :
	dictionary(new Dictionary_type())
{
	initialize_dictionary(is, delim1, delim2);
}

Core_Dictionary::Core_Dictionary(std::initializer_list<Entry_type> entries) :
	dictionary(new Dictionary_type())
{
	for (auto &entry : entries)
		dictionary->insert(entry);
}

Core_Dictionary::Core_Dictionary(const Core_Dictionary &origin) : 
	dictionary(new Dictionary_type(*origin.dictionary))
{
}

Core_Dictionary::Core_Dictionary(Core_Dictionary && origin) :
	dictionary(origin.dictionary)
{
	origin.dictionary = nullptr;
}

Core_Dictionary & Core_Dictionary::operator=(const Core_Dictionary & rhs)
{
	dictionary.reset(new Dictionary_type(*rhs.dictionary));
	return *this;
}

Core_Dictionary & Core_Dictionary::operator=(Core_Dictionary && rhs)
{
	dictionary = rhs.dictionary;
	rhs.dictionary = nullptr;
	return *this;
}

Core_Dictionary & Core_Dictionary::swap(Core_Dictionary &rhs)
{
	dictionary.swap(rhs.dictionary);
	return *this;
}

Core_Dictionary::~Core_Dictionary()
{
}

Core_Dictionary::Entry_iterator Core_Dictionary::emplace(const std::string &str, char delim)
{
	return insert(Entry_type(str, delim));
}

Core_Dictionary::Entry_iterator Core_Dictionary::emplace(const EntryWord_type & _entryword, const Definition_type & _definitions)
{
	return dictionary->emplace(_entryword, _definitions);
}

Core_Dictionary::Entry_iterator Core_Dictionary::insert(const Entry_type & new_entry)
{
	if (!new_entry.entry_word().empty())
		return dictionary->insert(new_entry);
	else
		return dictionary->begin();
}

void Core_Dictionary::clear()
{
	dictionary = nullptr;
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
	std::shared_ptr<Entry_type> deleted_entry(new Entry_type(words[pos]));
	dictionary->erase(*(words.begin() + pos));
	words.erase(pos);
	return deleted_entry;
}

std::shared_ptr<Core_Dictionary::Dictionary_type> Core_Dictionary::dictionary_ptr()
{
	return dictionary;
}

std::shared_ptr<Core_Dictionary::Dictionary_type> Core_Dictionary::dictionary_ptr() const
{
	return dictionary;
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

#ifdef LOW_MEMORY_REQUIREMENT
void Core_Dictionary::set_dictionary_path(std::shared_ptr<std::string> _dictionary_path)
{
	for (auto &entry : *this)
		entry.second.set_dictionary_path(_dictionary_path);
}
#endif // LOW_MEMORY_REQUIREMENT


std::ostream & operator<<(std::ostream & os, const Core_Dictionary::Entry_type & entry)
{
	os << entry.entry_word() << u8"\n";
	os << entry.definitions();
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

inline Entry::Entry(const std::string & str, char delim)
{
	std::string::size_type word_beg = str.find_first_not_of(u8" \t\n");
	std::string::size_type word_end = str.find(delim, word_beg);
	first = str.substr(word_beg, word_end);

	if (word_end != std::string::npos && word_end != str.size() - 1)
		second = Definitions(str.substr(str.find_first_not_of(u8" \t\n", word_end + 1), str.find_last_not_of(u8" \t\n")));
}

inline Entry::Entry(std::istream & is, char delim1, char delim2)	//Definitions are initialized with Definitions(Description_type && _description) noexcept.
{
	while (isspace(is.peek())) is.get();
	std::getline(is, first, delim1);

	while (isspace(is.peek())) is.get();
	std::string temp;
	std::getline(is, temp, delim2);
	second = Definitions(std::move(temp));
}

inline bool Entry::valid() const
{
	return !first.empty();
}

inline Entry2::Entry2(const std::string & _entry_word, std::streampos def_pos_beg, std::streampos def_pos_end)
{
	first = _entry_word;
	second = DefinitionPos(def_pos_beg, def_pos_end);
}

inline Entry2::Entry2(std::istream & is, char delim1, char delim2)
{
	while (isspace(is.peek())) is.get();
	std::getline(is, first, delim1);

	while (isspace(is.peek())) is.get();
	std::streampos def_pos_beg = is.tellg();
	while (!is.eof() && is.get() != static_cast<int>(delim2));
	std::streampos def_pos_end = is.tellg();
	def_pos_end -= 1;	//Ignore delim2
	second = DefinitionPos(def_pos_beg, def_pos_end);
}

inline Entry2::Entry2(const std::string & str, char delim)
{
	throw std::runtime_error("Trying to call Entry2(const std::string & str, char delim).");
}

inline void Entry2::set_dictionary_path(std::shared_ptr<std::string> _dictionary_path)
{
	second.set_dictionary_path(_dictionary_path);
}
	
	