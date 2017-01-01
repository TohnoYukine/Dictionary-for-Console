#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include <map>
#include <vector>
#include <deque>
#include <list>
#include <iterator>
#include <string>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <set>
#include <cctype>

#include "Core_Dictionary.h"

class Core_Dictionary;
class QueryResult
{
	friend std::ostream &operator<<(std::ostream &os, QueryResult &results);
	friend QueryResult operator|(const QueryResult &lhs, const QueryResult &rhs);
	friend QueryResult operator&(const QueryResult &lhs, const QueryResult &rhs);


public:
	using Entry_type = Core_Dictionary::Entry_type;
	using Dictionary_type = Core_Dictionary::Dictionary_type;
	using result_type = std::deque<Core_Dictionary::Entry_iterator>;
	using iterator = result_type::iterator;
	using const_iterator = result_type::const_iterator;		//It needs further confirmation whether const_iterator can modify dictionary.
	using size_type = result_type::size_type;
	using value_type = Core_Dictionary::Entry_iterator;

private:
	std::shared_ptr<Dictionary_type> dictionary;	//Shares data with Core_Dictionary. dictionary will be accessible till all reference destroyed.
	std::shared_ptr<result_type> result;			//Why using vector? multimap<T>::iterator is bidirectional iterator and has no operator<
	std::string word;						

public:
	QueryResult(std::string _word,
				std::shared_ptr<Dictionary_type> _ptr_dictionary,
				std::shared_ptr<result_type> _result);
	QueryResult(const QueryResult &origin);
	~QueryResult();

	inline iterator begin();
	inline iterator end();
	inline const_iterator cbegin() const;
	inline const_iterator cend() const;

	inline size_type size() const;
	inline bool empty() const;
	iterator erase(iterator &iter);					//erase from QueryResult, but not from dictionary.
	iterator erase(size_type pos);
	iterator erase(iterator &first, iterator &last);
	void clear();

	Entry_type operator[](size_type pos) const;

	std::string queried_word() const;
	
	//Elements in result are not guaranteed to be continuous in dictionary, which should not be depended on when query.

	Core_Dictionary::Entry_iterator pop_front();
private:

public:
	void print() const;

	//Temporaries used for debug.
	void print_raw() const;			
	std::string get_raw();			//Only the first result is returned.

	QueryResult &operator|=(const QueryResult &rhs);
	QueryResult &operator&=(const QueryResult &rhs);

};

//When treated as stream, only 
std::ostream &operator<<(std::ostream &os, QueryResult &results);
QueryResult operator|(const QueryResult &lhs, const QueryResult &rhs);
QueryResult operator&(const QueryResult &lhs, const QueryResult &rhs);
