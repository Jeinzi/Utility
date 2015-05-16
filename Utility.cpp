#include "Utility.h"

// Changes the color in which the console text is beeing displayed.
void ChangeColor(Color color)
{
#ifdef _WIN32
	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(Handle, (int)color);
#else
	char colorCode = (char)color;
	bool bright = false;
	std::string escapeCode("\x1B[");

	if (colorCode > 7)
	{
		colorCode -= 8;
		bright = true;
	}
	escapeCode += std::to_string(30 + colorCode);
	if (bright) escapeCode += ";1";
	escapeCode += "m";

	std::cout << escapeCode;
#endif
}

// Prints a standardized error message.
void PrintError(std::string error)
{
	ChangeColor(Color::Error);
	std::cout << "<!> ";
	ChangeColor(Color::Output);
	std::cout << error;
	ChangeColor(Color::Error);
	std::cout << " <!>" << std::endl;
	ChangeColor(Color::Output);
}

// Waits until an arbitrary key is pressed.
void Wait()
{
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	while (true)
	{
		if (_kbhit())
		{
			break;
		}
		Sleep(100);
	}
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}

// Clears the terminal and prints a standardized header.
void PrepareConsole(std::string ProgramName, std::string Version, std::string Description)
{
	std::string	Title = ProgramName + " " + Version;

	system("cls");
	ChangeColor(Color::Input);
	SetConsoleTitle(Title.c_str());

	std::cout << "****************************** " << ProgramName << " ******************************" << std::endl << std::endl;
	if (Description != "")
	{
		std::cout << Description << std::endl << std::endl;
	}
	return;
}

// Prints a string letter by letter with an arbitrary delay.
// Pressing 'q' will print the rest of the string instantly.
void PrintText(std::string Text, unsigned int Pause)
{
	if (Pause < 0)
	{
		Pause = 0;
	}

	for (unsigned int i = 0; i < Text.length(); i++)
	{
		std::cout << Text[i];
		if (_kbhit())
		{
			if (_getch() == 'q' || _getch() == 'Q')
			{
				for (i += 1; i < Text.length(); i++)
				{
					std::cout << Text[i];
				}
				break;
			}
		}
		Sleep(Pause);
	}

	std::cout << std::endl;
	return;
}

// Copies a directory to a new location.
bool CopyDirectory(std::string SourcePath, std::string TargetPath, bool CopySubdirectories)
{
	int				ErrorCode = 0;
	std::string		TargetFilePath = "";
	std::string		SourceFilePath = "";
	std::string		SearchPath = SourcePath + "/*";
	HANDLE			HFind;
	WIN32_FIND_DATA	FindFileData;

	std::cout << "Trying to copy files from '" << SourcePath << "' in '" << TargetPath << "'." << std::endl;

	// Creating directory.
	if (!PathFileExists(TargetPath.c_str()))
	{
		std::cout << "Directory '" << TargetPath << "' does not exist." << std::endl;
		ErrorCode = CreateDirectory(TargetPath.c_str(), 0);
		std::cout << "Created directory." << std::endl;
		if (ErrorCode == 0)
		{
			std::cout << "Error." << std::endl;
			return(false);
		}
	}

	std::cout << "Copying files from '" << SourcePath << "' in '" << TargetPath << "'." << std::endl;

	// Finding and copiing files.
	HFind = FindFirstFile(SearchPath.c_str(), &FindFileData);
	FindNextFile(HFind, &FindFileData);
	while (FindNextFile(HFind, &FindFileData))
	{
		std::cout << "Finding next File." << std::endl;
		TargetFilePath = TargetPath + "\\" + FindFileData.cFileName;
		SourceFilePath = SourcePath + "\\" + FindFileData.cFileName;
		std::cout << "Copying files from '" << SourceFilePath << "' to '" << TargetFilePath << "'." << std::endl;

		// Copy directories and files.
		if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{

			if (CopySubdirectories)
			{
				std::cout << "Copying directory." << std::endl;
				CopyDirectory(SourceFilePath, TargetFilePath, true);
			}
		}
		else
		{
			std::cout << "Copying file." << std::endl;
			CopyFile(SourceFilePath.c_str(), TargetFilePath.c_str(), false);
		}
	}
	FindClose(HFind);
	std::cout << "Returning." << std::endl;
	return(true);
}

//Copies the Files of one directory to another, without Subfolders
//WIN32_FIND_DATA	FindFileData;
//HANDLE			hFind;
//string			SearchPath = SourcePath + "\\*";

//CreateDirectory(TargetPath.c_str(), 0);
//hFind=FindFirstFile(SearchPath.c_str(), &FindFileData);
//FindNextFile(hFind, &FindFileData);
//string NewFilePath;
//string FilePath;
//while(FindNextFile(hFind, &FindFileData)) {
//NewFilePath = newpath + "\\" + FindFileData.cFileName;
//FilePath = path + "\\" + FindFileData.cFileName;
//CopyFile(FilePath.c_str(), NewFilePath.c_str(), false);
//}
//   FindClose(hFind);

// Returns the number of words in a string.
int CountWords(std::string Text, bool RespectInterpunctation)
{
	bool	Word = false;
	int		WordCount = 0;

	for (unsigned int i = 0; i < Text.length(); i++)
	{
		if (Text[i] != ' ' && ((Text[i] != ',' && Text[i] != '.' && Text[i] != '?' && Text[i] != '!') || !RespectInterpunctation))
		{
			if (Word == false)
			{
				WordCount++;
			}
			Word = true;
		}
		else
		{
			Word = false;
		}
	}

	return(WordCount);
}

