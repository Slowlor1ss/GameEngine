#include "BigginPCH.h"
#include "Logger.h"
#include <fcntl.h>
#include <iomanip>
#include <comdef.h>

#include "ConsoleColors.h"
#include "StringUtils.hpp"

void Logger::Initialize()
{
	m_os = &std::cout;

	//#if defined(DEBUG) | defined(_DEBUG)
	if (AllocConsole()) //if no console add one
	{
		// Redirect the CRT standard input, output, and error handles to the console
		FILE* pCout;
		freopen_s(&pCout, "CONIN$", "r", stdin);
		freopen_s(&pCout, "CONOUT$", "w", stdout);
		freopen_s(&pCout, "CONOUT$", "w", stderr);

		//Clear the error state for each of the C++ standard stream objects. We need to do this, as
		//attempts to access the standard streams before they refer to a valid target will cause the
		//iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
		//to always occur during startup regardless of whether anything has been read from or written to
		//the console or not.
		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
		std::wcin.clear();
		std::cin.clear();
		std::cin.clear();

		//Set ConsoleHandle
		m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		//Disable Close-Button (closing console would cause memory leaks)
		const HWND hwnd = GetConsoleWindow();
		if (hwnd != NULL)
		{
			HMENU hMenu = GetSystemMenu(hwnd, FALSE);
			if (hMenu != NULL) DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
		}
	}
	else //Already has console
	{
		//Set ConsoleHandle
		m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		//Disable Close-Button (closing console would cause memory leaks)
		const HWND hwnd = GetConsoleWindow();
		if (hwnd != NULL)
		{
			HMENU hMenu = GetSystemMenu(hwnd, FALSE);
			if (hMenu != NULL) DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
		}
	}
	//#endif
}

void Logger::LogDebug(const std::string& msg, const std::source_location location)
{
	Log(LogLevel::Debug, msg, location);
}

void Logger::LogInfo(const std::string& msg, const std::source_location location)
{
	Log(LogLevel::Info, msg, location);
}

void Logger::LogWarning(const std::string& msg, const std::source_location location)
{
	Log(LogLevel::Warning, msg, location);
}

void Logger::LogError(const std::string& msg, const std::source_location location)
{
	Log(LogLevel::Error, msg, location);
}

void Logger::LogToDo(const std::string& source, const std::source_location location)
{
	Log(LogLevel::ToDo, source, location);
}

void Logger::ClearConsole()
{
	std::system("cls");
}

void Logger::Log(LogLevel level, const std::string& msg, const std::source_location location)
{
#ifdef NDEBUG
	if (level == LogLevel::Debug || level == LogLevel::ToDo)
	{
		return;
	}
#endif

	std::stringstream stream;

	if (m_AppendTimestamp)
	{
		SYSTEMTIME st;
		GetSystemTime(&st);
		stream << "[" << st.wDay << "-" << st.wMonth << "-" << st.wYear << " - ";
		stream << st.wHour << ":" << st.wMinute << ":" << st.wSecond << ":" << st.wMilliseconds << "]";
	}

	stream << "[" << SimplifyPath(location.file_name(), 2) << "(" << std::to_string(location.line()) << ")" << "] ";
	//stream << "[" << SimplifyPath(__FILE__, 2) << "(" << std::to_string(__LINE__) << ")" << "] ";

	switch (level)
	{
	case LogLevel::Debug:
		stream << FMAG("([DEBUG])");
		break;
	case LogLevel::Info:
		stream << FLGRY("[INFO]");
		break;
	case LogLevel::Warning:
		stream << FYEL("[WARNING]");
		break;
	case LogLevel::Error:
		stream << FRED("[ERROR]");
		break;
	case LogLevel::ToDo:
		stream << FCYN("[FIX-ME]");
		break;
	default: ;
	}

	stream << ": " << msg << '\n';

	if (m_os)
	{
		(*m_os) << stream.str();
		m_os->flush();
	}

	//if error, break
	if (level == LogLevel::Error)
	{
		MessageBox(0, msg.c_str(), "ERROR", MB_OK | MB_ICONERROR);
	}

	if ((m_BreakBitField & static_cast<int>(level)) == static_cast<int>(level))
	{
#if _DEBUG
		__debugbreak();
#else
		exit(-1);
#endif
	}

}

//dirSize dedicates the size of the path
std::string Logger::SimplifyPath(const std::string& str, size_t dirSize)
{
	std::string s = str;
	std::string drive;
	if (s.starts_with("local://")) {
		drive = "local://";
		s = s.substr(8, s.length());
	}
	else if (s.starts_with("res://")) {
		drive = "res://";
		s = s.substr(6, s.length());
	}
	else if (s.starts_with("user://")) {
		drive = "user://";
		s = s.substr(7, s.length());
	}
	else if (s.starts_with("//") || s.starts_with("\\\\")) {
		drive = s.substr(0, 2);
		s = s.substr(2, s.length() - 2);
	}
	else if (s.starts_with("/") || s.starts_with("\\")) {
		drive = s.substr(0, 1);
		s = s.substr(1, s.length() - 1);
	}
	else {
		size_t p = s.find(":/");
		if (p == std::string::npos) {
			p = s.find(":\\");
		}
		if (p != std::string::npos && p < s.find("/")) {
			drive = s.substr(0, p + 2);
			s = s.substr(p + 2, s.length());
		}
	}

	s = Replace(s, "\\", "/");
	while (true) { //In case of using 2 or more slash
		std::string compare = Replace(s, "//", "/");
		if (s == compare) {
			break;
		}
		else {
			s = compare;
		}
	}
	std::vector<std::string> dirs = Split(s, "/", false);

	for (auto i = dirs.begin(); i != dirs.end();)
	{
		if (*i == ".")
			i = dirs.erase(i);
		else if (*i == "..")
		{
			if (i == dirs.begin())
			{
				i = dirs.erase(i);
			}
			else
			{
				i = dirs.erase(i - 1);
				i = dirs.erase(i);
			}
		}
		else
			++i;
	}

	s = "";

	if (dirSize == 0 || dirSize > dirs.size())
		dirSize = 0;
	else
	{
		dirSize = dirs.size() - dirSize;
		s += "...";
	}

	for (size_t i = dirSize; i < dirs.size(); i++) {
		if (i > 0) {
			s += "/";
		}
		s += dirs[i];
	}

	return drive + s;
}
