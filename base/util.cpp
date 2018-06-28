#include "util.h"
#include <sstream>
using namespace std;

long g_nloglevel = 3;
CSLog g_srvlog = CSLog(LOG_MODULE_SRV);

CRefObject::CRefObject()
{
	m_lock = NULL;
	m_refCount = 1;
}

CRefObject::~CRefObject()
{

}

void CRefObject::AddRef()
{
	if(m_lock) {
		m_lock->lock();
		m_refCount++;
		m_lock->unlock();
	} else {
		m_refCount++;
	}
}

void CRefObject::ReleaseRef()
{
	if(m_lock) {
		m_lock->lock();
		m_refCount--;
		if(0 == m_refCount) {
			delete this;
			return;
		}
		m_lock->unlock();
	} else {
		m_refCount--;
		if(0 == m_refCount) {
			delete this;
			//return;
		}
	}
}

void set_log_level(long nlevel)
{
	g_nloglevel = nlevel;
}

uint64_t get_tick_count()
{
#ifdef _WIN32

#else
	struct timeval tval;
	uint64_t ret_tick;

	gettimeofday(&tval,NULL);

	ret_tick = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
	return ret_tick;
#endif
}

void util_sleep(uint32_t millisecond)
{
#ifdef _WIN32
#else
	usleep(millisecond * 1000);
#endif
}


CStrExplode::CStrExplode(char* str, char seperator)
{
	m_item_cnt = 1;
	char* pos = str;
	while(*pos) {
		if(*pos == seperator) {
			m_item_cnt++;
		}

		pos++;
	}

	m_item_list = new char*[m_item_cnt];

	int idx = 0;
	char* start = pos = str;
	while(*pos) {
		if((pos != start) && (*pos == seperator)) {
			uint32_t len = pos - start;
			m_item_list[idx] = new char[len + 1];
			strncpy(m_item_list[idx],start,len);
			m_item_list[idx][len] = '\0';
			idx++;

			start = pos + 1;
		}

		pos++;
	}

	uint32_t len = pos - start;
	if(0 != len) {
		m_item_list[idx] = new char[len + 1];
		strncpy(m_item_list[idx],start,len);
		m_item_list[idx][len] = '\0';
	}
}

CStrExplode::~CStrExplode()
{
	for(uint32_t i = 0 ; i < m_item_cnt; i++) {
		delete [] m_item_list[i];
	}
	
	delete [] m_item_list;
}




void writePid()
{
	uint32_t curPid;
#ifdef _WIN32
#else
	curPid = (uint32_t)getpid();
#endif
	FILE* f = fopen("server.pid","w");
	assert(f);
	char szPid[32] = {0};
	snprintf(szPid,sizeof(szPid),"%d",curPid);
	fwrite(szPid,strlen(szPid),1,f);
	fclose(f);
}