// Returns the path to the custom "Jeinzi" directory,
// where all the application data is stored.
// Empty string, when operation was unsuccessful.
std::string GetAppDataDirectory()
{
	std::string path;

#ifdef _WIN32
	// Memory for roamingPath is allocated by SHGetKnownFolderPath()!
	wchar_t* roamingPath = nullptr;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, NULL, NULL, &roamingPath)))
	{
		std::wstringstream ss;
		ss << roamingPath << L"\\Jeinzi";
		std::wstring wide = ss.str();
		path = std::string(wide.begin(), wide.end());

		// Free memory.
		CoTaskMemFree(static_cast<void*>(roamingPath));
	}
	else
	{
		path = "";
	}
#else
	path = std::string(getenv("HOME")) + "/.Jeinzi";
#endif

	return(path);
}

// Converts a string to lower case.
std::string ToLower(std::string Text)
{
	transform(Text.begin(), Text.end(), Text.begin(), ::tolower);
	return(Text);
}

// Capitalizes the first letter in a string.
std::string FirstToUpper(std::string Text)
{
	//Wandelt den ersten Buchstaben eines Strings in einen Großbuchstaben um
	if (Text == "")
	{
		return("");
	}

	transform(Text.begin(), Text.begin() + 1, Text.begin(), ::toupper);
	return(Text);
}

// Returns the word at the specified index.
std::string GetWord(std::string Text, int Index)
{
	unsigned int WordCount = 0;
	std::string Word = "";

	//Indexkorrektur
	if (Index < 0)
	{
		Index = 0;
	}
	if (Text[0] != ' ')
	{
		WordCount = 1;
	}

	for (unsigned int i = 0; i < Text.length(); i++)
	{
		if (Text[i] == ' ')
		{
			WordCount++;
			continue;
		}
		if (WordCount == Index)
		{
			Word += Text[i];
		}
	}

	return(Word);
}

// Reverses a string!
std::string ReverseString(std::string Text)
{
	std::string Output = "";

	for (int i = Text.length() - 1; i >= 0; i--)
	{
		Output += Text[i];
	}

	return(Output);
}

// Converts an integer to a hex formatted string.
std::string IntToHex(int Number)
{
	int			Temp = 0;
	std::string	Output = "";

	if (Number < 1)
	{
		return("0");
	}

	while (Number > 0)
	{
		Temp = Number % 16;

		if (Temp <= 9)
		{
			Output += std::to_string(Temp);
		}
		else
		{
			switch (Temp)
			{
			case 10:
				Output += 'A';
				break;
			case 11:
				Output += 'B';
				break;
			case 12:
				Output += 'C';
				break;
			case 13:
				Output += 'D';
				break;
			case 14:
				Output += 'E';
				break;
			case 15:
				Output += 'F';
				break;
			default:
				std::cout << "Fehler in IntToHex: Integer ist keine Hexadezimale Ziffer." << std::endl;
			}
		}

		Number /= 16;
	}

	Output = ReverseString(Output);
	return(Output);
}

// Escapes a string for use in an URL (special characters are converted in a hex code).
std::string Escape(std::string Text)
{
	std::string Output = "";

	for (unsigned int i = 0; i < Text.length(); i++)
	{
		if ((Text[i] >= 48 && Text[i] <= 57) || (Text[i] >= 65 && Text[i] <= 90) || (Text[i] >= 97 && Text[i] <= 122) || Text[i] == '@' || Text[i] == '*' || Text[i] == '-' || Text[i] == '_' || Text[i] == '+' || Text[i] == '.' || Text[i] == '/')
		{
			Output += Text[i];
		}
		else
		{
			Output += '%';
			unsigned char Temp = Text[i];
			Output += IntToHex(Temp);
		}
	}

	return(Output);
}

// Returns the computer's name. Empty string, if no name could be retrieved.
std::string GetComputerName()
{
	// Get an empty char array.
	const size_t bufferSize = 255;
	char nameBuffer[bufferSize];
	std::string name;

	// Get computer name (OS specific).
#ifdef _WIN32
	unsigned long nameLength = bufferSize;
	if (GetComputerName(nameBuffer, &nameLength) != 0)
	{
		name = nameBuffer;
	}
#else
	if (gethostname(nameBuffer, bufferSize) == 0)
	{
		name = nameBuffer;
	}
#endif

	return(name);
}

// Returns the username. Empty string, if no name could be retrieved.
std::string GetUserName()
{
	std::string name;

	// Get user name (OS specific).
#ifdef _WIN32
	const size_t bufferSize = 255;
	char nameBuffer[bufferSize];
	unsigned long nameLength = bufferSize;
	if (GetUserName(nameBuffer, &nameLength) != 0)
	{
		name = nameBuffer;
	}
#else
	char *nameBuffer = getenv("LOGNAME");
	name = nameBuffer;
#endif

	return(name);
}

// Returns the format of a specified file name.
std::string GetFileFormat(std::string FileName)
{
	bool	FoundDot = false;
	std::string	Format = "";

	// Searches the string from last to first character.
	for (int i = FileName.length() - 1; i >= 0; i--)
	{
		// If a dot has been found, all characters to the end of the string are copied.
		if (FileName[i] == '.')
		{
			FoundDot = true;
			i++;
			for (unsigned int j = i; j < FileName.length(); j++)
			{
				Format += FileName[j];
			}
			break;
		}
	}

	return(Format);
}

// Returns the filename from a path.
std::string GetFileName(std::string path)
{
	bool		foundSlash = false;
	std::string	name = "";

	// Searching through string backwards.
	for (int i = path.length() - 1; i >= 0; i--)
	{
		// If some kind of slash has been found, the rest of the string will be copied.
		if (path[i] == '/' || path[i] == '\\')
		{
			foundSlash = true;
			name = path.substr(i + 1);
			break;
		}
	}

	if (foundSlash) return(name);
	else return(path);
}
