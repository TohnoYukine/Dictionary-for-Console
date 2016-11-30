#include "stdafx.h"

#include "WildcardWstringSupport.h"
#include "Dictionary.h"
#include "QueryResult.h"

#define NDEBUG


void WildcardWstringSupport::initialize_WildcardPrefix()
{
	for (auto iter = dictionary->begin(); iter != dictionary->end(); ++iter)
		WildcardPrefix_MapTo_Dictionary->emplace(iter->first, iter);
}

void WildcardWstringSupport::initialize_WildcardSuffix()
{
	for (auto iter = dictionary->begin(); iter != dictionary->end(); ++iter)
		WildcardSuffix_MapTo_Dictionary->emplace(iter->first, iter);
}

void WildcardWstringSupport::initialize_WildcardWord()
{
	for (auto iter = dictionary->begin(); iter != dictionary->end(); ++iter)
	{
		std::wistringstream phrase(iter->first);
		while (!phrase.eof())
		{
			std::wstring word;
			phrase >> word;
			WildcardWord_MapTo_Dictionary->emplace(word, iter);
		}
	}
}

bool WildcardWstringSupport::reverse_wstring_less(const std::wstring & lhs, const std::wstring & rhs)
{
	std::wstring::size_type lsz = lhs.size();
	std::wstring::size_type rsz = rhs.size();
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

WildcardWstringSupport::WildcardType WildcardWstringSupport::check_wildcard_type(const std::wstring & wstr)
{
	if (wstr.find(L' ') != std::wstring::npos)
	{
		if (wstr.find(L'*') != std::wstring::npos)
			return WildcardWord;
		else
			return NoWildcard;
	}
	else if (wstr.find(L'?') != std::wstring::npos)
	{
		if (wstr.find(L'*') != std::wstring::npos)
			return Unsupported;
		else
			return FixedPosition;
	}
	else
	{
		if (wstr.find(L'*') == std::wstring::npos)
			return NoWildcard;
		if (wstr.find_first_of(L'*') == 0)
		{
			if (wstr.find_last_of(L'*') == 0)
				return WildcardPrefix;
			else if (wstr.find_last_of(L'*') == wstr.size() - 1)
				return WildcardAffixes;
			else
				return Unsupported;
		}
		else if (wstr.find_last_of(L'*') == wstr.size() - 1)
			return WildcardSuffix;
		else
		{
			std::wstring::size_type pos = wstr.find(L'*');
			if (wstr.find(L'*', ++pos) == std::wstring::npos)
				return WildcardInfix;
			else
				return Unsupported;
		}
	}
		
	return WildcardType();
}

QueryResult WildcardWstringSupport::query_wildcard(const std::wstring & wstr)
{
	WildcardType type = check_wildcard_type(wstr);
	std::shared_ptr<QueryResult::result_type> result(new QueryResult::result_type());
	if (type == Unsupported)
		throw std::runtime_error("Unsupported wildcard type!");
	else if (type == NoWildcard)
	{
		for (auto iter = WildcardSuffix_MapTo_Dictionary->lower_bound(wstr);
			iter != WildcardSuffix_MapTo_Dictionary->upper_bound(wstr);
			++iter)
			result->push_back(iter->second);
	}
	else if (type == WildcardSuffix)
	{
		std::wstring lower_substr = wstr.substr(0, wstr.find(L'*'));
		std::wstring upper_substr(lower_substr);
		++upper_substr[upper_substr.size() - 1];
		
		for (auto iter = WildcardSuffix_MapTo_Dictionary->lower_bound(lower_substr);
			iter != WildcardSuffix_MapTo_Dictionary->lower_bound(upper_substr);
			++iter)
			result->push_back(iter->second);
	}
	else if (type == WildcardPrefix)
	{
		std::wstring lower_substr = wstr.substr(wstr.find(L'*') + 1);
		std::wstring upper_substr(lower_substr);
		++upper_substr[0];

#ifdef DEBUG
		auto var1 = WildcardPrefix_MapTo_Dictionary->lower_bound(lower_substr)->first;
		auto var2 = WildcardPrefix_MapTo_Dictionary->lower_bound(upper_substr)->first;
#endif // DEBUG

		for (auto iter = WildcardPrefix_MapTo_Dictionary->lower_bound(lower_substr);
			iter != WildcardPrefix_MapTo_Dictionary->lower_bound(upper_substr);
			++iter)
			result->push_back(iter->second);
	}
	else if (type == WildcardInfix)
	{
		std::wstring::size_type pos = wstr.find(L'*');
		std::wstring lower_substr = wstr.substr(0, pos);
		std::wstring upper_substr(lower_substr);
		++upper_substr[pos - 1];
		std::wstring suffix_substr = wstr.substr(pos + 1);
		for (auto iter = WildcardSuffix_MapTo_Dictionary->lower_bound(lower_substr);
			iter != WildcardSuffix_MapTo_Dictionary->lower_bound(upper_substr);
			++iter)
		{
			if (iter->first.size() >= wstr.size() - 1 &&
				iter->first.substr(iter->first.size() - suffix_substr.size()) == suffix_substr)
				result->push_back(iter->second);
		}
	}
	else if (type == WildcardWord)
	{
		std::wistringstream phrase(wstr);
		std::wstring word;
		std::vector<std::wstring> words;
		std::deque<std::wstring> words2;
		std::deque<std::wstring> words3;
		while (phrase >> word)
		{
			if (word != L"*" && word != L"?")
				words.push_back(word);
			words2.push_back(word);
		}
		for (auto &wd : words)
		{
			for (auto iter = WildcardWord_MapTo_Dictionary->lower_bound(wd);
				iter != WildcardWord_MapTo_Dictionary->upper_bound(wd);
				++iter)
			{
				std::wistringstream phrase2(iter->second->first);
				std::wstring word2;
				words3.clear();
				while (phrase2 >> word2)
					words3.push_back(word2);
				if (words2.front() != L"*" && words2.front() != L"?")
					if (words3.front() != words2.front())
						continue;
				if (words2.back() != L"*" && words2.back() != L"?")
					if (words3.back() != words2.back())
						continue;
				result->push_back(iter->second);
			}
		}
	}
	else
		throw std::runtime_error("Currently unsupported wildcard type!");
	return QueryResult(wstr, dictionary, result);
}

WildcardWstringSupport::WildcardWstringSupport() :
	dictionary(new Dictionary::Dictionary_type()),
	working_mode({ true,true,true,true,true,true }),
	WildcardPrefix_MapTo_Dictionary(new std::multimap<std::wstring, Dictionary::Entry_iterator, decltype(WildcardWstringSupport::reverse_wstring_less)*>(reverse_wstring_less)),
	WildcardSuffix_MapTo_Dictionary(new std::multimap<std::wstring, Dictionary::Entry_iterator>()),
	WildcardWord_MapTo_Dictionary(new std::multimap<std::wstring, Dictionary::Entry_iterator>()),
	FixedPosition_MapTo_Dictionary(new std::multimap<std::wstring, Dictionary::Entry_iterator>())
{
}

WildcardWstringSupport::WildcardWstringSupport(const Dictionary &dict) :
	dictionary(dict.dictionary),
	working_mode({ true,true,true,true,true,true }),
	WildcardPrefix_MapTo_Dictionary(new std::multimap<std::wstring, Dictionary::Entry_iterator, decltype(WildcardWstringSupport::reverse_wstring_less)*>(reverse_wstring_less)),
	WildcardSuffix_MapTo_Dictionary(new std::multimap<std::wstring, Dictionary::Entry_iterator>()),
	WildcardWord_MapTo_Dictionary(new std::multimap<std::wstring, Dictionary::Entry_iterator>()),
	FixedPosition_MapTo_Dictionary(new std::multimap<std::wstring, Dictionary::Entry_iterator>())
{
	initialize_WildcardPrefix();
	initialize_WildcardSuffix();
	initialize_WildcardWord();
}


WildcardWstringSupport::~WildcardWstringSupport()
{
}
