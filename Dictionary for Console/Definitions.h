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
	using MemberType = std::wstring;
	using Description = std::wstring;
	using MemberTypeList = std::vector<MemberType>;

	using key_type = MemberType;
	using mapped_type = Description;
	using value_type = std::pair<MemberType, Description>;

private:
	static MemberType DefaultType;

private:
	MemberTypeList memTypes;
	std::multimap<MemberType, Description> entry_details;

public:
	Definitions();
	//Used for simple Definitions
	Definitions(Description desc);
	//Given MemberTypeList, initialize with a series of descriptions. 
	//Descriptions must match MemberTypeList in number!
	explicit Definitions(MemberTypeList _memTypes, std::vector<Description> descvec);
	explicit Definitions(MemberTypeList _memTypes, std::initializer_list<Description> il);
	//Initialize with a series of MemberType-Description pair. Delimiter must be specified.
	explicit Definitions(wchar_t delim, std::initializer_list<std::wstring> Type_Desc);
	explicit Definitions(std::initializer_list<value_type> formatted_pairs);

	~Definitions();


	//Description &operator[];
	
};

