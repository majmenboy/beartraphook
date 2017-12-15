// General shit
#include "DLLMain.h"
#include "Utilities.h"

// Injection stuff
#include "INJ/ReflectiveLoader.h"

// Stuff to initialise
#include "Offsets.h"
#include "Interfaces.h"
#include "Mem.h"
#include "Hooks.h"
#include "RenderManager.h"
#include "Hacks.h"
#include "Menu.h"
#include "MiscHacks.h"
#include "Dumping.h"
#include "AntiAntiAim.h"
#include "hitmarker.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include <time.h> 
#include <fstream>
#include <Windows.h>
#include <string>
#include <iostream>
#include <time.h> 
#pragma comment(lib, "wldap32.lib" )
#pragma comment(lib, "crypt32.lib" )
#define CURL_STATICLIB
#include <Strsafe.h>
std::string StatusWordsToDisplay;

#define A 12312
#define B 23423
#define C 34534
#define FIRSTH 37
#define FIRSTH 37
#define VERSION 100
unsigned hash_str(const char* s)
{
	unsigned h = FIRSTH;
	while (*s) {
		h = (h * A) ^ (s[0] * B);
		s++;
	}
	return h; // or return h % C;
}
std::string Username;
std::string Email;
std::string Password;
float lineRealAngle;
float lineFakeAngle;
float headPos;
float flipBool;
int predicting;

// Used as part of the reflective DLL injection
extern HINSTANCE hAppInstance;

// Our DLL Instance
HINSTANCE HThisModule;
bool DoUnload;

UCHAR szFileSys[255], szVolNameBuff[255];
DWORD dwMFL, dwSysFlags;
DWORD dwSerial;
LPCTSTR szHD = "C:\\";

// Our thread we use to setup everything we need
// Everything appart from code in hooks get's called from inside 
// here.

std::string data; //will hold the url's contents
std::string HWIDCombine;
std::string HWID()
{
	using namespace std;

	SYSTEM_INFO siSysInfo;
	string UserComputerName;
	string test;
	string UserOsGUID;
	// Copy the hardware information to the SYSTEM_INFO structure. 
	// GetVolumeInformationA
	DWORD DriveSerial;

	// GetComputerNameA
	TCHAR ComputerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = sizeof(ComputerName) / sizeof(ComputerName[0]);
	// OS GUID
	string OsGUID;
	GetVolumeInformationA(0, nullptr, '\0', &DriveSerial, nullptr, nullptr, nullptr, 0);
	GetComputerNameA(ComputerName, &size);
	//UserDriveSerial = to_string(DriveSerial);
	HW_PROFILE_INFO hwProfileInfo;
	if (GetCurrentHwProfile(&hwProfileInfo))
	{
		UserOsGUID = hwProfileInfo.szHwProfileGuid;
		test = hwProfileInfo.szHwProfileName;
	}
	UserComputerName = ComputerName;

	string Messer = "sda23ad";
	GetSystemInfo(&siSysInfo);
	HWIDCombine = Messer + UserComputerName + UserOsGUID + test;
	return to_string(hash_str(HWIDCombine.c_str()));

}
inline static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}


template<typename Out>
void split(const std::string &s, char delim, Out result)
{
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}
std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

void mParseUrl(char *mUrl, std::string &serverName, std::string &filepath, std::string &filename)
{
	using namespace std;
	string::size_type n;
	string url = mUrl;

	if (url.substr(0, 7) == "http://")
		url.erase(0, 7);

	if (url.substr(0, 8) == "https://")
		url.erase(0, 8);

	n = url.find('/');
	if (n != string::npos)
	{
		serverName = url.substr(0, n);
		filepath = url.substr(n);
		n = filepath.rfind('/');
		filename = filepath.substr(n + 1);
	}

	else
	{
		serverName = url;
		filepath = "/";
		filename = "";
	}
}

SOCKET connectToServer(char *szServerName, WORD portNum)
{
	struct hostent *hp;
	unsigned int addr;
	struct sockaddr_in server;
	SOCKET conn;

	conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (conn == INVALID_SOCKET)
		return NULL;

	if (inet_addr(szServerName) == INADDR_NONE)
	{
		hp = gethostbyname(szServerName);
	}
	else
	{
		addr = inet_addr(szServerName);
		hp = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
	}

	if (hp == NULL)
	{
		closesocket(conn);
		return NULL;
	}

	server.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(portNum);
	if (connect(conn, (struct sockaddr*)&server, sizeof(server)))
	{
		closesocket(conn);
		return NULL;
	}
	return conn;
}

