#pragma once

#include <iostream>
#include <sstream>

// Minimal severity-filtered logging.
//
// The default level is Trace, i.e. everything is printed exactly as before.
// Lower it (from the Info window, or by changing g_logLevel) to quiet the
// per-frame render traces without deleting them.
enum class LogLevel
{
	Error = 0,
	Warn = 1,
	Info = 2,
	Trace = 3
};

inline LogLevel g_logLevel = LogLevel::Trace;

// Buffers a single message and writes it on destruction, so a filtered-out
// message costs nothing but the formatting call.
class LogStream
{
public:
	LogStream(LogLevel lvl, std::ostream& os) : enabled(lvl <= g_logLevel), out(os) {}
	~LogStream() { if (enabled) out << buf.str(); }
	LogStream(const LogStream&) = delete;
	LogStream& operator=(const LogStream&) = delete;

	template <typename T>
	LogStream& operator<<(const T& value)
	{
		if (enabled) buf << value;
		return *this;
	}

private:
	bool enabled;
	std::ostream& out;
	std::ostringstream buf;
};

inline LogStream logOut(LogLevel lvl) { return LogStream(lvl, std::cout); }
inline LogStream logErr(LogLevel lvl) { return LogStream(lvl, std::cerr); }
