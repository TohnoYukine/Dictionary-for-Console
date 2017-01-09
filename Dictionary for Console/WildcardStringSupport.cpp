#include "stdafx.h"

#include "WildcardStringSupport.h"
#include "QueryResult.h"

void WildcardStringSupport::initialize_WildcardPrefix()
{
	for (auto iter = dictionary->begin(); iter != dictionary->end(); ++iter)
		WildcardPrefix_MapTo_Dictionary->emplace(iter->first, iter);
}

void WildcardStringSupport::initialize_WildcardSuffix()
{
	for (auto iter = dictionary->begin(); iter != dictionary->end(); ++iter)
		WildcardSuffix_MapTo_Dictionary->emplace(iter->first, iter);
}

void WildcardStringSupport::initialize_WildcardWord()
{
	for (auto iter = dictionary->begin(); iter != dictionary->end(); ++iter)
	{
		std::istringstream phrase(iter->first);
		while (!phrase.eof())
		{
			std::string word;
			phrase >> word;
			WildcardWord_MapTo_Dictionary->emplace(word, iter);
		}
	}
}

bool WildcardStringSupport::reverse_string_less(const std::string & lhs, const std::string & rhs)
{
	std::string::size_type lsz = lhs.size();
	std::string::size_type rsz = rhs.size();
	for (int i = 0; i != lsz && i != rsz; ++i)
	{
		if (lhs[lsz - 1 - i] == rhs[rsz - 1 - i])
			continue;
		else if (lhs[lsz - 1 - i] < rhs[rsz - 1 - i])
			return true;
		else
			return false;
	}
	if (lhs.size() < rhs.size())
		return true;
	return false;
}

inline bool WildcardStringSupport::string_less(const std::string & lhs, const std::string & rhs)
{
	return lhs < rhs;
}

void WildcardStringSupport::emplace(const Core_Dictionary::Entry_iterator & new_entry)
{
	WildcardPrefix_MapTo_Dictionary->emplace(new_entry->first, new_entry);

	WildcardSuffix_MapTo_Dictionary->emplace(new_entry->first, new_entry);

	std::istringstream phrase(new_entry->first);
	while (!phrase.eof())
	{
		std::string word;
		phrase >> word;
		WildcardWord_MapTo_Dictionary->emplace(word, new_entry);
	}
}

void WildcardStringSupport::clear()
{
	dictionary = nullptr;
	WildcardPrefix_MapTo_Dictionary = nullptr;
	WildcardSuffix_MapTo_Dictionary = nullptr;
	WildcardWord_MapTo_Dictionary = nullptr;
}

void WildcardStringSupport::reset()
{
	clear();
}

void WildcardStringSupport::reset(std::shared_ptr<Core_Dictionary::Dictionary_type> _dictionary)
{
	reset();
	dictionary = _dictionary;
	WildcardPrefix_MapTo_Dictionary.reset(new std::multimap<std::string, Core_Dictionary::Entry_iterator, bool(*)(const std::string &lhs, const std::string &rhs)>(reverse_string_less));
	WildcardSuffix_MapTo_Dictionary.reset(new std::multimap<std::string, Core_Dictionary::Entry_iterator, bool(*)(const std::string &lhs, const std::string &rhs)>(string_less));
	WildcardWord_MapTo_Dictionary.reset(new std::multimap<std::string, Core_Dictionary::Entry_iterator, bool(*)(const std::string &lhs, const std::string &rhs)>(string_less));
	initialize_WildcardPrefix();
	initialize_WildcardSuffix();
	initialize_WildcardWord();
}

