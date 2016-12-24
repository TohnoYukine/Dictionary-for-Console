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

#include "Core_Dictionary.h"

//How about inheritance? Yet unavailable.
class QueryResult;
class WildcardStringSupport
{
	friend class Generic_Dictionary;

	using key_type = std::string;
	using mapped_type = Core_Dictionary::Entry_iterator;
	using value_type = std::multimap<key_type, mapped_type>::value_type;
	using iterator = std::multimap<key_type, mapped_type>::iterator;

	//'*' and '?' are considered wildcards.
	//'*' can substitute no or any number of characters.
	//'?' can substitute no or one character.
	//'*' and '?' cannot be used at same time.

	enum WildcardType 
	{
		WildcardPrefix = 0,
		WildcardSuffix = 1, 
		WildcardInfix = 2, 
		WildcardAffixes = 3,
		WildcardWord = 4, 
		FixedPosition = 5, 
		Unsupported = 6, 
		NoWildcard = 7
	};

	//WildcardPrefix	*able	|	table, able	|	a map of word entry sorted in an order that the reverse of word is in normal order. Mapped toward dictionary entry_iterator.
	//WildcardInfix		ab*le	|	abile, able	|	processed as WildcardSuffix, then WildcardPrefix.
	//WildcardSuffix	able*	|	ablet, able	|	a map of word entry mapped toward dictionary entry_iterator. 

	//WildcardAffixes	*able*	|	able, tablet	|	a map of word entry mapped toward dictionary entry_iterator. Query by iterate through the container. Further optimization needed.

	//WildcardWord		* able *, * able, able *	|	be able to, be able, able	|	a map of words of sparsed phrases.
	//Quite easy to dual with this. Just map individual words of the phrase to phrase itself.

	//FixedPosition		ab?le	|	abile, able	|	a map of word entry mapped toward dictionary entry_iterator. 

	//C++ Standard 23.1.2.8:
	//The insert members shall not affect the validity of iterators and references to the container, and the erase members shall invalidate only iterators and references to the erased elements.

protected:
	void initialize_WildcardPrefix();	//For WildcardPrefix 
	void initialize_WildcardSuffix();	//For WildcardSuffix
	void initialize_WildcardWord();		//For WildcardWord

//	void initialize_FixedPosition();	//For WildcardAffixes, FixedPosition, currently not implemented

private:
	//shared_ptr to dictionary.
	std::shared_ptr<Core_Dictionary::Dictionary_type> dictionary;

	static bool reverse_string_less(const std::string &lhs, const std::string &rhs);
	std::shared_ptr<std::multimap<std::string, Core_Dictionary::Entry_iterator, bool(*)(const std::string &lhs, const std::string &rhs)>> WildcardPrefix_MapTo_Dictionary;

	std::shared_ptr<std::multimap<std::string, Core_Dictionary::Entry_iterator, bool(*)(const std::string &lhs, const std::string &rhs)>> WildcardSuffix_MapTo_Dictionary;

	std::shared_ptr<std::multimap<std::string, Core_Dictionary::Entry_iterator, bool(*)(const std::string &lhs, const std::string &rhs)>> WildcardWord_MapTo_Dictionary;

	struct WorkingMode
	{
		bool is_WildcardPrefix_enabled;
		bool is_WildcardSuffix_enabled;
		bool is_WildcardInfix_enabled;
		bool is_WildcardAffix_enabled;
		bool is_WildcardWord_enabled;
		bool is_FixedPosition_enabled;
	} working_mode;

public:
	void set_working_mode(bool enablePrefix, bool enableSuffix, bool enableInfix, bool enableAffix, bool enableWord, bool enableFixedPosition);

private:
	void emplace(const Core_Dictionary::Entry_iterator &new_entry);
	void clear();
	void reset();
	void reset(std::shared_ptr<Core_Dictionary::Dictionary_type> _dictionary);

public:
	WildcardType check_wildcard_type(const std::string &str);

private:
	QueryResult query_wildcard(const std::string &str);
	inline QueryResult query_wildcard_prefix(const std::string &str);
	inline QueryResult query_wildcard_suffix(const std::string &str);
	inline QueryResult query_wildcard_infix(const std::string &str);
	inline QueryResult query_wildcard_word(const std::string &str);


public:
	//Querying a Wildcardstring is time-consuming. User should know what they do.
	explicit WildcardStringSupport();	//Used to default initialize WildcardStringSupport when a Core_Dictionary object is created.
	explicit WildcardStringSupport(const Core_Dictionary &dict);
	explicit WildcardStringSupport(std::shared_ptr<Core_Dictionary::Dictionary_type> _dictionary);

	explicit WildcardStringSupport(const WildcardStringSupport &origin);
	WildcardStringSupport(WildcardStringSupport &&origin);

	~WildcardStringSupport();
};

