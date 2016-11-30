#pragma once
#include <iostream>
#include <fstream>
#include <sstream>

#include <map>
#include <vector>
#include <deque>
#include <iterator>
#include <string>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <set>
#include <cctype>

#include "Dictionary.h"

//How about inheritance? Yet unavailable.
class QueryResult;
class WildcardWstringSupport
{
	friend class Dictionary;

	using key_type = std::wstring;
	using mapped_type = Dictionary::Entry_iterator;
	using value_type = std::multimap<key_type, mapped_type>::value_type;
	using iterator = std::multimap<key_type, mapped_type>::iterator;

	//'*' and '?' are considered wildcards.
	//'*' can substitute no or any number of characters.
	//'?' can substitute no or one character.
	//'*' and '?' cannot be used at same time.

	enum WildcardType { WildcardPrefix = 0, WildcardSuffix = 1, WildcardInfix = 2, WildcardAffixes = 3, WildcardWord = 4, FixedPosition = 5, Unsupported = 6, NoWildcard = 7 };
	//WildcardPrefix	*able	|	table, able	|	a map of word entry sorted in an order that the reverse of word is in normal order. Mapped toward dictionary entry_iterator.
	//WildcardInfix		ab*le	|	abile, able	|	processed as WildcardSuffix, then WildcardPrefix.
	//WildcardSuffix	able*	|	ablet, able	|	a map of word entry mapped toward dictionary entry_iterator. 

	//WildcardAffixes	*able*	|	able, tablet	|	a map of word entry mapped toward dictionary entry_iterator. Query by iterate through the container. Further optimization needed.

	//WildcardWord		* able *, * able, able *	|	be able to, be able, able	|	a map of words of sparsed phrases.
	//Quite easy to dual with this. Just map individual words of the phrase to phrase itself.

	//FixedPosition		ab?le	|	abile, able	|	a map of word entry mapped toward dictionary entry_iterator. 

	//C++ Standard 23.1.2.8:
	//The insert members shall not affect the validity of iterators and references to the container, and the erase members shall invalidate only iterators and references to the erased elements.

public:
	void initialize_WildcardPrefix();	//For WildcardPrefix 
	void initialize_WildcardSuffix();	//For WildcardInfix, WildcardSuffix, NoWildcard
	void initialize_WildcardWord();		//For WildcardWord

	void initialize_FixedPosition();	//For WildcardAffixes, FixedPosition, currently not implemented

private:

	std::shared_ptr<Dictionary::Dictionary_type> dictionary;

	static bool reverse_wstring_less(const std::wstring &lhs, const std::wstring &rhs);
	std::shared_ptr<std::multimap<std::wstring, Dictionary::Entry_iterator, decltype(WildcardWstringSupport::reverse_wstring_less)*>> WildcardPrefix_MapTo_Dictionary;

	std::shared_ptr<std::multimap<std::wstring, Dictionary::Entry_iterator>> WildcardSuffix_MapTo_Dictionary;

	std::shared_ptr<std::multimap<std::wstring, Dictionary::Entry_iterator>> WildcardWord_MapTo_Dictionary;

	std::shared_ptr<std::multimap<std::wstring, Dictionary::Entry_iterator>> FixedPosition_MapTo_Dictionary;

	struct WorkingMode
	{
		bool is_WildcardPrefix_enabled;
		bool is_WildcardSuffix_enabled;
		bool is_WildcardInfix_enabled;
		bool is_WildcardAffix_enabled;
		bool is_WildcardWord_enabled;
		bool is_FixedPosition_enabled;
	} working_mode;

private:
	void set_working_mode(bool enablePrefix, bool enableSuffix, bool enableInfix, bool enableAffix, bool enableWord, bool enableFixedPosition);

private:

public:
	WildcardType check_wildcard_type(const std::wstring &wstr);
	QueryResult query_wildcard(const std::wstring &wstr);

public:
	//Querying a Wildcardstring is time-consuming. User should know what they do.
	explicit WildcardWstringSupport();	//Used to default initialize WildcardWstringSupport when a Dictionary object is created.
	explicit WildcardWstringSupport(const Dictionary &dict);
	~WildcardWstringSupport();

	//To Do:
	/*
	*	bool operator==
	*	bool operator!=
	*	bool operator=
	*
	*
	*
	*
	*
	*/
};

