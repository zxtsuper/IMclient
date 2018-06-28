#ifndef __IMPDUBASE_H__
#define __IMPDUBASE_H__

#include "UtilPdu.h"


#ifdef _WIN32

#else
#define DLL_MODIFIER
#endif

typedef struct {
	uint32_t	length;		//the whole pdu length
	uint32_t	head_len;	//
	uint16_t	service_id;	//
	uint16_t	command_id;	//
	uint16_t	seq_num;	//packet sequence number
	uint16_t	socket_handle;	//add
	uint32_t	user_id;	//add
}PduHeader_t;

class DLL_MODIFIER CImPdu
{
public:
	CImPdu();
	virtual ~CImPdu();

	uchar_t* GetBuffer();
	uint32_t GetLength();
	uchar_t* GetBodyData();
	uint32_t GetBodyLength();

	uint16_t GetServiceId() { return m_pdu_header.service_id; }
	uint16_t GetCommandId() { return m_pdu_header.command_id; }
	uint16_t GetSeqNum() { return m_pdu_header.seq_num; }
	uint16_t GetHandle() { return m_pdu_header.socket_handle; }
	uint32_t GetUserId() { return m_pdu_header.user_id; }

	void SetServiceId(uint16_t service_id);
	void SetCommandId(uint16_t command_id);
	void SetHeadLeng(uint32_t head_len);

	static bool IsPduAvailable(uchar_t* buf, uint32_t len, uint32_t data_len);
	static CImPdu* ReadPdu(uchar_t* buf, uint32_t len);
	void Write(uchar_t* buf, uint32_t len) { m_buf.Write((void*)buf,len); }
	void AppendNullEnd() { m_buf.AppendNullEnd(); }

protected:
	CSimpleBuffer	m_buf;
	PduHeader_t	m_pdu_header;
};

#endif

