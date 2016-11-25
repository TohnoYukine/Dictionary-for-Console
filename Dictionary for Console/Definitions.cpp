#include "stdafx.h"
#include "Definitions.h"

Definitions::MemberType Definitions::DefaultType(L"Default");

Definitions::Definitions()
{
}

Definitions::Definitions(Description desc) :
	Definitions({ std::make_pair(DefaultType, desc) }) {}

Definitions::Definitions(MemberTypeList _memTypes, std::vector<Description> descvec)
{
	if (descvec.size() != _memTypes.size())
		throw std::runtime_error("MemberTypeList and Descriptions don't meet!");
	auto iter = _memTypes.begin();
	for (auto &desc : descvec)
		entry_details.insert(std::make_pair(*iter++, desc));
}

Definitions::Definitions(MemberTypeList _memTypes, std::initializer_list<Description> il) : memTypes(_memTypes)
{
	if (il.size() != _memTypes.size())
		throw std::runtime_error("MemberTypeList and Descriptions don't meet!");
	auto iter = _memTypes.begin();
	for (auto &desc : il)
		entry_details.insert(std::make_pair(*iter++, desc));
}

Definitions::Definitions(wchar_t delim, std::initializer_list<std::wstring> Type_Desc)
{
	MemberType memtype;
	Description desc;
	for (auto &rawline : Type_Desc)
	{
		std::wistringstream line(rawline);
		while (iswspace(line.peek()))	line.get();
		std::getline(line, memtype, delim);
		if (memtype.empty())
			memtype = DefaultType;
		while (iswspace(line.peek()))	line.get();
		std::getline(line, desc);
		entry_details.insert(std::make_pair(memtype, desc));
	}
}

Definitions::Definitions(std::initializer_list<value_type> formatted_pairs)
{
	for (auto &formatted_pair : formatted_pairs)
	{
		memTypes.push_back(formatted_pair.first);
		entry_details.insert(formatted_pair);
	}
}


Definitions::~Definitions()
{
}
