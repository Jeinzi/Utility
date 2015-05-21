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
#ifdef _WIN32
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	_getch();
#else
	// The termios struct stores flags which can manipulate the I/O Interface.
	termios oldSettings, newSettings;

	// Get current terminal configuration.
	// STDIN_FILENO is the default standard input file descriptor.
	tcgetattr(STDIN_FILENO, &oldSettings);
	newSettings = oldSettings;

	// Turning flags off.
	// ECHO: displaying user input.
	// ICANON: returning string on enter/EOF/EOL
	newSettings.c_lflag &= ~(ICANON | ECHO );

	// Setting parameters to terminal.
	// TCSANOW: change parameters immediately.
	tcsetattr( STDIN_FILENO, TCSANOW, &newSettings);

	// Requesting char.
	getchar();

	// Writing old settings back to terminal.
	tcsetattr( STDIN_FILENO, TCSANOW, &oldSettings);
#endif
}


// Clears the terminal.
void ClearTerminal()
{
#ifdef _WIN32
	COORD origin = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	// Get number of cells in console.
	GetConsoleScreenBufferInfo(console, &consoleInfo);
	DWORD written, cells = consoleInfo.dwSize.X * consoleInfo.dwSize.Y;
	// Fill all the cells with a blank.
	FillConsoleOutputCharacter(console, ' ', cells, origin, &written);
	FillConsoleOutputAttribute(console, consoleInfo.wAttributes, cells, origin, &written);
	SetConsoleCursorPosition(console, origin);
#else
	// \x1B		Starting the escape sequence.
	// [2J		Clears the entire screen.
	// [1;1H	Sets the cursor to the top left corner.
	std::cout << "\x1B[2J \x1B[1;1H";
#endif
}


// Sets the title of the terminal
void SetTerminalTitle(std::string title)
{
#ifdef _WIN32
	SetConsoleTitle(title.c_str());
#else
	// \x1B		Starting xterm escape sequence.
	// \0;		Set icon name and window title.
	// \007		Bell character.
	std::cout << "\x1B]0;" << title << "\007";
#endif
}


// Clears the terminal and prints a standardized header.
void PrepareTerminal(std::string programName, std::string version, std::string description)
{
	// Set terminal title.
	std::string	title = programName + " " + version;
	ClearTerminal();
	SetTerminalTitle(title);
	ChangeColor(Color::Input);

	// Get console width and fill first row with asterisks.
	unsigned long asterisksLeft;
	unsigned long asterisksRight;

#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &consoleInfo);
	asterisksLeft = (consoleInfo.dwSize.X - (programName.length() + 2)) / 2;
	asterisksRight = (consoleInfo.dwSize.X - (programName.length() + 2)) / 2
		+ (consoleInfo.dwSize.X - (programName.length() + 2)) % 2;
#else
	// TODO: Get terminal width under linux.
	asterisksLeft = 30;
	asterisksRight = 30;
#endif

	// Assembling caption.
	std::string caption;
	for (unsigned int i = 0; i < asterisksLeft; i++) caption += "*";
	caption += " ";
	caption += programName;
	caption += " ";
	for (unsigned int i = 0; i < asterisksRight; i++) caption += "*";

	// Printing.
	std::cout << caption << std::endl << std::endl;
	if (!description.empty())
	{
		std::cout << description << std::endl << std::endl;
	}
}


// Prints a string letter by letter with an arbitrary delay.
// Pressing 'q' will print the rest of the string instantly.
void PrintText(std::string text, unsigned int pause)
{
	// Need to change behaviour of terminal when using Linux
	// in order to check if a key has been pressed
	// while constantly printing characters.
#ifdef __linux__
	// Disable echo and waiting for user input.
	// See Wait() for detailed comments.
	termios oldSettings, newSettings;
	tcgetattr(STDIN_FILENO, &oldSettings);

	newSettings = oldSettings;
	newSettings.c_lflag &= ~(ICANON | ECHO);
	tcsetattr( STDIN_FILENO, TCSANOW, &newSettings);

	// Using fcntl() in fcntl.h to manipulate file descriptor.
	// F_GETFL:	Get file descriptor of standard in-/output.
	int oldFileDescriptor = fcntl(STDIN_FILENO, F_GETFL);
	// Set new file descriptor with O_NONBLOCK flag disabled.
	// O_NONBLOCK:	No operation on the file descriptor
	// 		will cause the calling process to wait.
	fcntl(STDIN_FILENO, F_SETFL, oldFileDescriptor | O_NONBLOCK);
#endif

	// All OSs: Print characters of string one by one.
	for (unsigned int i = 0; i < text.length(); i++)
	{
		std::cout << text[i];
		// Forcing to print everything in buffer instantly (needed for linux).
		fflush(stdout);
		char chr;


		// Get char if a key has been pressed.
#ifdef _WIN32
		if (_kbhit())
		{
			chr = _getch();
		}
#else
		chr = getchar();
#endif

		// All OSs: Convert char to lower case.
		chr = ToLower(std::string(1, chr))[0];
		// Print the remaining string instantly when q has been pressed.
		if (chr == 'q')
		{
			for (i += 1; i < text.length(); i++)
			{
				std::cout << text[i];
			}
			break;
		}

		// Sleep between two characters.
#ifdef _WIN32
		Sleep(pause);
#else
		usleep(pause * 1000);
#endif
	}

	// Reset terminal to previous settings when running on linux.
#ifdef __linux__
	tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
	fcntl(STDIN_FILENO, F_SETFL, oldFileDescriptor);
#endif

	std::cout << std::endl;
}


