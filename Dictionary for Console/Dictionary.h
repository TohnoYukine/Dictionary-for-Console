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

#define ASSIGN_NOT_ALLOWED	
#define FUTURE_FEATURES_UNIMPLEMENTED

class QueryResult;								//Allows user to query as well as add entries
class WildcardStringSupport;
class Dictionary
{
	friend class QueryResult;
	friend class WildcardStringSupport;

public:
	using EntryWord = std::string;
	using Entry_type = std::pair<EntryWord, Definitions>;
	using Entry_iterator = std::multimap<EntryWord, Definitions>::iterator;
	using Dictionary_type = std::multimap<EntryWord, Definitions>;

	static std::string default_category;
	static EntryWord InvalidWord;
	static Definitions InvalidDefinition;
	static Entry_type InvalidEntry;

	enum DataMode { RawAndDictionary = 0, RawDataOnly = 1, DictionaryOnly = 2 };

	enum DictionaryState { Unsorted = 0, Sorted = 1 };
	enum RawDataState { Incomplete = 0, Complete = 1 };
	enum SynchronizationState { Asynchronous = 0, Synchronous = 1 };


//Safety Policies. This is quite complicated, so I have not yet decided what exactly each state work.
/*	State		is_sorted		is_complete		is_synchronized		Possible Cases						
*																							
*	0			Unsorted		Incomplete		Asynchronous		Not Allowed
*	1			Unsorted		Incomplete		Synchronous			Not Allowed
*	2			Unsorted		Complete		Asynchronous		push_back()/emplace_back()/Initialized		
*	3			Unsorted		Complete		Synchronous			Not Allowed
*	4			Sorted			Incomplete		Asynchronous		clear_raw_data()				
*	5			Sorted			Incomplete		Synchronous			Not Allowed
*	6			Sorted			Complete		Asynchronous		Not Allowed
*	7			Sorted			Complete		Synchronous			Working/Default Initialized						
*/

private:
	DataMode data_mode;									//Indicate how user adds data.
														//RawDataOnly is quick, but needs sort later, which should be used to temporarily storing entries.
														//DictionaryOnly is useful when dictionary is loaded directly. Default data_mode.

	DictionaryState is_sorted;							//Indicate whether dictionary is built. When clear_dictionary() called, set to Unsorted if not empty.
	RawDataState is_complete;							//Indicate whether temp_entries is a complete copy of all entries. Default is Complete. When clear_raw_data() called, set to Incomplete.
	SynchronizationState is_synchronized;				//Indicate whether dictionary and temp_entries is synchronized. Marked for deprecation.

	std::string category;								//User may use different dictionaries. Used to check whether Dictionary can be combined.
	std::vector<Entry_type> temp_entries;				//Used to temporarily store data, including invalid entries, which are excluded in sort().
	std::shared_ptr<Dictionary_type> dictionary;
	std::shared_ptr<WildcardStringSupport> wildcard_string_mapping;

private:
//Assisting member functions used to do actual work.
	inline void get_data(std::istream &is, char delim1 = u8'\0', char delim2 = u8'\n');	//delim2 separate entries, delim1 separate entry word and definition
	inline Entry_type string_to_entry(std::string s, char delim);
	inline size_t check_working_state();

public:
//Copy Control
	explicit Dictionary(DataMode _mode = DictionaryOnly);

	//delim separates entries
	//delim2 separate entries, delim1 separate entry word and definition
	explicit Dictionary(std::istream &is, DataMode _mode = DictionaryOnly);
	explicit Dictionary(std::istream &is, char delim, DataMode _mode = DictionaryOnly);
	explicit Dictionary(std::istream &is, char delim1, char delim2, DataMode _mode = DictionaryOnly);
	explicit Dictionary(std::istream &is, std::string _category, DataMode _mode = DictionaryOnly);
	explicit Dictionary(std::istream &is, std::string _category, char delim, DataMode _mode = DictionaryOnly);
	explicit Dictionary(std::istream &is, std::string _category, char delim1, char delim2, DataMode _mode = DictionaryOnly);

