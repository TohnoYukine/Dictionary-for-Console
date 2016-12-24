#include "stdafx.h"

#include "RegexSupport.h"
#include "QueryResult.h"

void RegexSupport::initialize_RegexSupport()
{
	for (auto iter = dictionary->begin(); iter != dictionary->end(); ++iter)
		String_PairTo_Dictionary->emplace(iter->first, iter);
}

void RegexSupport::emplace(const Core_Dictionary::Entry_iterator & new_entry)
{
	String_PairTo_Dictionary->emplace(new_entry->first, new_entry);
}

void RegexSupport::clear()
{
	String_PairTo_Dictionary = nullptr;
	dictionary = nullptr;
}

void RegexSupport::reset()
{
	clear();
}

void RegexSupport::reset(std::shared_ptr<Core_Dictionary::Dictionary_type> _dictionary)
{
	clear();
	dictionary = _dictionary;
	String_PairTo_Dictionary.reset(new std::multimap<std::string, Core_Dictionary::Entry_iterator>());
	initialize_RegexSupport();
}

RegexSupport::RegexSupport() :
	dictionary(new Core_Dictionary::Dictionary_type()),
	String_PairTo_Dictionary(new std::multimap<std::string, Core_Dictionary::Entry_iterator>())
{
}

RegexSupport::RegexSupport(const Core_Dictionary & dict) :
	dictionary(dict.dictionary),
	String_PairTo_Dictionary(new std::multimap<std::string, Core_Dictionary::Entry_iterator>())
{
	initialize_RegexSupport();
}

RegexSupport::RegexSupport(std::shared_ptr<Core_Dictionary::Dictionary_type> _dictionary) :
	dictionary(_dictionary),
	String_PairTo_Dictionary(new std::multimap<std::string, Core_Dictionary::Entry_iterator>())
{
	initialize_RegexSupport();
}

RegexSupport::RegexSupport(const RegexSupport & origin) :
	dictionary(origin.dictionary),
	String_PairTo_Dictionary(origin.String_PairTo_Dictionary) {}

RegexSupport::RegexSupport(RegexSupport && origin) :
	dictionary(origin.dictionary),
	String_PairTo_Dictionary(origin.String_PairTo_Dictionary) {}

QueryResult RegexSupport::query_regex(const std::string & str)
{
	const std::regex re(str);
	std::shared_ptr<QueryResult::result_type> result(new QueryResult::result_type());
	for (auto iter = String_PairTo_Dictionary->cbegin(); iter != String_PairTo_Dictionary->cend(); ++iter)
		if (std::regex_match(iter->first, re))
			result->push_back(iter->second);
	return QueryResult(str, dictionary, result);
}


RegexSupport::~RegexSupport()
{
}
