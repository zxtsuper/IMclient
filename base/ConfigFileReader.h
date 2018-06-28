#ifndef __CONFIGFILEREADER_H__
#define __CONFIGFILEREADER_H__

#include "util.h"

class CConfigFileReader
{
public:
	CConfigFileReader(const char* filename);
	~CConfigFileReader();

public:
	char* GetConfigName(const char* name);
	int SetConfigValue(const char* name, const char* value);

private:
	void _LoadFile(const char* filename);
	void _ParseLine(char* line);
	char* _TrimSpace(char* name);
	int _WriteFile(const char* filename = NULL);

private:
	bool m_load_ok;
	map<string,string> m_config_map;
	string m_config_file;
};

#endif
