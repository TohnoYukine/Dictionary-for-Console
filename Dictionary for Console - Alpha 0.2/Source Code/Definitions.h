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
	friend std::ostream &operator<<(std::ostream &os, const Definitions &definition);

	using Attribute_type = std::string;
	using Description_type = std::string;

	using key_type = Attribute_type;
	using mapped_type = Description_type;
	using value_type = std::pair<Attribute_type, Description_type>;
	using container_type = std::vector<value_type>;
	using iterator = container_type::iterator;
	using const_iterator = container_type::const_iterator;

public:
	static Attribute_type DefaultAttribute;
	static Description_type DefaultDescription;

private:
	container_type entry_details;	//Attribute is used to mark the Description, default constructed is DefaultAttribute.

public:
	//For entry with no definitions
	Definitions();
	//Used for simple definitions
	Definitions(Description_type _description);
	Definitions(Description_type &&_description) noexcept;
	//Given AttributeList, initialize with a series of descriptions. 
	explicit Definitions(std::initializer_list<value_type> formatted_pairs);

	//For temporary use
	explicit Definitions(const std::vector<value_type> &formatted_pairs);
	explicit Definitions(std::vector<value_type> &&formatted_pairs) noexcept;

	explicit Definitions(const std::vector<Description_type> &_descriptions);

	Definitions(const Definitions &origin);
	Definitions(Definitions &&origin) noexcept;

	~Definitions();

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;

	//Description &operator[];
	
};

std::ostream &operator<<(std::ostream &os, const Definitions &definition);