	explicit Dictionary(std::ifstream &ifile, DataMode _mode = DictionaryOnly);
	explicit Dictionary(std::ifstream &ifile, char delim, DataMode _mode = DictionaryOnly);
	explicit Dictionary(std::ifstream &ifile, char delim1, char delim2, DataMode _mode = DictionaryOnly);
	explicit Dictionary(std::ifstream &ifile, std::string _category, DataMode _mode = DictionaryOnly);
	explicit Dictionary(std::ifstream &ifile, std::string _category, char delim, DataMode _mode = DictionaryOnly);
	explicit Dictionary(std::ifstream &ifile, std::string _category, char delim1, char delim2, DataMode _mode = DictionaryOnly);

	explicit Dictionary(std::initializer_list<Entry_type> entries, DataMode _mode = DictionaryOnly);


	explicit Dictionary(const Dictionary &);	//Implicitly call copy constructor is too expensive, users should know what they do.					

//Copy Assignment
	Dictionary &operator=(Dictionary) = delete;	

	Dictionary &swap(Dictionary &);
	~Dictionary();

public:
//STL-like functions

//Safety Policies
/*	Operation			Requirement->StateChange	true/false/unrated	true/false/unchanged/?
*						is_sorted				is_complete				is_synchronized
*	emplace_back		unrated	->	false		unrated	->	unchanged	unrated	->	false
*	push_back			unrated	->	false		unrated	->	unchanged	unrated	->	false
*	emplace				true	->	unchanged	unrated	->	unchanged	unrated	->	unchanged
*	insert				true	->	unchanged	unrated	->	unchanged	unrated	->	unchanged

*	sort				unrated	->	true		unrated	->	unchanged	unrated	->	unchanged
*	query				true	->	unchanged	unrated	->	unchanged	unrated	->	unchanged

*	erase				true	->	unchanged	unrated	->	unchanged	unrated	->	false

*	clear_raw_data		unrated	->	unchanged	unrated	->	false		unrated	->	?
*	clear_dictionary	unrated	->	?			unrated	->	unchanged	unrated	->	?
*	clear_all			unrated	->	true		unrated	->	true		unrated	->	true
*
*/


	//With _back, class works like a sequential container.
	//Following functions are used to fast build up dictionary in memory.
	void emplace_back(const std::string &str, char delim);	//Add lots of entries to temp_entries, set is_sorted as Unsorted.
	void emplace_back(const EntryWord &_entryword, const Definitions &_definitions);
	void push_back(const Entry_type &new_entry);

	//Without _back, class works like an associative container plus a sequential container.
	//Following functions are used to add new entries to a sorted dictionary.
	//DataMode defaultly set as DictionaryOnly. When adding a backup at temp_entries, specify RawAndDictionary.
	void emplace(const std::string &, char delim, DataMode _mode = DictionaryOnly);
	void emplace(const EntryWord &_entryword, const Definitions &_definitions, DataMode _mode = DictionaryOnly);
	void insert(const Entry_type &new_entry, DataMode _mode = DictionaryOnly);

	void sort();

	void clear_raw_data();		//clear_raw_data alone will not release memory! Used to free memory with shrink_to_fit. Unintended usage without sort() cause runtime_error.
	void shrink_to_fit();		//shrink_to_fit alone will not clear temp_entries!
	void clear_dictionary();	//clear_dictionary will clear dictionary, but will not clear temp_entries.
	void clear_all();

	//Functions related to erase should return data erased.
	//Directly erase from dictionary is not allowed. User should query first to erase.
	std::shared_ptr<Dictionary_type> erase(QueryResult &words);	//erase from dictionary, but not from temp_entries.
	std::shared_ptr<Entry_type> erase_entry(QueryResult &words, Dictionary_type::size_type pos);

//User friendly functions for noobs. To be honest, the STL functions can substitute them.
	void add_entry(const std::string &str, char delim);
	void add_entry(const EntryWord &_entryword, const Definitions &_definitions);
	



public:
//Future features
	void operator+=(const Entry_type &);		//Add Entry_type
	void operator+=(const Dictionary &);		//Combine Library

	void operator<<(const Entry_type &);		//Stream operators, I cannot code for them now
	void operator>>(const Entry_type &);		//Parameter type may need modify

	//This two are used to deal with complicated formats, which should be handled by user. sort() is needed.
	Entry_iterator lower_bound(const std::string &);
	Entry_iterator upper_bound(const std::string &);


public:
	void set_category(std::string _category);

	void enable_wildcard();

	bool query_print(std::string);				//Marked as deprecated, return false if no entry found.

	//wildcard is considered as a normal character in word.
	QueryResult query(std::string word);
	//wildcard 
	QueryResult query_wildcard(std::string wdc_word);
};

