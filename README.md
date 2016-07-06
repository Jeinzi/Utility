# Utility
#### Jabbering
This project features a collection of functions I ended up needing in various C++ software projects. Many of them originate from the first larger project I wrote, which was a text adventure I may eventually upload too. Their functionality can of course also be found in other libraries, but I just don't want to include a massive overhead when I really only need one or two methods of a specific library.

I also tried to keep the code cross-plattform, which means it should be compilable on most Linux distributions. That being said, this is my first attempt trying to write C++-Code that runs on other OSs than Windows, so don't expect flawless, bug-free code. It took me quite a while to figure things out, but I guess it was kinda fun. Maybe someone else than me can make use of this or learn something along the way.

#### Overview
A word on terminals and colors: Using the windows command prompt, there is a set of color codes that can be set directly as either background or foreground. Other terminals can be manipulated using [ANSI escape sequences](https://en.wikipedia.org/wiki/ANSI_escape_code#Colors) in order to change color or text formatting - but the interpretation of these codes vary. This library defines `enum class Color` whose members can be used instead of plain color codes. If you need advanced control on your terminal and you want your software to work reliably on any system, take a look at one of the implementations of [curses](https://en.wikipedia.org/wiki/Curses_(programming_library)).
```C++
void ChangeColor(Color color);
```
---
I had the desire to display a customly formatted error message:
```C++
void PrintError(std::string error);
```
---
Waits until an arbitrary key is being pressed. On Windows `_getch()` in _conio.h_ does the job. On Linux, this was a pain in the butt using only standard libraries, because the terminal has to be reconfigured in order to disable `getchar()`'s echo and waiting for a new line character.
```C++
void Wait();
```
---
Clears the terminal - easy using an ANSI escape code.
```C++
void ClearTerminal();
```
---
Sets the title of the terminal - also possible with escape sequences on non-windows terminals.
```C++
void SetTerminalTitle(std::string title);
```
---
Clears the terminal and outputs the program's name, version and a description.
```C++
void PrepareTerminal(std::string programName, std::string version, std::string description = "");
```
---
Prints a text letter by letter with a given delay between the individual characters. Prints the text instantly as soon as 'q' has been pressed.
```C++
void	PrintText(std::string text, unsigned int pause);
```
---
Checks whether a path exists in an OS-independant manner:
```C++
bool PathExists(std::string path);
```
---
Creates a directory and all it's parent directories:
```C++
bool CreateDirectory(std::string path);
```
---
Checks whether a path points to a file or a directory.
```C++
bool IsFile(std::string path);
```
---
Counts the words within a string. Takes spaces and punctuation into account.
```C++
int CountWords(std::string text);
```
---
This function returns the path to the directory, where I like to save my own application data. That is, on window _$AppData$/Roaming/Jeinzi/_ and on Linux _$home$/.Jeinzi_. Btw I could go ballistic if some software decides to save its data in my main user directory (windows) :angry:
```C++
std::string GetJeinziDirectory();
```
---
Converts a string to lower case.
```C++
std::string ToLower(std::string text);
```
Maybe I'll remove that function since it's really just a call to
```C++
transform(text.begin(), text.end(), text.begin(), ::tolower);
```
---
Converts the first character of a string to upper case. Same game as `ToLower(std::string)`.
```C++
std::string FirstToUpper(std::string text);
```
---
Returns the word at the given _index_ within the string. Pays attention to punctuation marks, of course.
```C++
std::string GetWord(std::string text, unsigned int index);
```
---
Converts an integer into a hex formatted string. I really couldn't find a method for that.
...Yeah, well, I guess I could have used C-style `printf()` if I really didn't want to write my own function.
```C++
std::string IntToHexString(int number);
```
---
Escapes a string for use in an URL - reserved characters are converted in the corresponding hex escape sequence. Needed that when I was writing an image crawler for 4chan using _curl_.
```C++
std::string EscapeURL(std::string text);
```
---
Yeah. Returns the computer's name.
```C++
std::string GetComputerName();
```
---
This was a bit confusion on Linux, at first.
```C++
std::string GetUserName();
```
---
Really just searches through the string backwards until the first dot is reached and returns everything thereafter. If no dot is found, an empty string is returned.
```C++
std::string GetFileFormat(std::string path);
```
---
Returns everything after the last (back-)slash or the parameter itself, if there is no such character within it.
```C++
std::string GetFileName(std::string path);
```
