#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <conio.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#ifdef _WIN32
	#include <Windows.h>
	// Shell used to retrieve known folder paths (AppData, UserDirectory etc.).
	#include <ShlObj.h>
#else
	// Provides access to the POSIX operating system API. (gethostname() etc.)
	// Defines  miscellaneous  symbolic constants and types, and declares miscellaneous functions.
	#include <unistd.h>
	// Used to retrieve environment variables.
	#include <stdlib.h>
#endif

// Enum representing a color.
enum class Color
{
#ifdef _WIN32
	// Windows: Values matching the windows terminal codes.
	Black = 0,
	DarkBlue = 1,
	DarkGreen = 2,
	DarkCyan = 3,
	DarkRed = 4,
	DarkMagenta = 5,
	DarkYellow = 6,
	LightGray = 7,
	Gray = 8,
	Blue = 9,
	Green = 10,
	Cyan = 11,
	Red = 12,
	Magenta = 13,
	Yellow = 14,
	White = 15,
#else
	// Linux: Values 0 - 7 matching ANSI escape codes.
	// To get the bright variant, one has to use the same escape codes
	// with an additional parameter.
	Black = 0,
	DarkRed = 1,
	DarkGreen = 2,
	DarkYellow = 3,
	DarkBlue = 4,
	DarkMagenta = 5,
	DarkCyan = 6,
	LightGray = 7,
	Gray = 8,
	Red = 9,
	Green = 10,
	Yellow = 11,
	Blue = 12,
	Magenta = 13,
	Cyan = 14,
	White = 15,
#endif

	Info = DarkYellow,
	Error = DarkRed,
	Input = White,
	Output = LightGray
};

// Function prototypes.
void	ChangeColor(Color color);
void	PrintError(std::string error);
void	Wait();
void	PrepareConsole(std::string ProgramName, std::string Version, std::string Description = "");
void	PrintText(std::string Text, int Pause);
bool	CopyDirectory(std::string SourcePath, std::string TargetPath, bool CopySubdirectories = true);
int		CountWords(std::string Text, bool RespectInterpunctation = true);
std::string	GetAppDataDirectory();
std::string	ToLower(std::string Text);
std::string	FirstToUpper(std::string Text);
std::string	GetWord(std::string Text, int Index);
std::string	IntToHex(int Number);
std::string	ReverseString(std::string Text);
std::string	Escape(std::string Text);
std::string	GetComputerName();
std::string	GetUserName();
std::string	GetFileFormat(std::string FileName);
std::string	GetFileName(std::string Path);

#endif