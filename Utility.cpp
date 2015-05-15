#include "Utility.h"

void Color(int ColorCode)
{
	//Ändert die Farbe, in der Text in der Konsole dargestellt wird
	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(Handle, ColorCode);
	return;
}

void Error(string Error)
{
	//Gibt eine standardmäßig formatierte Fehlermeldung aus
	Color(ERROR_COLOR);
	cout << "<!> ";
	Color(OUTPUT_COLOR);
	cout << Error;
	Color(ERROR_COLOR);
	cout << " <!>" << endl;
	Color(OUTPUT_COLOR);
}

void Wait()
{
	//Wartet, bis eine beliebige Taste gedrückt wird
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	while(true)
	{
		if(_kbhit())
		{
			break;
		}
		Sleep(100);
	}
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}

void PrepareConsole(string ProgramName, string Version, string Description)
{
	//Bereitet den Bildschirm standardisiert vor
	string	Title = ProgramName + " " + Version;

	system("cls");
	Color(INPUT_COLOR);
	SetConsoleTitle(Title.c_str());

	cout << "****************************** " << ProgramName << " ******************************" << endl << endl;
	if(Description != "")
	{
		cout << Description << endl << endl;
	}
	return;
}

void PrintText(string Text, int Pause)
{
	//Gibt einen Text schrittweise aus (Ein Buchstabe nach dem anderen)
	//Wird dabei die Taste 'q' gedrückt, wird der Text direkt ausgegeben

	//Pausenzeit wird korrigiert
	if(Pause < 0)
	{
		Pause = 0;
	}

	for(unsigned int i = 0; i < Text.length(); i++)
	{
		cout << Text[i];
		if(_kbhit())
		{
			if(_getch() == 'q' || _getch() == 'Q')
			{
				for(i += 1; i < Text.length(); i++)
				{
					cout << Text[i];
				}
				break;
			}
		}
		Sleep(Pause);
	}

	cout << endl;
	return;
}

