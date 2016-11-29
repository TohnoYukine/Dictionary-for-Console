#pragma once

#include <string>
#include <vector>
#include <map>
#include <initializer_list>
#include <utility>
#include <algorithm>
#include <sstream>
#include <cctype>

class Definitions
{
	using Attribute = std::wstring;
	using Description = std::wstring;
	using AttributeList = std::vector<Attribute>;

	using key_type = Attribute;
	using mapped_type = Description;
	using value_type = std::pair<Attribute, Description>;
	using iterator = std::multimap<Attribute, Description>::iterator;
	using const_iterator = std::multimap<Attribute, Description>::const_iterator;

public:
	static Attribute DefaultAttribute;
	static Description DefaultDescription;

private:
	AttributeList attributes;
	std::multimap<Attribute, Description> entry_details;

public:
	//For entry with no definitions
	Definitions();
	//Used for simple definitions
	Definitions(Description _description);
	//Given AttributeList, initialize with a series of descriptions. 
	//Descriptions must match AttributeList in number!
	explicit Definitions(AttributeList _attibutes, std::vector<Description> _descriptions);
	explicit Definitions(AttributeList _attibutes, std::initializer_list<Description> _descriptions);
	explicit Definitions(std::initializer_list<value_type> formatted_pairs);
	//Initialize with a series of Attribute-Description pair. Delimiter must be specified.
	explicit Definitions(wchar_t delim, std::initializer_list<Attribute> _attibutes);

	~Definitions();

	iterator begin();
	iterator end();
	const_iterator cbegin() const;
	const_iterator cend() const;

	//Description &operator[];
	
};

