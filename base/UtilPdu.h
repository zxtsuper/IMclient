#ifndef __UTILPDU_H__
#define __UTILPDU_H__

#include "ostype.h"
#include <set>
#include <map>
#include <list>
#include <string>
using namespace std;

#ifdef _WIN32

#else
	#define DLL_MODIFIER
#endif


class DLL_MODIFIER CSimpleBuffer
{
public:
	CSimpleBuffer();
	~CSimpleBuffer();
	void clear();//2017.6.21 add by wang

	uchar_t* GetBuffer() { return m_buffer; }
	uint32_t GetAllocSize() { return m_alloc_size; }
	uint32_t GetWriteOffset() { return m_write_offset; }
	void IncWriteOffset(uint32_t len) { m_write_offset += len; }

	void Extend(uint32_t len);
	//append data to buffer
	uint32_t Write(void* buf, uint32_t len);
	void AppendNullEnd();
	//read data at the begin of buffer,then move the unread data to front
	uint32_t Read(void* buf, uint32_t len);

private:
	uchar_t* m_buffer;
	uint32_t m_alloc_size;
	uint32_t m_write_offset;
};

#endif
