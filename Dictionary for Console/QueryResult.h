#pragma once
#include "Dictionary.h"

class QueryResult
{
	using EntryWord = std::wstring;
	using Definition = std::wstring;
	using Entry = std::pair<EntryWord, Definitions>;

private:
	std::shared_ptr<std::multimap<EntryWord, Definitions>> dictionary;
	std::shared_ptr<std::vector<std::multimap<EntryWord, Definitions>::iterator>> result;
	std::wstring word;

public:
	QueryResult(std::wstring _word,
				std::shared_ptr<std::multimap<EntryWord, Definitions>> _ptr_dictionary,
				std::shared_ptr<std::vector<std::multimap<EntryWord, Definitions>::iterator>> _result);
	~QueryResult();

public:
	void print();
	bool empty();
};

