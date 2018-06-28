#ifndef __SLOG_API_H__
#define __SLOG_API_H__

#include <stdio.h>

#define WATCH_DELAY_TIME	10 * 1000

class CSLogObject;


class CSLog
{
public:
	CSLog(const char* module_name,int delay = WATCH_DELAY_TIME);
	virtual ~CSLog();

	void Trace(const char* format, ...);
	void Debug(const char* format, ...);
	void Info(const char* format, ...);
	void Warn(const char* format, ...);
	void Error(const char* format, ...);
	void Fatal(const char* format, ...);

private:
	CSLogObject* m_log;
};

#endif
