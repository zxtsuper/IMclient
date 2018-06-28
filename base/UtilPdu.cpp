#include "UtilPdu.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

CSimpleBuffer::CSimpleBuffer()
{
	m_buffer = NULL;
	m_alloc_size = 0;
	m_write_offset = 0;
}

CSimpleBuffer::~CSimpleBuffer()
{
	//printf("CSimpleBuffer deconstroctor\n");
	m_alloc_size = 0;
	m_write_offset = 0;
	if(m_buffer) {
		free(m_buffer);
		m_buffer = NULL;
	}
}
void CSimpleBuffer::clear()//2017.6.21 add by wang
{
	//printf("CSimpleBuffer deconstroctor\n");
	m_alloc_size = 0;
	m_write_offset = 0;
	if(m_buffer) {
		free(m_buffer);
		m_buffer = NULL;
	}
}

void CSimpleBuffer::Extend(uint32_t len)
{
	//add by 2016/08/24-13:51
	uint32_t ntmpsize = m_write_offset + len;
	ntmpsize += ntmpsize >> 2;
	if((ntmpsize <= 0) ||((0 < ntmpsize) &&(ntmpsize <= m_alloc_size))) {
		return;
	}
	m_alloc_size = ntmpsize;
	uchar_t* new_buf = (uchar_t*)realloc(m_buffer,m_alloc_size);
	m_buffer = new_buf;
	//printf("CSimpleBuffer::Extend offset=%d, Extend=%d, allocsize=%d\n",
	//	 m_write_offset, len, m_alloc_size);
	//original,if new-size is less-than old-size, realloc would raise exception (invalid next size).
//	m_alloc_size = m_write_offset + len;
//	m_alloc_size += m_alloc_size >> 2; //increase by 1/4 allocate size
//	uchar_t* new_buf = (uchar_t*)realloc(m_buffer,m_alloc_size);
//	m_buffer = new_buf;
}

//append data to buffer
uint32_t CSimpleBuffer::Write(void* buf, uint32_t len)
{
	if(m_alloc_size < m_write_offset + len) {
		Extend(len);
	}

	if(buf) {
		memcpy(m_buffer + m_write_offset,buf,len);
	}

	m_write_offset += len;
	this->AppendNullEnd();	//add by 2016/07/13-15:25
	return len;
}

void CSimpleBuffer::AppendNullEnd()
{
	m_buffer[m_write_offset] = '\0';
}

//read data at the begin of buffer,then move the unread data to front
uint32_t CSimpleBuffer::Read(void* buf, uint32_t len)
{
	if(m_write_offset < len) {
		len = m_write_offset;
	}

	if(buf) {
		memcpy(buf,m_buffer,len);
	}

	m_write_offset -= len;
	memmove(m_buffer,m_buffer+len,m_write_offset);
	this->AppendNullEnd();	//add by 2016/07/13-15:25
	return len;
}



