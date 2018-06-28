#include "ConfigFileReader.h"


CConfigFileReader::CConfigFileReader(const char* filename)
{
	m_load_ok = false;
	_LoadFile(filename);
}

CConfigFileReader::~CConfigFileReader()
{

}

char* CConfigFileReader::GetConfigName(const char* name)
{
	if(!m_load_ok)
		return NULL;

	char* value = NULL;
	map<string,string>::iterator it = m_config_map.find(name);
	if(m_config_map.end() != it) {
		value = (char*)it->second.c_str();
	}

	return value;
}

int CConfigFileReader::SetConfigValue(const char* name, const char* value)
{
	if(!m_load_ok)
		return -1;

	map<string,string>::iterator it = m_config_map.find(name);
	if(m_config_map.end() != it) {
		it->second = value;
	} else {
		m_config_map.insert(make_pair<string,string>(name,value));
	}

	return _WriteFile();
}

int CConfigFileReader::_WriteFile(const char* filename)
{
	FILE* fp = NULL;
	if(NULL == filename) {
		fp = fopen(m_config_file.c_str(),"w");
	} else {
		fp = fopen(filename,"w");
	}

	if(NULL == fp) {
		return -1;
	}

	char szPair[128] = {0};
	map<string,string>::iterator it = m_config_map.begin();
	for(; m_config_map.end() != it ; ++it) {
		memset(szPair,0,sizeof(szPair));
		snprintf(szPair,sizeof(szPair),"%s=%s\n",it->first.c_str(),it->second.c_str());
		uint32_t ret = fwrite(szPair,strlen(szPair),1,fp);
		if(1 != ret) {
			fclose(fp);
			return -1;
		}
	}

	fclose(fp);
	return 0;
}

void CConfigFileReader::_LoadFile(const char* filename)
{
	m_config_file.clear();
	m_config_file.append(filename);

	FILE* fp = fopen(filename,"r");
	if(!fp) {
		//log
		return;
	}

	char buf[256] = {0};
	for(;;) {
		char* p = fgets(buf,256,fp);
		if(!p)
			break;

		size_t len = strlen(buf);
		if('\n' == buf[len - 1])
			buf[len - 1] = 0;

		char* ch = strchr(buf,'#');
		if(ch)
			*ch = 0;

		if(0 == strlen(buf))
			continue;

		_ParseLine(buf);
	}

	fclose(fp);
	m_load_ok = true;
}

void CConfigFileReader::_ParseLine(char* line)
{
	char* p = strchr(line,'=');
	if(NULL == p)
		return;

	//printf("config:line(%s)\n",line);
	*p = 0;
	char* key = _TrimSpace(line);
	char* value = _TrimSpace(p + 1);
	if(key && value) {
		//printf("config:insert(%s:%s)\n",key,value);
		m_config_map.insert(make_pair<string,string>(key,value));
	}
}

char* CConfigFileReader::_TrimSpace(char* name)
{
	char* start_pos = name;
	while((' ' == *start_pos) || ('\t' == *start_pos)) {
		start_pos++;
	}

	if(0 == strlen(start_pos))
		return NULL;

	char* end_pos = name + strlen(name) - 1;
	while((' ' == *end_pos) || ('\t' == *end_pos)) {
		*end_pos = 0;
		end_pos--;
	}

	int len = (int)(end_pos - start_pos) + 1;
	if(len <= 0)
		return NULL;

	return start_pos;
}
