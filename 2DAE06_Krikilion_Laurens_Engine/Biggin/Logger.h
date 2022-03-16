#pragma once
#include <chrono>
#include <source_location>
#include "Singleton.h"

enum class LogLevel
{
	Debug = 0x1,
	Info = 0x2,
	Warning = 0x4,
	Error = 0x8,
	ToDo = 0x16
};

class Logger final : public biggin::Singleton<Logger>
{

public:
	~Logger() override = default;
	Logger(const Logger& other) = delete;
	Logger(Logger&& other) noexcept = delete;
	Logger& operator=(const Logger& other) = delete;
	Logger& operator=(Logger&& other) noexcept = delete;

	void Initialize();

	void Log(LogLevel level, const std::string& msg, const std::source_location location = std::source_location::current());
	void LogDebug(const std::string& msg, const std::source_location location = std::source_location::current());
	void LogInfo(const std::string& msg, const std::source_location location = std::source_location::current());
	void LogWarning(const std::string& msg, const std::source_location location = std::source_location::current());
	void LogError(const std::string& msg, const std::source_location location = std::source_location::current());
	void LogToDo(const std::string& source, const std::source_location location = std::source_location::current());

	static void ClearConsole();

	void BreakOnLog(LogLevel level, bool doBreak) { doBreak ? m_BreakBitField |= static_cast<int>(level) : m_BreakBitField &= ~static_cast<int>(level); }

	void AppendTimestamp(bool append) { m_AppendTimestamp = append; }

	static std::string SimplifyPath(const std::string& str, size_t dirSize = 0);

private:
	Logger() = default;

	std::ostream* m_os{};
	std::ofstream* m_FileOutStream{};

	double m_PcFreq{};
	static constexpr int MAX_PERFORMANCE_TIMERS{ 10 };
	__int64 m_PerformanceTimerArr[MAX_PERFORMANCE_TIMERS]{};
	std::chrono::high_resolution_clock::time_point m_TimePoints[MAX_PERFORMANCE_TIMERS]{};

	int m_BreakBitField{ static_cast<int>(LogLevel::Error) };
	bool m_AppendTimestamp{ false };

	HANDLE m_ConsoleHandle{ nullptr };

	friend Singleton<Logger>;
};

