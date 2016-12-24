#include "stdafx.h"
#include "Win32ConsoleColor.h"

HANDLE Win32ConsoleColor::console = GetStdHandle(STD_OUTPUT_HANDLE);
PCONSOLE_SCREEN_BUFFER_INFO Win32ConsoleColor::original_screen_buffer_info = new _CONSOLE_SCREEN_BUFFER_INFO;
size_t Win32ConsoleColor::initialization_state = 0;

bool Win32ConsoleColor::Initialize_Win32ConsoleColor()
{
	bool ret = GetConsoleScreenBufferInfo(console, original_screen_buffer_info);
	++initialization_state;
	return ret;
}

Win32ConsoleColor::Win32ConsoleColor(const WORD & wAttr)
{
	if (!initialization_state)
		throw std::runtime_error("Uninitialized Win32ConsoleColor class not allowed!");
	character_attributes = wAttr;
}

Win32ConsoleColor::~Win32ConsoleColor()
{
	SetConsoleTextAttribute(Win32ConsoleColor::console, original_screen_buffer_info->wAttributes);
}

std::ostream & operator<<(std::ostream & os, Win32ConsoleColor &&color)
{
	SetConsoleTextAttribute(Win32ConsoleColor::console, color.character_attributes);
	return os;
}
