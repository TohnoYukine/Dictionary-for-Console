#include "stdafx.h"
#include "DefinitionPos.h"


DefinitionPos::DefinitionPos() :
	dictionary_path(new std::string()) {}

DefinitionPos::DefinitionPos(std::shared_ptr<std::string> _dictionary_path, std::streampos _pos_beg, std::streampos _pos_end) :
	dictionary_path(_dictionary_path),
	def_beg(_pos_beg),
	def_end(_pos_end) {}

DefinitionPos::DefinitionPos(std::streampos _pos_beg, std::streampos _pos_end) :
	dictionary_path(new std::string()),
	def_beg(_pos_beg),
	def_end(_pos_end) {}

void DefinitionPos::set_dictionary_path(std::shared_ptr<std::string> _dictionary_path)
{
	dictionary_path = _dictionary_path;
}

DefinitionPos::DefinitionPos(const DefinitionPos & origin) :
	dictionary_path(origin.dictionary_path),
	def_beg(origin.def_beg),
	def_end(origin.def_end) {}

DefinitionPos::DefinitionPos(DefinitionPos && origin) noexcept :
	dictionary_path(origin.dictionary_path),
	def_beg(std::move(origin.def_beg)),
	def_end(std::move(origin.def_end))
{
	origin.dictionary_path = nullptr;
}

DefinitionPos & DefinitionPos::operator=(const DefinitionPos & origin)
{
	dictionary_path = origin.dictionary_path;
	def_beg = origin.def_beg;
	def_end = origin.def_end;
	return *this;
}

DefinitionPos & DefinitionPos::operator=(DefinitionPos && origin)
{
	dictionary_path = origin.dictionary_path;
	def_beg = std::move(origin.def_beg);
	def_end = std::move(origin.def_end);
	origin.dictionary_path = nullptr;
	return *this;
}


DefinitionPos::~DefinitionPos()
{
	dictionary_path = nullptr;
}

void DefinitionPos::print() const
{
	std::cout << *this << std::flush;
}

std::string DefinitionPos::get_raw() const
{
	std::stringstream sstrm;
	sstrm << *this;
	return sstrm.str();
}

std::ostream & operator<<(std::ostream & os, const DefinitionPos & definition)
{
	std::ifstream ifile(*definition.dictionary_path, std::ifstream::in);
	if (!ifile.is_open())
		throw std::runtime_error("Dictionary file not specified!");
	ifile.seekg(definition.def_beg);
	std::streamoff sz = definition.def_end - definition.def_beg;
	char *sink = new char[sz];
	ifile.read(sink, sz);
	ifile.close();
	os << sink << u8'\n';
	return os;
}
