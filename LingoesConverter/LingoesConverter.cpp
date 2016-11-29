// LingoesConverter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using std::string;
using std::wstring;
using std::cin;
using std::cout;
using std::endl;
using std::wcin;
using std::wcout;
using std::istream;
using std::ostream;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::wifstream;
using std::wofstream;

const string sample(R"(E:\Dictionary\Oxford Advanced Learner's Dictionary.ld2)");

struct Lingoes
{

};

int16_t get_int16(istream &is);

int main()
{
	ifstream ifile(sample);

	//[0, 4) 
	string flag_1(4, {});
	ifile.read(&flag_1[0], 4);
	cout << "Flag_1:\t\t" << flag_1 << endl;

	//[4, 20)
	int8_t MD5[16];
	ifile.read(reinterpret_cast<char*>(MD5), 16);
	cout << "MD5\t\t";
	for (int i = 0; i != 16; ++i)
		cout << static_cast<int>(MD5[i]) << " ";
	cout << endl;

	//[20, 24)
	int8_t DwordOrder[4];
	ifile.read(reinterpret_cast<char*>(DwordOrder), 4);
	cout << "DwordOrder\t";
	for (int i = 0; i != 4; ++i)
		cout << DwordOrder[i];
	cout << endl;

	//[24, 28)
	cout << "Version\t\t";
	for (int i = 0; i != 4; ++i)
		cout << ifile.get();
	cout << endl;

	//[28, 44)
	int8_t DES[16];
	ifile.read(reinterpret_cast<char*>(DES), 16);
	cout << "DES\t\t";
	for (int i = 0; i != 16; ++i)
		cout << static_cast<int>(DES[i]) << " ";
	cout << endl;

	//[44, 48)
	int8_t dic_type[4];
	ifile.read(reinterpret_cast<char*>(dic_type), 4);
	cout << "DicType\t\t";
	for (int i = 0; i != 4; ++i)
		cout << static_cast<int>(dic_type[i]);
	cout << endl;


}
