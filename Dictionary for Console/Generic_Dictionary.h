#pragma once
#include <iostream>
#include <fstream>
#include <sstream>

#include <map>
#include <vector>
#include <iterator>
#include <string>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <set>
#include <cctype>

#include "Core_Dictionary.h"
#include "WildcardStringSupport.h"
#include "RegexSupport.h"

class Generic_Dictionary final : public Core_Dictionary
{
private:
	friend class QueryResult;

private:
	WildcardStringSupport wildcard_string_support;
	RegexSupport regex_support;

public:
	Generic_Dictionary();

	explicit Generic_Dictionary(std::istream &is, Category_type _category = default_category);
	explicit Generic_Dictionary(std::istream &is, char delim, Category_type _category = default_category);
	explicit Generic_Dictionary(std::istream &is, char delim1, char delim2, Category_type _category = default_category);

	explicit Generic_Dictionary(std::initializer_list<Entry_type> entries);

	explicit Generic_Dictionary(const Generic_Dictionary &origin);
	Generic_Dictionary(Generic_Dictionary &&origin);

	Generic_Dictionary &operator=(const Generic_Dictionary &rhs);
	Generic_Dictionary &operator=(Generic_Dictionary &&rhs);

	virtual Generic_Dictionary &swap(Generic_Dictionary &) = delete;

	virtual ~Generic_Dictionary();



public:
	//STL-like functions

	//Following functions are used to add new entries to a sorted dictionary.
	//DataMode defaultly set as DictionaryOnly. When adding a backup at temp_entries, specify RawAndDictionary.
	virtual Entry_iterator emplace(const std::string &str, char delim) override;
	virtual Entry_iterator emplace(const EntryWord_type &_entryword, const Definition_type &_definitions) override;
	virtual Entry_iterator insert(const Entry_type &new_entry) override;

	virtual void clear() override;

	//Functions related to erase should return data erased.
	//Directly erase from dictionary is not allowed. User should query first to erase.
	virtual std::shared_ptr<Dictionary_type> erase(QueryResult &words) override;
	virtual std::shared_ptr<Entry_type> erase_entry(QueryResult &words, Dictionary_type::size_type pos) override;

//	virtual QueryResult query(const EntryWord_type &word) override;

public:
	virtual void operator+=(const Entry_type &entry) override;		//Add Entry_type
	virtual void operator+=(const Core_Dictionary &rhs) override = delete;	//Combine Library

};