int getHeaderLength(char *content)
{
	const char *srchStr1 = "\r\n\r\n", *srchStr2 = "\n\r\n\r";
	char *findPos;
	int ofset = -1;

	findPos = strstr(content, srchStr1);
	if (findPos != NULL)
	{
		ofset = findPos - content;
		ofset += strlen(srchStr1);
	}

	else
	{
		findPos = strstr(content, srchStr2);
		if (findPos != NULL)
		{
			ofset = findPos - content;
			ofset += strlen(srchStr2);
		}
	}
	return ofset;
}

char *readUrl2(char *szUrl, long &bytesReturnedOut, char **headerOut)
{
	using namespace std;
	const int bufSize = 512;
	char readBuffer[bufSize], sendBuffer[bufSize], tmpBuffer[bufSize];
	char *tmpResult = NULL, *result;
	SOCKET conn;
	string server, filepath, filename;
	long totalBytesRead, thisReadSize, headerLen;

	mParseUrl(szUrl, server, filepath, filename);

	///////////// step 1, connect //////////////////////
	conn = connectToServer((char*)server.c_str(), 80);

	///////////// step 2, send GET request /////////////
	sprintf(tmpBuffer, "GET %s HTTP/1.0", filepath.c_str());
	strcpy(sendBuffer, tmpBuffer);
	strcat(sendBuffer, "\r\n");
	sprintf(tmpBuffer, "Host: %s", server.c_str());
	strcat(sendBuffer, tmpBuffer);
	strcat(sendBuffer, "\r\n");
	strcat(sendBuffer, "\r\n");
	send(conn, sendBuffer, strlen(sendBuffer), 0);

	//    SetWindowText(edit3Hwnd, sendBuffer);
	printf("Buffer being sent:\n%s", sendBuffer);

	///////////// step 3 - get received bytes ////////////////
	// Receive until the peer closes the connection
	totalBytesRead = 0;
	while (1)
	{
		memset(readBuffer, 0, bufSize);
		thisReadSize = recv(conn, readBuffer, bufSize, 0);

		if (thisReadSize <= 0)
			break;

		tmpResult = (char*)realloc(tmpResult, thisReadSize + totalBytesRead);

		memcpy(tmpResult + totalBytesRead, readBuffer, thisReadSize);
		totalBytesRead += thisReadSize;
	}

	headerLen = getHeaderLength(tmpResult);
	long contenLen = totalBytesRead - headerLen;
	result = new char[contenLen + 1];
	memcpy(result, tmpResult + headerLen, contenLen);
	result[contenLen] = 0x0;
	char *myTmp;

	myTmp = new char[headerLen + 1];
	strncpy(myTmp, tmpResult, headerLen);
	myTmp[headerLen] = NULL;
	delete(tmpResult);
	*headerOut = myTmp;

	bytesReturnedOut = contenLen;
	closesocket(conn);
	return(result);
}
bool DownloadHacks = false;
bool DownloadHacks3 = false;
bool DownloadHacks4 = false;
bool DownloadHacks5 = false;
bool DownloadHacks6 = false;
bool DownloadHacks7 = false;
bool DownloadHacks8 = false;
bool isInCharString(char *str1, char *search)
{
	for (int i = 0; i < strlen(str1); ++i)
	{
		if (strncmp(&str1[i], search, strlen(search)) == 0)
			return true;
	}

	return false;
}
bool UserIsPremiumEmail()
{
	using namespace std;
	HINSTANCE hInst;
	WSADATA wsaData;

	int x = 0;
	string SplitEmail1;
	string SplitEmail2;
	const int bufLen = 1024;
	std::vector<std::string> SplitEmail = split(Email, '@');
	for (const auto& text : SplitEmail)
	{

		if (x == 0)
		{
			SplitEmail1 = text;
		}
		if (x == 1)
		{
			SplitEmail2 = text;
		}
		x++;
	}
	string SplitEmail3 = SplitEmail1 + "%40" + SplitEmail2;
	string URL = "" + SplitEmail3 + "&submit=Submit";
	char *szUrl = new char[URL.length() + 1];
	strcpy(szUrl, URL.c_str());

	long fileSize;
	char *memBuffer, *headerBuffer;
	FILE *fp;

	memBuffer = headerBuffer = NULL;

	if (WSAStartup(0x101, &wsaData) != 0)
		exit(1);

	memBuffer = readUrl2(szUrl, fileSize, &headerBuffer);
	delete[] szUrl;
	WSACleanup();
	/*
	'Admin---------------------4
	'Premium Garry's Mod-------11
	'Premium CS:GO Lite--------9
	'Premium CS:GO Beta--------10
	'Premium CS:GO-------------8
	'Banned--------------------7
	'Not-----------------------2
	*/
	std::vector<std::string> websiteData = split(memBuffer, ',');
	for (const auto& text : websiteData)
	{
		char *testchar = new char[text.length() + 1];
		strcpy(testchar, text.c_str());
		std::vector<std::string> Something = split(text, ',');
		if (isInCharString(testchar, "8") || isInCharString(testchar, "4")) // checks the .txt file if you are allowed
		{
			return true;
		}
		delete[] testchar;
	}
	return false;
}
bool UserIsPremium()
{
	using namespace std;
	HINSTANCE hInst;
	WSADATA wsaData;

	const int bufLen = 1024;
	string URL = "" + Username + "&submit=Submit";
	char *szUrl = new char[URL.length() + 1];
	strcpy(szUrl, URL.c_str());

	long fileSize;
	char *memBuffer, *headerBuffer;
	FILE *fp;

	memBuffer = headerBuffer = NULL;

	if (WSAStartup(0x101, &wsaData) != 0)
		exit(1);

	memBuffer = readUrl2(szUrl, fileSize, &headerBuffer);
	delete[] szUrl;
	WSACleanup();
	/*
	'Admin---------------------4
	'Premium Garry's Mod-------11
	'Premium CS:GO Lite--------9
	'Premium CS:GO Beta--------10
	'Premium CS:GO-------------8
	'Banned--------------------7
	'Not-----------------------2
	*/
	std::vector<std::string> websiteData = split(memBuffer, ',');
	for (const auto& text : websiteData)
	{
		char *testchar = new char[text.length() + 1];
		strcpy(testchar, text.c_str());
		std::vector<std::string> Something = split(text, ',');
		if (isInCharString(testchar, "8") || isInCharString(testchar, "4")) // checks the .txt file if you are allowed
		{
			return true;
		}
		delete[] testchar;
	}
	return false;
}
HINSTANCE hInst;
WSADATA wsaData;

