#include "stdafx.h"
#include "Definitions.h"

Definitions::Attribute_type Definitions::DefaultAttribute(u8"");
Definitions::Description_type Definitions::DefaultDescription;

Definitions::Definitions() {}

Definitions::Definitions(Description_type _description) :
	Definitions({ std::make_pair(DefaultAttribute, _description) }) {}

Definitions::Definitions(Description_type && _description) noexcept :
	Definitions({ std::make_pair(DefaultAttribute, std::move(_description)) }) {}

Definitions::Definitions(std::initializer_list<value_type> formatted_pairs)
{
	for (auto &formatted_pair : formatted_pairs)
		entry_details.push_back(formatted_pair);
}


Definitions::Definitions(const std::vector<value_type> &formatted_pairs)
{
	for (const auto &formatted_pair : formatted_pairs)
		entry_details.push_back(formatted_pair);
}

Definitions::Definitions(std::vector<value_type>&& formatted_pairs) noexcept :
	entry_details(std::move(formatted_pairs)) {}

Definitions::Definitions(const std::vector<Description_type> &_descriptions)
{
	for (const auto &description : _descriptions)
		entry_details.emplace_back(DefaultAttribute, description);
}


Definitions::Definitions(const Definitions & origin)
{
	for (const auto &formatted_pair : origin)
		entry_details.push_back(formatted_pair);
}

Definitions::Definitions(Definitions && origin) noexcept :
	entry_details(std::move(origin.entry_details)) {}

Definitions::~Definitions() {}

Definitions::iterator Definitions::begin()
{
	return entry_details.begin();
}

Definitions::iterator Definitions::end()
{
	return entry_details.end();
}

Definitions::const_iterator Definitions::begin() const
{
	return cbegin();
}

Definitions::const_iterator Definitions::end() const
{
	return cend();
}

Definitions::const_iterator Definitions::cbegin() const
{
	return entry_details.cbegin();
}

Definitions::const_iterator Definitions::cend() const
{
	return entry_details.cend();
}

std::ostream & operator<<(std::ostream & os, const Definitions & definition)
{
	for (const auto &entry : definition.entry_details)
		os << entry.second << u8"\n";
	return os;
}