WildcardStringSupport::WildcardType WildcardStringSupport::check_wildcard_type(const std::string & str)
{
	if (str.find(u8' ') != std::string::npos)
	{
		if (str.find(u8'*') != std::string::npos)
			return WildcardWord;
		else
			return NoWildcard;
	}
	else if (str.find(u8'?') != std::string::npos)
	{
		if (str.find(u8'*') != std::string::npos)
			return Unsupported;
		else
			return FixedPosition;
	}
	else
	{
		if (str.find(u8'*') == std::string::npos)
			return NoWildcard;
		if (str.find_first_of(u8'*') == 0)
		{
			if (str.find_last_of(u8'*') == 0)
				return WildcardPrefix;
			else if (str.find_last_of(u8'*') == str.size() - 1)
				return WildcardAffixes;
			else
				return Unsupported;
		}
		else if (str.find_last_of(u8'*') == str.size() - 1)
			return WildcardSuffix;
		else
		{
			std::string::size_type pos = str.find(u8'*');
			if (str.find(u8'*', ++pos) == std::string::npos)
				return WildcardInfix;
			else
				return Unsupported;
		}
	}
		
	return WildcardType();
}

QueryResult WildcardStringSupport::query_wildcard(const std::string & str) const
{
	std::shared_ptr<QueryResult::result_type> result(new QueryResult::result_type());
	if (str.empty())
		return QueryResult(str, dictionary, result);

	WildcardType type = check_wildcard_type(str);

	switch (type)
	{

	case WildcardPrefix:
	{
		return query_wildcard_prefix(str);
	}

	case WildcardSuffix:
	{
		return query_wildcard_suffix(str);
	}

	case WildcardInfix:
	{
		return query_wildcard_infix(str);
	}

	case WildcardAffixes:
	{
		throw std::runtime_error("Currently unsupported wildcard type!");
		break;
	}

	case WildcardWord:
	{
		return query_wildcard_word(str);
	}

	case FixedPosition:
	{
		throw std::runtime_error("Currently unsupported wildcard type!");
		break;
	}

	case Unsupported:
	{
		throw std::runtime_error("Unsupported wildcard type!");
		break;
	}

	case NoWildcard:
	{
		for (auto iter = WildcardSuffix_MapTo_Dictionary->lower_bound(str);
			iter != WildcardSuffix_MapTo_Dictionary->upper_bound(str);
			++iter)
			result->push_back(iter->second);
		break;
	}
	}
	
	return QueryResult(str, dictionary, result);
}

QueryResult WildcardStringSupport::query_wildcard_prefix(const std::string & str) const
{
	std::shared_ptr<QueryResult::result_type> result(new QueryResult::result_type());

	std::string lower_substr = str.substr(str.find(u8'*') + 1);
	std::string upper_substr(lower_substr);
	++upper_substr.front();

	for (auto iter = WildcardPrefix_MapTo_Dictionary->lower_bound(lower_substr);
		iter != WildcardPrefix_MapTo_Dictionary->lower_bound(upper_substr);
		++iter)
		result->push_back(iter->second);

	return QueryResult(str, dictionary, result);
}

QueryResult WildcardStringSupport::query_wildcard_suffix(const std::string & str) const
{
	std::shared_ptr<QueryResult::result_type> result(new QueryResult::result_type());

	std::string lower_substr = str.substr(0, str.find(u8'*'));
	std::string upper_substr(lower_substr);
	++upper_substr.back();

	for (auto iter = WildcardSuffix_MapTo_Dictionary->lower_bound(lower_substr);
		iter != WildcardSuffix_MapTo_Dictionary->lower_bound(upper_substr);
		++iter)
		result->push_back(iter->second);

	return QueryResult(str, dictionary, result);
}

QueryResult WildcardStringSupport::query_wildcard_infix(const std::string & str) const
{
	return query_wildcard_prefix(str) | query_wildcard_suffix(str);
}

