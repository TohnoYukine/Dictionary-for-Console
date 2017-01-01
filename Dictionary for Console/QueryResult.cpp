#include "stdafx.h"
#include "QueryResult.h"


QueryResult::QueryResult(std::string _word,
	std::shared_ptr<Core_Dictionary::Dictionary_type> _ptr_dictionary,
	std::shared_ptr<result_type> _result) :
	word(_word), dictionary(_ptr_dictionary), result(_result) {}

QueryResult::QueryResult(const QueryResult & origin) :
	word(origin.word), dictionary(origin.dictionary), result(new result_type(*origin.result)) {}

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

QueryResult::Entry_type QueryResult::operator[](size_type pos) const
{
	if (pos >= result->size())
		throw std::out_of_range("Subscript out of range!");
	return Entry_type(*result->operator[](pos));
}

QueryResult::size_type QueryResult::size() const
{
	return result->size();
}

bool QueryResult::empty() const
{
	return result->empty();
}

void QueryResult::clear()
{
	result->clear();
}

QueryResult::iterator QueryResult::erase(iterator &iter)
{
	return result->erase(iter);;
}

QueryResult::iterator QueryResult::erase(size_type pos)
{
	iterator iter = result->erase(begin() + pos);
	return iter;
}

QueryResult::iterator QueryResult::erase(iterator & first, iterator & last)
{
	return result->erase(first, last);
}

void QueryResult::print() const
{
	std::cout << word << "\t" << result->size() << (result->size() <= 1 ? " entry found." : " entries found.") << std::endl;
	//This part is not completed until operator<< is overloaded for Definitions 
	for (auto &iter : *result)
	{
		std::cout << iter->first << std::endl;
		iter->second.print();
	}
}

void QueryResult::print_raw() const
{
	QueryResult temp(*this);
	while (!empty())
		std::cout << temp << std::endl;
}

std::string QueryResult::get_raw()
{
	std::string ret;
	for (auto entry_iter = cbegin(); entry_iter != cend(); ++entry_iter)
	{
		ret += (*entry_iter)->first;
		ret += u8'\t';
		ret += (*entry_iter)->second.get_raw();
		ret += u8'\n';
	}
	return ret;
}

QueryResult &QueryResult::operator|=(const QueryResult & rhs)
{
	if (dictionary != rhs.dictionary)
		throw std::runtime_error("QueryResult operator|= operands don't match!");

	word += u8"|";
	word += rhs.word;

	std::copy(rhs.cbegin(), rhs.cend(), std::back_inserter(*result));
	std::sort(begin(), end());
	QueryResult::result_type::iterator end_iter = std::unique(begin(), end());
	erase(end_iter, end());

	result->shrink_to_fit();

	return *this;
}

QueryResult & QueryResult::operator&=(const QueryResult & rhs)
{
	if (dictionary != rhs.dictionary)
		throw std::runtime_error("QueryResult operator|= operands don't match!");

	word += u8"&";
	word += rhs.word;

	QueryResult::result_type lhs_cpy(size());
	QueryResult::result_type rhs_cpy(rhs.size());
	std::partial_sort_copy(cbegin(), cend(), lhs_cpy.begin(), lhs_cpy.end());
	std::partial_sort_copy(rhs.cbegin(), rhs.cend(), rhs_cpy.begin(), rhs_cpy.end());

	QueryResult::result_type ret_result;
	result->clear();
	std::set_intersection(lhs_cpy.cbegin(), lhs_cpy.cend(), rhs_cpy.cbegin(), rhs_cpy.cend(), std::back_inserter(ret_result));
	std::copy(ret_result.cbegin(), ret_result.cend(), std::back_inserter(*result));

	result->shrink_to_fit();

	return *this;
}

std::string QueryResult::queried_word() const
{
	return word;
}

Core_Dictionary::Entry_iterator QueryResult::pop_front()
{
	Core_Dictionary::Entry_iterator ret(result->front());
	result->pop_front();
	return ret;
}

std::ostream & operator<<(std::ostream & os, QueryResult & results)
{
	QueryResult::value_type result(results.pop_front());
	os << result->first << u8'\n';	//Print the word.
	os << result->second;			//Print the Definitions of the word.
	return os;
}

QueryResult operator|(const QueryResult & lhs, const QueryResult & rhs)
{
	if (lhs.dictionary != rhs.dictionary)
		throw std::runtime_error("QueryResult operator| operands don't match!");

	QueryResult ret_result(lhs);
	ret_result |= rhs;

	return ret_result;
}

QueryResult operator&(const QueryResult & lhs, const QueryResult & rhs)
{
	if (lhs.dictionary != rhs.dictionary)
		throw std::runtime_error("QueryResult operator& operands don't match!");

	QueryResult ret_result(lhs);
	ret_result &= rhs;

	return ret_result;
}
