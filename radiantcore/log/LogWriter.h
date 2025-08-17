#pragma once

#include <set>
#include <map>
#include <memory>
#include "ilogwriter.h"

#include "LogStream.h"

namespace applog
{

class LogWriter :
	public ILogWriter
{
private:
	// The set of unique log devices
	typedef std::set<ILogDevice*> LogDevices;
	LogDevices _devices;

	std::map<LogLevel, std::unique_ptr<LogStream>> _streams;

public:
	LogWriter();

	/**
	 * greebo: Writes the given buffer p with the given length to the
	 *         various output devices (i.e. Console and Log file).
	 */
	void write(const char* p, std::size_t length, LogLevel level) override;

	std::ostream& getLogStream(LogLevel level) override;
	std::mutex& getStreamLock() override;

	/**
	 * greebo: Use these methods to attach/detach a log device from the
	 *         writer class. After attaching a device, all log output
	 *         will be written to it.
	 */
	void attach(ILogDevice* device) override;
	void detach(ILogDevice* device) override;

	// Contains the static singleton instance of this writer
	static LogWriter& Instance();
};

} // namespace applog
