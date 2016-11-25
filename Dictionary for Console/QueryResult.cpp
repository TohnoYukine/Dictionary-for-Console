#include "stdafx.h"
#include "QueryResult.h"


QueryResult::QueryResult(std::wstring _word,
	std::shared_ptr<std::multimap<EntryWord, Definitions>> _ptr_dictionary,
	std::shared_ptr<std::vector<std::multimap<EntryWord, Definitions>::iterator>> _result) :
	word(_word), dictionary(_ptr_dictionary), result(_result) {}

QueryResult::~QueryResult()
{
}

void QueryResult::print()
{
	std::wcout << word << "\t" << result->size() << "\t entries found." << std::endl;
//	for (auto &iter : *result)
//		std::wcout << iter->second << std::endl;
}

bool QueryResult::empty()
{
	if (result->size() == 0)
		return true;
	else
		return false;
}
