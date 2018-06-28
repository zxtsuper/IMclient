#ifndef __UTIL_H__
#define __UTIL_H__

#define _CRT_SECURE_NO_DEPRECATE

#include "ostype.h"
#include "UtilPdu.h"
#include "Lock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slog/slog_api.h"
#ifndef _WIN32
#include <strings.h>
#endif

#include <sys/stat.h>
#include <assert.h>

#ifdef _WIN32

#else
#include <stdarg.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#endif

#define NOTUSED_ARG(v) ((void)v)

class CRefObject
{
public:
	CRefObject();
	virtual ~CRefObject();

	void SetLock(CLock* lock) {m_lock = lock;}
	void AddRef();
	void ReleaseRef();
private:
	int	m_refCount;
	CLock*	m_lock;
};

//#define LOG_MODULE_SRV	"SRV"
#define LOG_MODULE_SRV	"default"	//add 2016/11/30

extern long g_nloglevel;
extern CSLog g_srvlog;

#define __FILENAME__ (strrchr(__FILE__,'/') ? (strrchr(__FILE__,'/') + 1) : __FILE__)
#if defined(_WIN32) || defined(_WIN64)
#define log(fmt,...) g_srvlog.Info("<%s>\t<%d>\t<%s>,"fmt,__FILENAME__,__LINE__,__FUNCTION__,##__VA_ARGS__)
#else
//#define log(fmt,args...) g_srvlog.Info("<%s>|<%d>|<%s>,"fmt,__FILENAME__,__LINE__,__FUNCTION__,##args)
#define logtrace(fmt,args...) g_srvlog.Trace("<%s>|<%d>|<%s>,"fmt,__FILENAME__,__LINE__,__FUNCTION__,##args)
#define logdebug(fmt,args...) g_srvlog.Debug("<%s>|<%d>|<%s>,"fmt,__FILENAME__,__LINE__,__FUNCTION__,##args)
#define loginfo(fmt,args...) g_srvlog.Info("<%s>|<%d>|<%s>,"fmt,__FILENAME__,__LINE__,__FUNCTION__,##args)
#define logwarn(fmt,args...) g_srvlog.Warn("<%s>|<%d>|<%s>,"fmt,__FILENAME__,__LINE__,__FUNCTION__,##args)
#define logerror(fmt,args...) g_srvlog.Error("<%s>|<%d>|<%s>,"fmt,__FILENAME__,__LINE__,__FUNCTION__,##args)
#define logfatal(fmt,args...) g_srvlog.Fatal("<%s>|<%d>|<%s>,"fmt,__FILENAME__,__LINE__,__FUNCTION__,##args)
#define log(fmt,args...) do{if(1==g_nloglevel){g_srvlog.Trace("<%s>|<%d>|<%s>,"fmt,__FILENAME__,__LINE__,__FUNCTION__,##args);}\
				else if(2==g_nloglevel){g_srvlog.Debug("<%s>|<%d>|<%s>,"fmt,__FILENAME__,__LINE__,__FUNCTION__,##args);}\
				else if(3==g_nloglevel){g_srvlog.Info("<%s>|<%d>|<%s>,"fmt,__FILENAME__,__LINE__,__FUNCTION__,##args);}\
				else if(4==g_nloglevel){g_srvlog.Warn("<%s>|<%d>|<%s>,"fmt,__FILENAME__,__LINE__,__FUNCTION__,##args);}\
				else if(5==g_nloglevel){g_srvlog.Error("<%s>|<%d>|<%s>,"fmt,__FILENAME__,__LINE__,__FUNCTION__,##args);}\
				else if(6==g_nloglevel){g_srvlog.Fatal("<%s>|<%d>|<%s>,"fmt,__FILENAME__,__LINE__,__FUNCTION__,##args);}}while(0);
#endif


uint64_t get_tick_count();
void util_sleep(uint32_t millisecond);
void set_log_level(long nlevel);

class CStrExplode
{
public:
	CStrExplode(char* str, char seperator);
	~CStrExplode();

public:
	uint32_t GetItemCnt() { return m_item_cnt; }
	char* GetItem(uint32_t idx) { return m_item_list[idx]; }

private:
	uint32_t	m_item_cnt;
	char**		m_item_list;
};


void writePid();


#endif
