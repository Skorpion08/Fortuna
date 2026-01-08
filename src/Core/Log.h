#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <fmt/core.h>
#include <filesystem>

#ifndef LOG_LEVEL
#define LOG_LEVEL 0
#endif

// Make everything noexcept

namespace Log
{
enum class Level : int { Info = 0, Warn, Error, Critical };

inline const char* logsPath = "Logs/";

#ifdef LOG_CONSOLE_ENABLED
inline std::mutex consoleMutex;
#endif

#if defined(LOG_FILE_ENABLED) || defined(LOG_CRASH_ENABLED)
inline std::string logTimestamp;
inline std::mutex timestampMutex;
#endif

#ifdef LOG_FILE_ENABLED
inline std::ofstream logFile; // the file stream
inline std::mutex fileMutex;
#endif

#ifdef LOG_CRASH_ENABLED
inline std::ofstream crashLogFile;
inline std::mutex crashFileMutex;
#endif

// Makes sure there's a proper logs directory
inline void EnsureLogsPath()
{
	std::error_code ec;
	if (!std::filesystem::exists(logsPath))
		std::filesystem::create_directories(logsPath, ec);

	if (ec)
	{
		std::cerr << fmt::format("Failed to create logs directory, path: '{}', {}", logsPath, ec.message()) << '\n';
	}
}

inline void InitLogTimestamp()
{
	std::lock_guard<std::mutex> lock(timestampMutex);

	if (!logTimestamp.empty()) { std::cerr << "Tried to init log timestamp again!"; return; }

	auto now = std::chrono::system_clock::now();
	auto t = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
#ifdef _WIN32
	localtime_s(&tm, &t);
#else
	localtime_r(&t, &tm);
#endif

	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y%m%d_%H%M%S");

	logTimestamp = oss.str();
}

inline std::string Timestamp()
{
	using namespace std::chrono;
	auto now = system_clock::now();
	auto t = system_clock::to_time_t(now);
	std::tm tm{};
#ifdef _WIN32
	localtime_s(&tm, &t);
#else
	localtime_r(&t, &tm);
#endif

	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
	return oss.str();
}

inline void InitCrashLog()
{
#ifdef LOG_CRASH_ENABLED

	std::string ts;
	{
		std::lock_guard<std::mutex> lock(timestampMutex);
		ts = logTimestamp;
	}

	if (ts.empty())
		std::cerr << "Log timestamp not set\n";

	std::string filename = fmt::format("{}crash_{}.log", logsPath, ts);

	std::lock_guard<std::mutex> lock(crashFileMutex);

	if (crashLogFile.is_open())
	{
		// Already opened
		crashLogFile << "=== Crash log reopened at " << Timestamp() << " ===\n";
		return;
	}

	EnsureLogsPath();

	crashLogFile.open(filename, std::ios::out | std::ios::app); // append mode
	if (!crashLogFile.is_open())
		std::cerr << "Failed to open crash log file: " << filename << "\n";
	else
		crashLogFile << "=== Crash log started at " << Timestamp() << " ===\n";
#endif
}

// Call this once at app start
inline void Init()
{
#ifdef LOG_FILE_ENABLED
	std::string ts;
	{
		std::lock_guard<std::mutex> lock(timestampMutex);
		ts = logTimestamp;
	}

	if (ts.empty())
		std::cerr << "Log timestamp not set\n";

	std::string filename = fmt::format("{}log_{}.log", logsPath, ts);

	std::lock_guard<std::mutex> lock(fileMutex);

	if (logFile.is_open())
	{
		// Already opened
		logFile << "=== Crash log reopened at " << Timestamp() << " ===\n";
		return;
	}

	EnsureLogsPath();

	logFile.open(filename, std::ios::out | std::ios::app); // append mode
	if (!logFile.is_open())
		std::cerr << "Failed to open log file: " << filename << "\n";
	else
		logFile << "=== Log started at " << Timestamp() << " ===\n";
#endif
}

inline void WriteCrashLogFile(const std::string& message)
{
#ifdef LOG_CRASH_ENABLED
	static std::once_flag crashInitFlag;

	// Ensure the crash log file is created exactly once
	std::call_once(crashInitFlag, []()
		{
			InitCrashLog();
		});

	std::lock_guard<std::mutex> lock(crashFileMutex);
	if (crashLogFile.is_open())
		crashLogFile << message << '\n' << std::flush;
#endif
}

inline void WriteLogFile(const std::string& message)
{
#ifdef LOG_FILE_ENABLED
	std::lock_guard<std::mutex> lock(fileMutex);
	if (logFile.is_open())
		logFile << message << '\n';
#endif
}

inline void WriteConsole(Level level, const std::string& message)
{
#ifdef LOG_CONSOLE_ENABLED
	std::lock_guard<std::mutex> lock(consoleMutex);

	if (static_cast<int>(level) >= static_cast<int>(Level::Error))
		std::cerr << message << "\n";
	else
		std::cout << message << "\n";
#endif
}

template<typename... Args>
inline void Write(Level level, const std::string& fmtStr, Args&&... args)
{
	if (static_cast<int>(level) < LOG_LEVEL)
		return;

	std::string prefix;
	switch (level)
	{
	case Level::Info:     prefix = "[INFO]"; break;
	case Level::Warn:     prefix = "[WARN]"; break;
	case Level::Error:    prefix = "[ERROR]"; break;
	case Level::Critical: prefix = "[CRITICAL]"; break;
	}

	std::string message = fmt::format(fmt::runtime(fmtStr), std::forward<Args>(args)...);
	std::string line = fmt::format("[{}] {} {}", Timestamp(), prefix, message);

	WriteConsole(level, line);

	if (static_cast<int>(level) >= static_cast<int>(Level::Critical))
	{
		WriteCrashLogFile(line);
	}

	// Write to file if open
	WriteLogFile(line);
}

template<typename... Args> inline void Info(const std::string& fmtStr, Args&&... args) { Write(Level::Info, fmtStr, std::forward<Args>(args)...); }
template<typename... Args> inline void Warn(const std::string& fmtStr, Args&&... args) { Write(Level::Warn, fmtStr, std::forward<Args>(args)...); }
template<typename... Args> inline void Error(const std::string& fmtStr, Args&&... args) { Write(Level::Error, fmtStr, std::forward<Args>(args)...); }
template<typename... Args> inline void Critical(const std::string& fmtStr, Args&&... args) { Write(Level::Critical, fmtStr, std::forward<Args>(args)...); }

// Call at shutdown
inline void Shutdown() noexcept
{
#ifdef LOG_FILE_ENABLED
	try
	{
		{
			std::lock_guard<std::mutex> lock(fileMutex);
			if (logFile.is_open())
			{
				std::string ts = Timestamp();
				if (!ts.empty())
					logFile << "=== Log ended at " << ts << " ===\n";
				else
					logFile << "=== Log ended ===\n";
				logFile.close();
			}
		}
	}
	catch (...) {}
#endif

#ifdef LOG_CRASH_ENABLED
	try
	{
		{
			std::lock_guard<std::mutex> lock(crashFileMutex);
			if (crashLogFile.is_open())
			{
				crashLogFile << "=== Crash log ended at " << Timestamp() << " ===\n";
				crashLogFile.close();
			}
		}
	}
	catch (...) {}
#endif
}
}