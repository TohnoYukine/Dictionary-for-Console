#include "stdafx.h"
#include "Generic_Dictionary.h"
#include "QueryResult.h"

Generic_Dictionary::Generic_Dictionary(std::istream & is, Category_type _category) :
	Generic_Dictionary(is, u8'\t', u8'\n', _category) {}


Generic_Dictionary::Generic_Dictionary(std::istream & is, char delim, Category_type _category) :
	Generic_Dictionary(is, u8'\t', delim, _category) {}

Generic_Dictionary::Generic_Dictionary(std::istream & is, char delim1, char delim2, Category_type _category) :
	Core_Dictionary(is, _category),
	wildcard_string_support(dictionary),
	regex_support(dictionary) {}

Generic_Dictionary::Generic_Dictionary(std::initializer_list<Entry_type> entries) :
	Core_Dictionary(entries),
	wildcard_string_support(dictionary),
	regex_support(dictionary) {}

Generic_Dictionary::Generic_Dictionary(const Generic_Dictionary & origin) :
	Core_Dictionary(origin),	//Slice down!
	wildcard_string_support(origin.wildcard_string_support),
	regex_support(origin.regex_support) {}

Generic_Dictionary::Generic_Dictionary(Generic_Dictionary && origin) :
	wildcard_string_support(std::move(origin.wildcard_string_support)),
	regex_support(std::move(origin.regex_support)),
	Core_Dictionary(std::move(origin)) {}

Generic_Dictionary & Generic_Dictionary::operator=(const Generic_Dictionary & rhs)
{
	dictionary = rhs.dictionary;
	wildcard_string_support.reset(dictionary);
	regex_support.reset(dictionary);
	return *this;
}

Generic_Dictionary & Generic_Dictionary::operator=(Generic_Dictionary && rhs)
{
	clear();
	dictionary = rhs.dictionary;
	wildcard_string_support.reset(dictionary);
	regex_support.reset(dictionary);
	rhs.clear();
	return *this;
}

QueryResult Generic_Dictionary::query_dispatcher(const EntryWord_type & word)
{
	switch (WildcardStringSupport::check_wildcard_type(word))
	{
	case WildcardStringSupport::WildcardPrefix:
	case WildcardStringSupport::WildcardSuffix:
	case WildcardStringSupport::WildcardInfix:
	case WildcardStringSupport::WildcardWord:
		return wildcard_string_support.query_wildcard(word);
	default:
		return regex_support.query_regex(word);
	}
}

Generic_Dictionary::Generic_Dictionary() :
	Core_Dictionary(),
	wildcard_string_support(dictionary),
	regex_support(dictionary) {}


Generic_Dictionary::~Generic_Dictionary() {}

Core_Dictionary::Entry_iterator Generic_Dictionary::emplace(const std::string & str, char delim)
{
	Entry_iterator ret = Core_Dictionary::emplace(str, delim);
	wildcard_string_support.emplace(ret);
	regex_support.emplace(ret);
	return ret;
}

Core_Dictionary::Entry_iterator Generic_Dictionary::emplace(const EntryWord_type & _entryword, const Definition_type & _definitions)
{
	Entry_iterator ret = Core_Dictionary::emplace(_entryword, _definitions);
	wildcard_string_support.emplace(ret);
	regex_support.emplace(ret);
	return ret;
}

Core_Dictionary::Entry_iterator Generic_Dictionary::insert(const Entry_type & new_entry)
{
	Entry_iterator ret = Core_Dictionary::insert(new_entry);
	wildcard_string_support.emplace(ret);
	regex_support.emplace(ret);
	return ret;
}

void Generic_Dictionary::clear()
{
	Core_Dictionary::clear();
	wildcard_string_support.clear();
	regex_support.clear();
}

std::shared_ptr<Generic_Dictionary::Dictionary_type> Generic_Dictionary::erase(QueryResult & words)
{
	std::shared_ptr<Generic_Dictionary::Dictionary_type> ret(Core_Dictionary::erase(words));
	wildcard_string_support.reset(dictionary);
	regex_support.reset(dictionary);
	return ret;
}

std::shared_ptr<Generic_Dictionary::Entry_type> Generic_Dictionary::erase_entry(QueryResult & words, Dictionary_type::size_type pos)
{
	std::shared_ptr<Generic_Dictionary::Entry_type> ret(Core_Dictionary::erase_entry(words, pos));
	wildcard_string_support.reset(dictionary);
	regex_support.reset(dictionary);
	return ret;
}

QueryResult Generic_Dictionary::query(const EntryWord_type & word)
{
	return query_dispatcher(word);
}

void Generic_Dictionary::operator+=(const Entry_type & entry)
{
	Core_Dictionary::operator+=(entry);
	wildcard_string_support.reset(dictionary);
	regex_support.reset(dictionary);
}
