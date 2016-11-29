#include "stdafx.h"
#include "QueryResult.h"


QueryResult::QueryResult(std::wstring _word,
	std::shared_ptr<Dictionary::Dictionary_type> _ptr_dictionary,
	std::shared_ptr<result_type> _result) :
	word(_word), dictionary(_ptr_dictionary), result(_result) {}

QueryResult::QueryResult(const QueryResult & origin) :
	word(origin.word), dictionary(origin.dictionary), result(origin.result) {}

QueryResult::~QueryResult()
{
}

QueryResult::iterator QueryResult::begin()
{
	return result->begin();
}

QueryResult::iterator QueryResult::end()
{
	return result->end();
}

QueryResult::const_iterator QueryResult::cbegin() const
{
	return result->cbegin();
}

QueryResult::const_iterator QueryResult::cend() const
{
	return result->cend();
}

QueryResult::value_type & QueryResult::operator[](size_type pos)
{
	if (pos >= result->size())
		throw std::out_of_range("Subscript out of range!");
	return result->at(pos);
}

QueryResult::value_type & QueryResult::operator[](size_type pos) const
{
	if (pos >= result->size())
		throw std::out_of_range("Subscript out of range!");
	return result->at(pos);
}

QueryResult::size_type QueryResult::size() const
{
	return result->size();
}

void QueryResult::clear()
{
	result->clear();
}

QueryResult::iterator QueryResult::erase(iterator &iter)
{
	iter = result->erase(iter);
	return iter;
}

QueryResult::iterator QueryResult::erase(size_type pos)
{
	iterator iter = result->erase(begin() + pos);
	return iter;
}

void QueryResult::print() const
{
	std::wcout << word << "\t" << result->size() << (result->size() <= 1 ? " entry found." : " entries found.") << std::endl;
	//This part is not completed until operator<< is overloaded for Definitions 
	for (auto &iter : *result)
	{
		std::wcout << iter->first << std::endl;
		for (auto &details : iter->second)
			std::wcout << details.first << L":\t" << details.second << std::endl;
	}
}

void QueryResult::print_raw() const
{
	for (auto &iter : *result)
	{
		std::wcout << iter->first << std::endl;
		for (auto &details : iter->second)
			std::wcout << details.first << L":\t" << details.second << std::endl;
	}
}

std::wstring QueryResult::get_raw()
{
	return std::wstring((*begin())->second.begin()->second);
}

bool QueryResult::empty()
{
	return result->empty();
}
