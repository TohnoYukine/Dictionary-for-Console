#pragma once

#include <string>
#include <vector>
#include <map>
#include <initializer_list>
#include <utility>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <stdexcept>
#include <memory>
#include <set>


class DefinitionPos
{
	friend std::ostream &operator<<(std::ostream &os, const DefinitionPos &definition);

	std::shared_ptr<std::string> dictionary_path;
	std::streampos def_beg;
	std::streampos def_end;

public:

private:

public:
	//For entry with no DefinitionPos
	DefinitionPos();
	//Used for simple DefinitionPos
	explicit DefinitionPos(std::shared_ptr<std::string> _dictionary_path, std::streampos _pos_beg, std::streampos _pos_end);
	explicit DefinitionPos(std::streampos _pos_beg, std::streampos _pos_end);

	void set_dictionary_path(std::shared_ptr<std::string> _dictionary_path);

	DefinitionPos(const DefinitionPos &origin);
	DefinitionPos(DefinitionPos &&origin) noexcept;

	DefinitionPos &operator=(const DefinitionPos &origin);
	DefinitionPos &operator=(DefinitionPos &&origin);

	~DefinitionPos();

	std::string get_raw() const;
	void print() const;
	//Description &operator[];

};

std::ostream &operator<<(std::ostream &os, const DefinitionPos &definition);