bool CopyDirectory(string SourcePath, string TargetPath, bool CopySubdirectories)
{
	//Kopiert einen Ordner an einen anderen Ort, standardmäßig mit Unterordnern
	int				ErrorCode = 0;
	string			TargetFilePath = "";
	string			SourceFilePath = "";
	string			SearchPath = SourcePath + "/*";
	HANDLE			HFind;
	WIN32_FIND_DATA	FindFileData;

	cout << "Trying to copy files from '" << SourcePath << "' in '" << TargetPath << "'." << endl;

	//Erstellt, wenn nötig, das Verzeichnis
	if(!PathFileExists(TargetPath.c_str()))
	{
		cout << "Directory '" << TargetPath << "' does not exist." << endl;
		ErrorCode = CreateDirectory(TargetPath.c_str(), 0);
		cout << "Created directory." << endl;
		if(ErrorCode == 0)
		{
			cout << "Error." << endl;
			return(false);
		}
	}

	cout << "Copying files from '" << SourcePath << "' in '" << TargetPath << "'." << endl;

	//Dateien werden gesucht und kopiert
	HFind = FindFirstFile(SearchPath.c_str(), &FindFileData);
	FindNextFile(HFind, &FindFileData);
	while(FindNextFile(HFind, &FindFileData))
	{
		cout << "Finding next File." << endl;
		TargetFilePath = TargetPath + "\\" + FindFileData.cFileName;
		SourceFilePath = SourcePath + "\\" + FindFileData.cFileName;
		cout << "Copying files from '" << SourceFilePath << "' to '" << TargetFilePath << "'." << endl;

		//Ordner werden, wenn gefordert, kopiert
		if(FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{

			if(CopySubdirectories)
			{
				cout << "Copying directory." << endl;
				CopyDirectory(SourceFilePath, TargetFilePath, true);
			}
		}
		else
		{
			cout << "Copying file." << endl;
			CopyFile(SourceFilePath.c_str(), TargetFilePath.c_str(), false);
		}
	}
    FindClose(HFind);
	cout << "Returning." << endl;
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

int CountWords(string Text, bool RespectInterpunctation)
{
	//Gibt die Anzahl an Wörtern in einem String zurück
	bool	Word = false;
	int		WordCount = 0;

	for (unsigned int i = 0; i < Text.length(); i++)
	{
		if ( Text[i] != ' ' && ((Text[i] != ',' && Text[i] != '.' && Text[i] != '?' && Text[i] != '!') || !RespectInterpunctation ) )
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

string ToLower(string Text) {
	//Gibt einen String konvertiert in Kleinbuchstaben aus
	transform(Text.begin(), Text.end(), Text.begin(), ::tolower);
	return(Text);
}

string FirstToUpper(string Text)
{
	//Wandelt den ersten Buchstaben eines Strings in einen Großbuchstaben um
	if(Text == "")
	{
		return("");
	}

	transform(Text.begin(), Text.begin() + 1, Text.begin(), ::toupper);
	return(Text);
}

string GetWord(string Text, int Index)
{
	//Gibt das Wort an der mit Index angegebenen Stelle zurück
	unsigned int WordCount = 0;
	string Word = "";

	//Indexkorrektur
	if(Index < 0)
	{
		Index = 0;
	}
	if (Text[0] != ' ')
	{
		WordCount = 1;
	}

	for(unsigned int i = 0; i < Text.length(); i++)
	{
		if(Text[i] == ' ')
		{
			WordCount++;
			continue;
		}
		if(WordCount == Index)
		{
			Word += Text[i];
		}
	}

	return(Word);
}


string ReverseString(string Text)
{
	//Dreht einen String um und gibt diesen zurück
	string Output = "";

	for(int i = Text.length() - 1; i >= 0; i--)
	{
		Output += Text[i];
	}

	return(Output);
}


string IntToHex(int Number)
{
	//Wandelt eine Ganzzahl in Hexadezimale Schreibweise um und gibt diese als String zurück
	int		Temp = 0;
	string	Output = "";

	if(Number < 1)
	{
		return("0");
	}
	
	while(Number > 0)
	{
		Temp = Number % 16;

		if(Temp <= 9)
		{
			Output += IntToString(Temp);
		}
		else
		{
			switch(Temp)
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
				cout << "Fehler in IntToHex: Integer ist keine Hexadezimale Ziffer." << endl;
			}
		}

		Number /= 16;
	}

	Output = ReverseString(Output);
	return(Output);
}

string Escape(string Text)
{
	//Escaped einen String, um ihn in einer URL verwenden zu können - Sonderzeichen werden in %HexZahl konvertiert
	string Output = "";

	for(unsigned int i = 0; i < Text.length(); i++)
	{
		if( (Text[i] >= 48 && Text[i] <= 57) || (Text[i] >= 65 && Text[i] <= 90) || (Text[i] >= 97 && Text[i] <= 122) || Text[i] == '@' || Text[i] == '*' || Text[i] == '-' || Text[i] == '_' || Text[i] == '+' || Text[i] == '.' || Text[i] == '/')
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

string GetUserName()
{
	//Gibt den Benutzernamen zurück
	DWORD	Size;
	char	UserName[255];

	Size = sizeof(UserName);
	GetUserName(UserName, &Size);

	return UserName;
}

string GetComputerName()
{
	//Gibt den Computernamen zurück
	DWORD	Size;
	char	ComputerName[255];

	Size = sizeof(ComputerName);
	GetComputerName(ComputerName, &Size);

	return ComputerName;
}

string GetFileFormat(string FileName)
{
	//Gibt das Format eines übergebenen Dateinamens zurück
	bool	FoundDot = false;
	string	Format = "";

	for (int i = FileName.length() - 1; i >= 0; i--)
	{
		//Wurde ein Punkt gefunden, wird der Dateiname von diesem Char bis zum Ende kopiert
		if (FileName[i] == '.')
		{
			//Format wird herausgefiltert
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

// Extrahiert den Dateinamen aus einem Pfad und gibt diesen zurück.
string	GetFileName(string path)
{
	bool	foundSlash = false;
	string	name = "";

	for (int i = path.length() - 1; i >= 0; i--)
	{
		// Wurde ein Slash gefunden, wird der Dateiname von diesem Char bis zum Ende kopiert.
		if (path[i] == '/' || path[i] == '\\')
		{
			// Dateiname wird herausgefiltert.
			foundSlash = true;
			name = path.substr(i + 1);
			break;
		}
	}

	if (foundSlash) return(name);
	else return(path);
}