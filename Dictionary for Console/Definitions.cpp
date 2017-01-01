#include "stdafx.h"
#include "Definitions.h"

Definitions::Attribute_type Definitions::DefaultAttribute(u8"");
Definitions::Description_type Definitions::DefaultDescription;

Definitions::Definitions() {}

Definitions::Definitions(const Description_type &_description) :
	Definitions({ std::make_pair(DefaultAttribute, _description) }) {}

Definitions::Definitions(Description_type && _description) noexcept :
	Definitions({ std::make_pair(DefaultAttribute, std::move(_description)) }) {}

Definitions::Definitions(std::initializer_list<value_type> formatted_pairs)
{
	for (auto &formatted_pair : formatted_pairs)
		entry_details.push_back(formatted_pair);
}

Definitions::Definitions(const std::string & str, char delim)
{
	std::string::size_type pos1 = str.find_first_not_of(u8" \t\n");
	std::string::size_type pos2 = str.find(delim, pos1);
	while (pos1 != std::string::npos)
	{
		if (pos2 == pos1)
			continue;
		entry_details.emplace_back(DefaultAttribute, str.substr(pos1, pos2));
		if (pos2 == std::string::npos || pos2 == str.size() - 1)
			break;
		pos1 = str.find_first_not_of(u8" \t\n", pos2 + 1);
		pos2 = str.find(delim, pos1);
	}
}

Definitions::Definitions(std::istream & is, char delim1, char delim2)
{
	std::string line;
	std::string str;
	std::getline(is, str, delim2);

	std::string::size_type pos1 = str.find_first_not_of(u8" \t\n");
	std::string::size_type pos2 = str.find(delim1, pos1);
	while (pos1 != std::string::npos)
	{
		if (pos2 == pos1)
			continue;
		entry_details.emplace_back(DefaultAttribute, str.substr(pos1, pos2));
		if (pos2 == std::string::npos || pos2 == str.size() - 1)
			break;
		pos1 = str.find_first_not_of(u8" \t\n", pos2 + 1);
		pos2 = str.find(delim1, pos1);
	}
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

Definitions & Definitions::operator=(const Definitions & origin)
{
	entry_details.clear();
	for (const auto &formatted_pair : origin.entry_details)
		entry_details.push_back(formatted_pair);
	return *this;
}

Definitions & Definitions::operator=(Definitions && origin)
{
	entry_details.clear();
	entry_details = std::move(origin.entry_details);
	return *this;
}

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

std::string Definitions::get_raw() const
{
	std::string ret;
	for (const auto &details : *this)
	{
		ret += details.second;
		ret += u8'\t';
	}
	ret.pop_back();
	return ret;
}

void Definitions::print() const
{
	for (const auto &details : *this)
		std::cout << details.second << std::endl;
}

std::ostream & operator<<(std::ostream & os, const Definitions & definition)
{
	for (const auto &entry : definition.entry_details)
		os << entry.second << u8"\n";
	return os;
}
