#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <Windows.h>
#include <string>
#include <algorithm>
#include <conio.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

//Namespace
using namespace std;

//Defines
#define ERROR_COLOR 4
#define OK_COLOR 6
#define INPUT_COLOR 15
#define OUTPUT_COLOR 7

//Funktionsprototypen
void	Color(int ColorCode);
void	Error(string Error);
void	Wait();
void	PrepareConsole(string ProgramName, string Version, string Description = "");
void	PrintText(string Text, int Pause);
bool	CopyDirectory(string SourcePath, string TargetPath, bool CopySubdirectories = true);
int		StringToInt(string Number);
int		CountWords(string Text, bool RespectInterpunctation = true);
string	ToLower(string Text);
string	FirstToUpper(string Text);
string	GetWord(string Text, int Index);
string	IntToString(int Number);
string	IntToHex(int Number);
string	ReverseString(string Text);
string	Escape(string Text);
string	GetUserName();
string	GetComputerName();
string	GetFileFormat(string FileName);
string	GetFileName(string Path);

#endif