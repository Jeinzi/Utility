#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <Windows.h>
#include <string>
#include <algorithm>
#include <conio.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

//Defines
#define ERROR_COLOR 4
#define OK_COLOR 6
#define INPUT_COLOR 15
#define OUTPUT_COLOR 7

//Funktionsprototypen
void	Color(int ColorCode);
void	Error(std::string Error);
void	Wait();
void	PrepareConsole(std::string ProgramName, std::string Version, std::string Description = "");
void	PrintText(std::string Text, int Pause);
bool	CopyDirectory(std::string SourcePath, std::string TargetPath, bool CopySubdirectories = true);
int		CountWords(std::string Text, bool RespectInterpunctation = true);
std::string	ToLower(std::string Text);
std::string	FirstToUpper(std::string Text);
std::string	GetWord(std::string Text, int Index);
std::string	IntToHex(int Number);
std::string	ReverseString(std::string Text);
std::string	Escape(std::string Text);
std::string	GetUserName();
std::string	GetComputerName();
std::string	GetFileFormat(std::string FileName);
std::string	GetFileName(std::string Path);

#endif