// TODO Review function
// Copies a directory to a new location.
bool CopyDirectory(std::string sourcePath, std::string targetPath, bool copySubdirectories)
{
#ifdef _WIN32
	int				errorCode = 0;
	std::string		targetFilePath = "";
	std::string		sourceFilePath = "";
	std::string		searchPath = sourcePath + "/*";
	HANDLE			hFind;
	WIN32_FIND_DATA	findFileData;

	std::cout << "Trying to copy files from '" << sourcePath << "' in '" << targetPath << "'." << std::endl;

	// Creating directory.
	if (!PathFileExists(targetPath.c_str()))
	{
		std::cout << "Directory '" << targetPath << "' does not exist." << std::endl;
		errorCode = CreateDirectory(targetPath.c_str(), 0);
		std::cout << "Created directory." << std::endl;
		if (errorCode == 0)
		{
			std::cout << "Error." << std::endl;
			return(false);
		}
	}

	std::cout << "Copying files from '" << sourcePath << "' in '" << targetPath << "'." << std::endl;

	// Finding and copiing files.
	hFind = FindFirstFile(searchPath.c_str(), &findFileData);
	FindNextFile(hFind, &findFileData);
	while (FindNextFile(hFind, &findFileData))
	{
		std::cout << "Finding next File." << std::endl;
		targetFilePath = targetPath + "\\" + findFileData.cFileName;
		sourceFilePath = sourcePath + "\\" + findFileData.cFileName;
		std::cout << "Copying files from '" << sourceFilePath << "' to '" << targetFilePath << "'." << std::endl;

		// Copy directories and files.
		if (findFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{

			if (copySubdirectories)
			{
				std::cout << "Copying directory." << std::endl;
				CopyDirectory(sourceFilePath, targetFilePath, true);
			}
		}
		else
		{
			std::cout << "Copying file." << std::endl;
			CopyFile(sourceFilePath.c_str(), targetFilePath.c_str(), false);
		}
	}
	FindClose(hFind);
	std::cout << "Returning." << std::endl;
	return(true);
#else
	// TODO Add Linux support.
	return(false);
#endif
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
// respectInterpunctation defaults to true.
int CountWords(std::string text)
{
	bool	word = false;
	int		wordCount = 0;

	// Search through string.
	for (unsigned int i = 0; i < text.length(); i++)
	{
		// If there isn't a char separating different words, wordCount is incremented once.
		if (text[i] != ' ' && text[i] != ',' && text[i] != '.' && text[i] != '?' && text[i] != '!')
		{
			if (!word)
			{
				wordCount++;
			}
			word = true;
		}
		else
		{
			word = false;
		}
	}

	return(wordCount);
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
std::string ToLower(std::string text)
{
	transform(text.begin(), text.end(), text.begin(), ::tolower);
	return(text);
}


// Capitalizes the first letter in a string.
std::string FirstToUpper(std::string text)
{
	if (text.empty()) return("");

	transform(text.begin(), text.begin() + 1, text.begin(), ::toupper);
	return(text);
}


// Returns the word at the specified index.
std::string GetWord(std::string text, unsigned int index)
{
	bool word = false;
	int currentIndex = -1;
	std::string extractedWord = "";

	// Search through string.
	for (unsigned int i = 0; i < text.length(); i++)
	{
		if (text[i] != ' ' && text[i] != ',' && text[i] != '.' && text[i] != '?' && text[i] != '!')
		{
			// If there is no space or punctation mark, there is a word.
			// Following chars will be extracted until the next occurence of a non-word-character.
			if (!word)
			{
				currentIndex++;
			}
			word = true;
			if (currentIndex == index)
			{
				extractedWord += text[i];
			}
		}
		else
		{
			word = false;
		}
	}

	return(extractedWord);
}


// Converts an integer to a hex formatted string.
std::string IntToHexString(int number)
{
	if (number == 0) return("0");

	bool positive = true;
	int			remainder;
	std::string	output;
	// Remember sign and deal with positive number.
	if (number < 0)
	{
		positive = false;
		number = -number;
	}

	// Add remainder auf (number % 16) to string, then divide by 16.
	while (number > 0)
	{
		remainder = number % 16;

		if (remainder <= 9)
		{
			output += std::to_string(remainder);
		}
		else
		{
			switch (remainder)
			{
			case 10:
				output += 'A';
				break;
			case 11:
				output += 'B';
				break;
			case 12:
				output += 'C';
				break;
			case 13:
				output += 'D';
				break;
			case 14:
				output += 'E';
				break;
			case 15:
				output += 'F';
				break;
			}
		}

		number /= 16;
	}

	// Add sign.
	if (!positive) output += "-";
	// Reverse string.
	std::reverse(output.begin(), output.end());
	return(output);
}


// Escapes a string for use in an URL (reserved characters are converted in a hex code).
std::string Escape(std::string text)
{
	std::string output = "";

	for (unsigned int i = 0; i < text.length(); i++)
	{
		if (isalnum(text[i])
			|| text[i] == '-'
			|| text[i] == '_'
			|| text[i] == '.'
			|| text[i] == '~')
		{
			output += text[i];
		}
		else
		{
			output += '%';
			unsigned char temp = text[i];
			output += IntToHexString(temp);
		}
	}

	return(output);
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
std::string GetFileFormat(std::string path)
{
	bool		foundDot = false;
	std::string	format = "";

	// Searches the string from last to first character.
	for (int i = path.length() - 1; i >= 0; i--)
	{
		// If a dot has been found, all characters to the end of the string are copied.
		if (path[i] == '.')
		{
			foundDot = true;
			i++;
			for (unsigned int j = i; j < path.length(); j++)
			{
				format += path[j];
			}
			break;
		}
	}

	return(format);
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
