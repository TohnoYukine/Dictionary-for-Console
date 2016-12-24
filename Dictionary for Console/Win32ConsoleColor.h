#pragma once

#include <Windows.h>
#include <iostream>

//Win32ConsoleColor object are not meant to be stored. They are used in a statement that ends with a semicolon.
//This class depends on evaluation order. It is for temporary use.
class Win32ConsoleColor
{
	friend std::ostream &operator<<(std::ostream &os, Win32ConsoleColor &&color);

	static HANDLE console;
	static PCONSOLE_SCREEN_BUFFER_INFO original_screen_buffer_info;
	static size_t initialization_state;

	WORD character_attributes;

public:
	static bool Initialize_Win32ConsoleColor();
	Win32ConsoleColor() = delete;
	explicit Win32ConsoleColor(const WORD &wAttr);
	~Win32ConsoleColor();
};

std::ostream &operator<<(std::ostream &os, Win32ConsoleColor &&color);