void CheckHWID()
{
	using namespace std;

	const int bufLen = 1024;
	string URL = "" + Username + "&hwidin=" + HWID() + "&submit=Submit";
	char *szUrl = new char[URL.length() + 1];
	strcpy(szUrl, URL.c_str());

	long fileSize;
	char *memBuffer, *headerBuffer;
	FILE *fp;

	memBuffer = headerBuffer = NULL;

	if (WSAStartup(0x101, &wsaData) != 0)
		exit(1);


	bool DownloadHacks1 = false;
	bool DownloadHacks2 = false;
	memBuffer = readUrl2(szUrl, fileSize, &headerBuffer);
	delete[] szUrl;
	WSACleanup();

	if (isInCharString(memBuffer, "HWID is correct"))
	{
		DownloadHacks1 = true;
	}
	else
	{
	}

	if (UserIsPremium() && DownloadHacks1 && UserIsPremiumEmail())
	{
		DownloadHacks3 = true;
		DownloadHacks4 = true;
		DownloadHacks5 = true;
		DownloadHacks6 = true;
		DownloadHacks7 = true;
		DownloadHacks8 = true;
		DownloadHacks = true;
	}

}
const std::string currentDateTime()
{
	using namespace std;
	time_t theTime = time(NULL);
	struct tm *aTime = localtime(&theTime);

	int day = aTime->tm_mday;
	int month = aTime->tm_mon + 1; // Month is 0 – 11, add 1 to get a jan-dec 1-12 concept
	int year = aTime->tm_year + 1900; // Year is # years since 1900
	int hour = aTime->tm_hour;
	int min = aTime->tm_min;

	std::string Time = to_string(year) + to_string(month) + to_string(day) + to_string(hour) + to_string(min);
	return Time;
}
/*

	CheckHWID();
	CheckForDebuggers();
	CheckForDebugger();
	// Intro banner with info
	PrintMetaHeader();

	//---------------------------------------------------------
	// Initialise all our shit
	Offsets::Initialise(); // Set our VMT offsets and do any pattern scans
	Interfaces::Initialise(); // Get pointers to the valve classes
	NetVar.RetrieveClasses(); // Setup our NetVar manager (thanks shad0w bby)
	NetvarManager::Instance()->CreateDatabase();
	Render::Initialise();
	//hitmarker::singleton()->initialize();
	Hacks::SetupHacks();
	Menu::SetupMenu();
	Hooks::Initialise();
	ApplyNetVarsHooks();

	// Dumping
	//Dump::DumpClassIds();

	//---------------------------------------------------------

	// While our cheat is running
	while (DoUnload == false)
	{
		CheckForDebuggers();
		CheckForDebugger();
		Sleep(3000);
	}

	RemoveNetVarsHooks();
	Hooks::UndoHooks();
	Sleep(2000); // Make sure none of our hooks are running
	FreeLibraryAndExitThread(HThisModule, 0);
	DelMe();
	return 0;





*/
int InitialThread()
{
	using namespace std;

	ifstream UsernameFile;
	UsernameFile.open("C:\\Windows\\username.txt");
	UsernameFile >> Username;
	UsernameFile.close();

	ifstream EmailFile;
	EmailFile.open("C:\\Windows\\email.txt");
	EmailFile >> Email;
	EmailFile.close();

	ifstream PasswordFile;
	PasswordFile.open("C:\\Windows\\password.txt");
	PasswordFile >> Password;
	PasswordFile.close();

	remove("C:\\Windows\\username.txt");
	remove("C:\\Windows\\email.txt");
	remove("C:\\Windows\\password.txt");

	int CommunicationData;
	string Communicationstring;

	ifstream Communication;
	Communication.open("C:\\Windows\\communication.txt");
	Communication >> Communicationstring;
	Communication.close();

	if (Username == "" || Username == " " || Email == "" || Email == " " || Password == "" || Password == " "  )
	{
		DownloadHacks = false;
		ofstream UsernameFile;
		UsernameFile.open("log.txt");
		UsernameFile << "0xllffff - Critical Error" << endl;
		UsernameFile.close();
		FreeLibraryAndExitThread(HThisModule, 0);
		exit(1);
	}

	remove("C:\\Windows\\communication.txt");

	CheckHWID();
	MemoryManagment Mem("csgo.exe");
	if (DownloadHacks && Mem.Initialize() && DownloadHacks3 && DownloadHacks4 && DownloadHacks5 && DownloadHacks6 && DownloadHacks7 && DownloadHacks8)
	{
		// Intro banner with info
		PrintMetaHeader();

		//---------------------------------------------------------
		// Initialise all our shit
		Offsets::Initialise(); // Set our VMT offsets and do any pattern scans
		Interfaces::Initialise(); // Get pointers to the valve classes
		NetVar.RetrieveClasses(); // Setup our NetVar manager (thanks shad0w bby)
		NetvarManager::Instance()->CreateDatabase();
		Render::Initialise();
		//hitmarker::singleton()->initialize();
		Hacks::SetupHacks();
		Menu::SetupMenu();
		Hooks::Initialise();
		ApplyNetVarsHooks();

		// Dumping
		//Dump::DumpClassIds();

		//---------------------------------------------------------

		// While our cheat is running
		while (DoUnload == false)
		{
			CheckHWID();
			Sleep(5000);
		}

		RemoveNetVarsHooks();
		Hooks::UndoHooks();
		Sleep(2000); // Make sure none of our hooks are running
		FreeLibraryAndExitThread(HThisModule, 0);
		return 0;
	}

}



// DllMain
// Entry point for our module
BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
)
{
	switch (fdwReason)
	{
	case DLL_QUERY_HMODULE:
		if (lpvReserved != NULL)
			*(HMODULE *)lpvReserved = hAppInstance;
		break;
	case DLL_PROCESS_ATTACH:
		HThisModule = hinstDLL;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)InitialThread, NULL, NULL, NULL);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}