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

#define LOW_MEMORY_REQUIREMENT

#ifdef LOW_MEMORY_REQUIREMENT
#undef HIGH_PERFORMANCE
#endif // LOW_MEMORY_REQUIREMENT


#include "Definitions.h"
#include "DefinitionPos.h"

class QueryResult;			//Allows user to query as well as add entries

class Entry : public std::pair<std::string, Definitions>
{
private:
	using std::pair<std::string, Definitions>::first;
	using std::pair<std::string, Definitions>::second;

public:
	using EntryWord_type = std::string;
	using Definition_type = Definitions;

	using std::pair<std::string, Definitions>::pair;
	inline explicit Entry(const std::string &str, char delim);
	inline explicit Entry(std::istream &is, char delim1, char delim2);
	inline bool valid() const;

	inline std::string &entry_word() { return first; }
	inline Definitions &definitions() { return second; }
	inline const std::string &entry_word() const { return first; }
	inline const Definitions &definitions() const { return second; }
};

//Used with DefinitionPos
class Entry2 : public std::pair<std::string, DefinitionPos>
{
private:
	using std::pair<std::string, DefinitionPos>::first;
	using std::pair<std::string, DefinitionPos>::second;

public:
	using EntryWord_type = std::string;
	using Definition_type = DefinitionPos;

	using std::pair<std::string, DefinitionPos>::pair;
	inline explicit Entry2(const std::string &_entry_word, std::streampos def_pos_beg, std::streampos def_pos_end);
	inline explicit Entry2(std::istream &is, char delim1, char delim2);
	inline explicit Entry2(const std::string &str, char delim);	//Used to keep the same style as Entry, does not work!!

	inline void set_dictionary_path(std::shared_ptr<std::string> _dictionary_path);	//Set _dictionary_path for DefinitionPos to visit the file.

	inline std::string &entry_word() { return first; }
	inline DefinitionPos &definitions() { return second; }
	inline const std::string &entry_word() const { return first; }
	inline const DefinitionPos &definitions() const { return second; }
};

//Data leaks from iterators. Need some further encapsulation.
class Entry_iterator : public std::multimap<Entry::EntryWord_type, Entry::Definition_type>::iterator
{
public:
	inline Entry operator*() const { return Entry(std::multimap<Entry::EntryWord_type, Entry::Definition_type>::iterator::operator*()); }
	inline Entry *operator->() const { return &operator*(); }
};

class Entry2_iterator : public std::multimap<Entry2::EntryWord_type, Entry2::Definition_type>::iterator
{
public:
	inline Entry2 operator*() const { return Entry2(std::multimap<Entry2::EntryWord_type, Entry2::Definition_type>::iterator::operator*()); }
	inline Entry2 *operator->() const { return &operator*(); }
};


class Core_Dictionary
{
public:
#ifdef HIGH_PERFORMANCE
	using Entry_type = Entry;
#endif // HIGH_PERFORMANCE
#ifdef LOW_MEMORY_REQUIREMENT
	using Entry_type = Entry2;
#endif // LOW_MEMORY_REQUIREMENT

	using EntryWord_type = Entry_type::EntryWord_type;
	using Definition_type = Entry_type::Definition_type;

	using Dictionary_type = std::multimap<EntryWord_type, Definition_type>;
	using Entry_iterator = std::multimap<EntryWord_type, Definition_type>::iterator;

	using const_Entry_iterator = std::multimap<EntryWord_type, Definition_type>::const_iterator;

	static EntryWord_type InvalidWord;
	static Definition_type InvalidDefinition;
	static Entry_type InvalidEntry;

private:
//	friend class QueryResult;
//	friend class WildcardStringSupport;
//	friend class RegexSupport;

//Data members
protected:
	std::shared_ptr<Dictionary_type> dictionary;

private:
//Assisting member functions used to do actual work.
	inline void initialize_dictionary(std::istream &is, char delim1 = u8'\t', char delim2 = u8'\n');	//delim2 separate entries, delim1 separate entry word and definition

public:
//Copy Control
	explicit Core_Dictionary();

	//delim separates entries
	//delim2 separate entries, delim1 separate entry word and definition
	explicit Core_Dictionary(std::istream &is);
	explicit Core_Dictionary(std::istream &is, char delim);
	explicit Core_Dictionary(std::istream &is, char delim1, char delim2);

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
	virtual Entry_iterator insert(const Entry_type &new_entry);	//insert checks for empty entry_word and discard it automatically.

	virtual void clear();

	//Functions related to erase should return data erased.
	//Directly erase from dictionary is not allowed. User should query first to erase.
	virtual std::shared_ptr<Dictionary_type> erase(QueryResult &words);	
	virtual std::shared_ptr<Entry_type> erase_entry(QueryResult &words, Dictionary_type::size_type pos);	

	//Used for initializing Support Packs. Provide access to data.
	std::shared_ptr<Dictionary_type> dictionary_ptr();
	std::shared_ptr<Dictionary_type> dictionary_ptr() const;

public:
	virtual void operator+=(const Entry_type &entry);		//Add Entry_type
	virtual void operator+=(const Core_Dictionary &rhs) = delete;	//Combine Library

	//This two are used to deal with complicated formats, which should be handled by user.
	inline Entry_iterator begin();
	inline Entry_iterator end();
	inline const_Entry_iterator begin() const;
	inline const_Entry_iterator end() const;
	inline const_Entry_iterator cbegin() const;
	inline const_Entry_iterator cend() const;

	inline Entry_iterator lower_bound(const EntryWord_type &str);
	inline Entry_iterator upper_bound(const EntryWord_type &str);
	inline const_Entry_iterator lower_bound(const EntryWord_type &str) const;
	inline const_Entry_iterator upper_bound(const EntryWord_type &str) const;


public:
	bool query_print(const EntryWord_type &word) const;				//Marked as deprecated, return false if no entry found.

	virtual QueryResult query(const EntryWord_type &word);

#ifdef LOW_MEMORY_REQUIREMENT
	void set_dictionary_path(std::shared_ptr<std::string> _dictionary_path);
#endif // LOW_MEMORY_REQUIREMENT

};

std::ostream& operator<<(std::ostream &os, const Core_Dictionary::Entry_type &entry);

bool operator<(Core_Dictionary::Entry_iterator lhs, Core_Dictionary::Entry_iterator rhs);
bool operator<(Core_Dictionary::const_Entry_iterator lhs, Core_Dictionary::const_Entry_iterator rhs);



