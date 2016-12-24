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

#include "Definitions.h"

class QueryResult;								//Allows user to query as well as add entries
class Core_Dictionary
{
public:
	using Category_type = std::string;
	using EntryWord_type = std::string;
	using Definition_type = Definitions;
	using Entry_type = std::pair<EntryWord_type, Definition_type>;
	using Dictionary_type = std::multimap<EntryWord_type, Definition_type>;
	using Entry_iterator = std::multimap<EntryWord_type, Definition_type>::iterator;
	using const_Entry_iterator = std::multimap<EntryWord_type, Definition_type>::const_iterator;

	static Category_type default_category;
	static EntryWord_type InvalidWord;
	static Definition_type InvalidDefinition;
	static Entry_type InvalidEntry;

private:
	friend class QueryResult;
	friend class WildcardStringSupport;
	friend class RegexSupport;

//Data members
protected:
	Category_type category;								//User may use different dictionaries. Used to check whether Dictionary can be combined.
	std::shared_ptr<Dictionary_type> dictionary;

private:
//Assisting member functions used to do actual work.
	inline void initialize_dictionary(std::istream &is, char delim1 = u8'\t', char delim2 = u8'\n');	//delim2 separate entries, delim1 separate entry word and definition
	inline Entry_type string_to_entry(std::string s, char delim);

public:
//Copy Control
	explicit Core_Dictionary();

	//delim separates entries
	//delim2 separate entries, delim1 separate entry word and definition
	explicit Core_Dictionary(std::istream &is, Category_type _category = default_category);
	explicit Core_Dictionary(std::istream &is, char delim, Category_type _category = default_category);
	explicit Core_Dictionary(std::istream &is, char delim1, char delim2, Category_type _category = default_category);

	explicit Core_Dictionary(std::initializer_list<Entry_type> entries);

	explicit Core_Dictionary(const Core_Dictionary &origin);	//Implicitly call copy constructor is too expensive, users should know what they do.					
	Core_Dictionary(Core_Dictionary &&origin);

//Copy Assignment
	Core_Dictionary &operator=(const Core_Dictionary &rhs);
	Core_Dictionary &operator=(Core_Dictionary &&rhs);

	virtual Core_Dictionary &swap(Core_Dictionary &);
	virtual ~Core_Dictionary();

public:
//STL-like functions

	//Following functions are used to add new entries to a sorted dictionary.
	//DataMode defaultly set as DictionaryOnly. When adding a backup at temp_entries, specify RawAndDictionary.
	virtual Entry_iterator emplace(const std::string &str, char delim);
	virtual Entry_iterator emplace(const EntryWord_type &_entryword, const Definition_type &_definitions);
	virtual Entry_iterator insert(const Entry_type &new_entry);

	virtual void clear();

	//Functions related to erase should return data erased.
	//Directly erase from dictionary is not allowed. User should query first to erase.
	virtual std::shared_ptr<Dictionary_type> erase(QueryResult &words);	
	virtual std::shared_ptr<Entry_type> erase_entry(QueryResult &words, Dictionary_type::size_type pos);	



public:
	virtual void operator+=(const Entry_type &entry);		//Add Entry_type
	virtual void operator+=(const Core_Dictionary &rhs) = delete;	//Combine Library

	//This two are used to deal with complicated formats, which should be handled by user.
	Entry_iterator begin();
	Entry_iterator end();
	const_Entry_iterator begin() const;
	const_Entry_iterator end() const;
	const_Entry_iterator cbegin() const;
	const_Entry_iterator cend() const;

	Entry_iterator lower_bound(const EntryWord_type &str);
	Entry_iterator upper_bound(const EntryWord_type &str);
	const_Entry_iterator lower_bound(const EntryWord_type &str) const;
	const_Entry_iterator upper_bound(const EntryWord_type &str) const;


public:
	void set_category(Category_type _category);

	bool query_print(const EntryWord_type &word) const;				//Marked as deprecated, return false if no entry found.

	virtual QueryResult query(const EntryWord_type &word);
};

std::ostream& operator<<(std::ostream &os, const Core_Dictionary::Entry_type &entry);

bool operator<(Core_Dictionary::Entry_iterator lhs, Core_Dictionary::Entry_iterator rhs);
bool operator<(Core_Dictionary::const_Entry_iterator lhs, Core_Dictionary::const_Entry_iterator rhs);