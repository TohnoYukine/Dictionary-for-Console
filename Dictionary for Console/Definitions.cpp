#include "stdafx.h"
#include "Definitions.h"

Definitions::Attribute Definitions::DefaultAttribute(u8"Default");
Definitions::Description Definitions::DefaultDescription;

Definitions::Definitions()
{
}

Definitions::Definitions(Description _description) :
	Definitions({ std::make_pair(DefaultAttribute, _description) }) {}

Definitions::Definitions(AttributeList _attibutes, std::vector<Description> _descriptions)
{
	if (_descriptions.size() != _attibutes.size())
		throw std::runtime_error("AttributeList and Descriptions don't meet!");
	auto iter = _attibutes.begin();
	for (auto &description : _descriptions)
		entry_details.insert(std::make_pair(*iter++, description));
}

Definitions::Definitions(AttributeList _attibutes, std::initializer_list<Description> _descriptions) : attributes(_attibutes)
{
	if (_descriptions.size() != _attibutes.size())
		throw std::runtime_error("AttributeList and Descriptions don't meet!");
	auto iter = _attibutes.begin();
	for (auto &description : _descriptions)
		entry_details.insert(std::make_pair(*iter++, description));
}

Definitions::Definitions(char delim, std::initializer_list<Attribute> _attibutes)
{
	Attribute attr;
	Description desc;
	for (auto &rawline : _attibutes)
	{
		std::istringstream line(rawline);
		while (isspace(line.peek()))	line.get();
		std::getline(line, attr, delim);
		if (attr.empty())
			attr = DefaultAttribute;
		while (isspace(line.peek()))	line.get();
		std::getline(line, desc);
		entry_details.insert(std::make_pair(attr, desc));
	}
}

Definitions::Definitions(std::initializer_list<value_type> formatted_pairs)
{
	for (auto &formatted_pair : formatted_pairs)
	{
		attributes.push_back(formatted_pair.first);
		entry_details.insert(formatted_pair);
	}
}


Definitions::~Definitions()
{
}

Definitions::iterator Definitions::begin()
{
	return entry_details.begin();
}

Definitions::iterator Definitions::end()
{
	return entry_details.end();
}

Definitions::const_iterator Definitions::cbegin() const
{
	return entry_details.cbegin();
}

Definitions::const_iterator Definitions::cend() const
{
	return entry_details.cend();
}
