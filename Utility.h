#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#ifdef _WIN32
	#include <Windows.h>
	// Shell used to retrieve known folder paths (AppData, UserDirectory etc.).
	#include <ShlObj.h>
	// Library introduced by Borland to deal with the console, still supported by Visual Studio.
	#include <conio.h>
	#include <Shlwapi.h>
	#pragma comment(lib, "shlwapi.lib")
#else
	// Provides access to the POSIX operating system API. (gethostname() etc.)
	// Defines  miscellaneous  symbolic constants and types, and declares miscellaneous functions.
	// Used: STDIN_FILENO in Wait(), usleep().
	#include <unistd.h>
	// Used to retrieve environment variables.
	#include <stdlib.h>
	// Contains the definitions used by the terminal I/O interfaces.
	// Used in Wait() function.
	#include <termios.h>
	// Used to manipulate file descriptor as in PrintText().
	#include <fcntl.h>
	// Used to manipulates the underlying device parameters of special files.
	// Is used in PrepareTerminal() to retrieve the current size of the terminal.
	#include <sys/ioctl.h>
	// Used to obtain information about files as in PathExists().
	// Also, mkdir() is used in CreateDirectory() and S_ISDIR() is used in IsFile().
	#include <sys/stat.h>
	// Defines the integer variable errno, which is set by system calls
	// and some library functions in the event of an error.
	// Used in CreateDirectory().
	#include <errno.h>
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
	Gray = 7,
	DarkGray = 8,
	Blue = 9,
	Green = 10,
	Cyan = 11,
	Red = 12,
	Magenta = 13,
	Yellow = 14,
	White = 15,
#else
	// Linux: Values 0 - 7 matching ANSI escape codes.
	// To get the dark variant, the same escape codes
	// are used with an additional parameter.
	Black = 0,
	DarkRed = 1,
	DarkGreen = 2,
	DarkYellow = 3,
	DarkBlue = 4,
	DarkMagenta = 5,
	DarkCyan = 6,
	Gray = 7,
	DarkGray = 8,
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
	Output = Gray
};

// Function prototypes.
void	ChangeColor(Color color);
void	PrintError(std::string error);
void	Wait();
void	ClearTerminal();
void	SetTerminalTitle(std::string title);
void	PrepareTerminal(std::string programName, std::string version, std::string description = "");
void	PrintText(std::string text, unsigned int pause);
bool	PathExists(std::string path);
bool	CreateDirectory(std::string path);
bool	IsFile(std::string path);
int		CountWords(std::string text);
std::string	GetJeinziDirectory();
std::string	ToLower(std::string text);
std::string	FirstToUpper(std::string text);
std::string	GetWord(std::string text, unsigned int index);
std::string	IntToHexString(int number);
std::string	EscapeURL(std::string text);
std::string	GetComputerName();
std::string	GetUserName();
std::string	GetFileFormat(std::string path);
std::string	GetFileName(std::string path);

#endif
