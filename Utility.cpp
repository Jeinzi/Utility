#include "Utility.h"

// Changes the color in which the console text is being displayed.
void ChangeColor(Color color)
{
#ifdef _WIN32
	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(Handle, (int)color);
#else
	char colorCode = (char)color;
	bool bright = false;
	std::string escapeCode("\x1B[");

	// If the color is a light version of an other color,
	// map the light version to the normal version and add attribute "bright".
	if (colorCode > 7)
	{
		colorCode -= 8;
		bright = true;
	}
	escapeCode += std::to_string(30 + colorCode);
	escapeCode += "m";

	if (bright)
	{
		// Display brighter colors.
		escapeCode += "\x1B[1m";
	}
	else
	{
		// Display darker colors.
		escapeCode += "\x1B[2m";
	}

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
	std::cout << "\x1B[2J\x1B[1;1H";
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

	// Get console width (OS dependent).
	unsigned long width;
	unsigned long asterisksLeft;
	unsigned long asterisksRight;

#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &consoleInfo);
	width = consoleInfo.dwSize.X;
#else
	winsize size;
	// First argument of ioctl()  is an open file descriptor.
	// Second argument is a device-dependent request code.
	// The third argument is an untyped pointer to memory.
	ioctl(0, TIOCGWINSZ, &size);
	width = size.ws_col;
#endif

	asterisksLeft = (width - (programName.length() + 2)) / 2;
	asterisksRight = (width - (programName.length() + 2)) / 2
		+ (width - (programName.length() + 2)) % 2;

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

	// All OSs: Print characters of the string one by one.
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


// Returns a boolean indicating if the specified path is a valid file or directory.
bool PathExists(std::string path)
{
	bool pathExists;
#ifdef _WIN32
	// Converting the return value of PathFileExists()
	// (which is an integer) to a boolean value.
	pathExists = (PathFileExists(path.c_str()) != 0);
#else
	// Although this is compiled as c++ code, the struct keyword is required
	// because of the ambiguity of the structure stat and the function stat().
	struct stat fileStats;
	// Getting information about the file.
	// If the operation has been successful, 0 is returned and the file thus exists.
	pathExists = (stat(path.c_str(), &fileStats) == 0);
#endif
	return(pathExists);
}


// Creates a directory and its parent directories, if necessary.
// Returns a boolean value indicating if the directory has been created successfully.
bool CreateDirectory(std::string path)
{
	// Instantly return true if the path already exists.
	if (PathExists(path))
	{
		return(true);
	}

	// Replace backslashes by forward slashes.
	for (unsigned int i = 0; i < path.length(); i++)
	{
		if (path[i] == '\\')
		{
			path[i] = '/';
		}
	}

	// Get the path of the parent directory.
	std::size_t lastSlash = path.rfind('/');
	if (lastSlash != std::string::npos)
	{
		// Create the parent directory.
		// If it can not be created, return false.
		std::string parentDirectory = path.substr(0, lastSlash);
		if (!CreateDirectory(parentDirectory))
		{
			return(false);
		}
	}

	// The parent directories now exist,
	// so let's create the current directory in the OS specific way.
#ifdef _WIN32
	// Create a directory with default security descriptor.
	unsigned long returnValue = CreateDirectory(path.c_str(), NULL);
	if (returnValue == 0)
	{
		// Check errors.
		unsigned long error = GetLastError();
		if (error != ERROR_ALREADY_EXISTS)
		{
			return(false);
		}
	}
#else
	// mkdir() can be found in stat.h.
	// errno is defined in errno.h.
	// Create a directory with standard file mode bits.
	// S_IRWXU: 	Read, write, execute/search by owner.
	// S_IRWXG:	Read, write, execute/search by group.
	// S_IROTH:	Read permission, others.
	// S_IXOTH:	Execute/search permission, others.
	int returnValue = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if(returnValue != 0)
	{
		if(errno != EEXIST)
		{
			return(false);
		}
	}
#endif

	// If no error occured, return true.
	return(true);
}


// Returns true if the given path names a file, otherwise false.
bool IsFile(std::string path)
{
	// Return false if the path does not exist.
	if(!PathExists(path))
	{
		return(false);
	}

	bool isFile = true;
	// Check file attributes (OS specific).
#ifdef _WIN32
	// Windows: Get attributes.
	unsigned long fileAttributes;
	fileAttributes = GetFileAttributes(path.c_str());
	// If there was an error or the path specifies a directory,
	// false shall be returned.
	if(fileAttributes == INVALID_FILE_ATTRIBUTES
		|| (fileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		isFile = false;
	}
#else
	// Linux: Get information about the file/directory.
	struct stat stats;
	lstat(path.c_str(), &stats);
	// Check for directory.
	isFile = !S_ISDIR(stats.st_mode);
#endif

	return(isFile);
}


// Returns the number of words in a string.
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
// Empty string, when the operation has been unsuccessful.
std::string GetJeinziDirectory()
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
	// Remember sign and henceforth deal with a positive number.
	if (number < 0)
	{
		positive = false;
		number = -number;
	}

	// Add remainder (number % 16) to string, then divide by 16.
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


// Escapes a string for use in an URL (reserved characters are converted in hex escape sequence).
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


// Returns the format of a specified file name
// or an empty string, if there is no format.
std::string GetFileFormat(std::string path)
{
	// Searching the last occuring dot.
	size_t dotPosition;
	dotPosition = path.rfind('.');

	// If there is no dot, return an empty string.
	if (dotPosition == std::string::npos)
	{
		return("");
	}

	// If there is a dot, return everything thereafter.
	std::string format = path.substr(dotPosition + 1);
	return(format);
}


// Returns the filename from a path.
std::string GetFileName(std::string path)
{
	// Replace all backslashes by forward slashes.
	std::replace(path.begin(), path.end(), '\\', '/');

	// Search the last occurence of a slash.
	size_t slashPosition = path.rfind('/');

	if (slashPosition == std::string::npos)
	{
		// Return the path if no slash has been found.
		return(path);
	}

	// Otherwise return everything from the last slash to the end of the string.
	std::string name = path.substr(slashPosition + 1);
	return(name);
}