QueryResult WildcardStringSupport::query_wildcard_word(const std::string & str) const
{
	std::shared_ptr<QueryResult::result_type> result(new QueryResult::result_type());

	std::istringstream phrase(str);
	std::string word;
	std::vector<std::string> words;
	std::deque<std::string> words2;
	std::deque<std::string> words3;
	while (phrase >> word)
	{
		if (word != u8"*" && word != u8"?")
			words.push_back(word);
		words2.push_back(word);
	}
	for (auto &wd : words)
	{
		for (auto iter = WildcardWord_MapTo_Dictionary->lower_bound(wd);
			iter != WildcardWord_MapTo_Dictionary->upper_bound(wd);
			++iter)
		{
			std::istringstream phrase2(iter->second->first);
			std::string word2;
			words3.clear();
			while (phrase2 >> word2)
				words3.push_back(word2);

			if (words2.front() != u8"*" && words2.front() != u8"?")
				if (words3.front() != words2.front())
					continue;
			if (words2.back() != u8"*" && words2.back() != u8"?")
				if (words3.back() != words2.back())
					continue;

			result->push_back(iter->second);
		}
	}

	return QueryResult(str, dictionary, result);
}

WildcardStringSupport::WildcardStringSupport(const Core_Dictionary &dict) :
	dictionary(dict.dictionary_ptr()),
	working_mode({ true,true,true,true,true,true }),
	WildcardPrefix_MapTo_Dictionary(new std::multimap<std::string, Core_Dictionary::Entry_iterator, bool(*)(const std::string &lhs, const std::string &rhs)>(reverse_string_less)),
	WildcardSuffix_MapTo_Dictionary(new std::multimap<std::string, Core_Dictionary::Entry_iterator, bool(*)(const std::string &lhs, const std::string &rhs)>(string_less)),
	WildcardWord_MapTo_Dictionary(new std::multimap<std::string, Core_Dictionary::Entry_iterator, bool(*)(const std::string &lhs, const std::string &rhs)>(string_less))
{
	initialize_WildcardPrefix();
	initialize_WildcardSuffix();
	initialize_WildcardWord();
}

WildcardStringSupport::WildcardStringSupport(std::shared_ptr<Core_Dictionary::Dictionary_type> _dictionary) :
	dictionary(_dictionary),
	working_mode({ true,true,true,true,true,true }),
	WildcardPrefix_MapTo_Dictionary(new std::multimap<std::string, Core_Dictionary::Entry_iterator, bool(*)(const std::string &lhs, const std::string &rhs)>(reverse_string_less)),
	WildcardSuffix_MapTo_Dictionary(new std::multimap<std::string, Core_Dictionary::Entry_iterator, bool(*)(const std::string &lhs, const std::string &rhs)>(string_less)),
	WildcardWord_MapTo_Dictionary(new std::multimap<std::string, Core_Dictionary::Entry_iterator, bool(*)(const std::string &lhs, const std::string &rhs)>(string_less))
{
	initialize_WildcardPrefix();
	initialize_WildcardSuffix();
	initialize_WildcardWord();
}

WildcardStringSupport::WildcardStringSupport(const WildcardStringSupport & origin) :
	dictionary(origin.dictionary),
	working_mode(origin.working_mode),
	WildcardPrefix_MapTo_Dictionary(origin.WildcardPrefix_MapTo_Dictionary),
	WildcardSuffix_MapTo_Dictionary(origin.WildcardSuffix_MapTo_Dictionary),
	WildcardWord_MapTo_Dictionary(origin.WildcardWord_MapTo_Dictionary) {}

WildcardStringSupport::WildcardStringSupport(WildcardStringSupport && origin) :
	dictionary(origin.dictionary),
	working_mode(std::move(origin.working_mode)),
	WildcardPrefix_MapTo_Dictionary(origin.WildcardPrefix_MapTo_Dictionary),
	WildcardSuffix_MapTo_Dictionary(origin.WildcardSuffix_MapTo_Dictionary),
	WildcardWord_MapTo_Dictionary(origin.WildcardWord_MapTo_Dictionary) {}

WildcardStringSupport::~WildcardStringSupport()
{
}
