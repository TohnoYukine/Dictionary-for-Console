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

#include "Dictionary.h"

class Dictionary;
class QueryResult
{
public:
	using Dictionary_type = Dictionary::Dictionary_type;
	using result_type = std::vector<Dictionary::Entry_iterator>;
	using iterator = result_type::iterator;
	using const_iterator = result_type::const_iterator;		//It needs further confirmation whether const_iterator can modify dictionary.
	using size_type = Dictionary_type::size_type;
	using value_type = Dictionary::Entry_iterator;

private:
	std::shared_ptr<Dictionary_type> dictionary;	//Shares data with Dictionary. dictionary will be accessible till all reference destroyed.
	std::shared_ptr<result_type> result;			//Why using vector? multimap<T>::iterator is bidirectional iterator and has no operator<
	std::string word;						

public:
	QueryResult(std::string _word,
				std::shared_ptr<Dictionary_type> _ptr_dictionary,
				std::shared_ptr<result_type> _result);
	QueryResult(const QueryResult &origin);
	~QueryResult();

	iterator begin();
	iterator end();
	const_iterator cbegin() const;
	const_iterator cend() const;

	size_type size() const;
	iterator erase(iterator &iter);					//erase from QueryResult, but not from dictionary.
	iterator erase(size_type pos);
	void clear();

	value_type &operator[](size_type pos);
	value_type &operator[](size_type pos) const;
	value_type &operator*();
	value_type &operator*() const;
	
	//Dictionary::Entry_iterator operator++();
	//Elements in result are not guaranteed to be continuous in dictionary, which should not depend on when query.

private:

public:
	void print() const;

	//Temporaries used for debug.
	void print_raw() const;			
	std::string get_raw();			//Only the first result is returned.

	//Empty result, but not affect dictionary
	bool empty();
};

