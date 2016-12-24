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
#include <regex>

#include "Core_Dictionary.h"

class QueryResult;
class RegexSupport
{
	friend class Generic_Dictionary;

	using key_type = std::string;
	using mapped_type = Core_Dictionary::Entry_iterator;
	using value_type = std::multimap<key_type, mapped_type>::value_type;
	using iterator = std::multimap<key_type, mapped_type>::iterator;

private:
	std::shared_ptr<Core_Dictionary::Dictionary_type> dictionary;

	std::shared_ptr<std::multimap<std::string, Core_Dictionary::Entry_iterator>> String_PairTo_Dictionary;

private:
	void initialize_RegexSupport();
	//There is no good way to way to order regex objects!
	void emplace(const Core_Dictionary::Entry_iterator & new_entry);
	void clear();
	void reset();
	void reset(std::shared_ptr<Core_Dictionary::Dictionary_type> _dictionary);

public:
	explicit RegexSupport();
	explicit RegexSupport(const Core_Dictionary &dict);
	explicit RegexSupport(std::shared_ptr<Core_Dictionary::Dictionary_type> _dictionary);

	explicit RegexSupport(const RegexSupport &origin);
	RegexSupport(RegexSupport &&origin);

private:
	QueryResult query_regex(const std::string &str);

	~RegexSupport();
};

