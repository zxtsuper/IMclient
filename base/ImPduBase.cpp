#include "ImPduBase.h"
#include <string.h>
#include <stdio.h>

std::string findcommand(const std::string& scontent,const std::string& skey)
{
	std::string stmp("");
	int ncmdpos = scontent.find(skey);
	if(ncmdpos != std::string::npos) {
		int nendpos = scontent.find(';',ncmdpos+1);
		if(nendpos != std::string::npos) {
			int nlength = nendpos - ncmdpos - skey.length();
			stmp = scontent.substr(ncmdpos+skey.length(),nlength);
		}
	}

	return stmp;
}

CImPdu::CImPdu()
{
	m_pdu_header.service_id = 0;
	m_pdu_header.command_id = 0;
	m_pdu_header.seq_num = 0;
}

CImPdu::~CImPdu()
{

}


bool CImPdu::IsPduAvailable(uchar_t* buf, uint32_t len, uint32_t data_len)
{
	return false;
}

CImPdu* CImPdu::ReadPdu(uchar_t* buf, uint32_t len)
{
	//printf("ImPduBase::ReadPdu buf=%s, len=%d\n", buf, len);
	if(len < 9) {
		//printf("len is less than 9\n");
		return NULL;
	}	

	CImPdu* pPdu = NULL;
	char* pHeadBegin = strstr((char*)buf, "@@@");
	if(NULL == pHeadBegin) {
		printf("*** buf=%s *\n", (char*)buf);
		printf("pHeadBegin is Null\n");
		return pPdu;
	}
	char* pHeadEnd = strstr((char*)pHeadBegin, "\r\n");
	if(NULL == pHeadEnd) {
		printf("pHeadEnd is Null\n");
		return pPdu;
	}
	string shead(pHeadBegin,pHeadEnd-pHeadBegin);
	string slen = findcommand(shead, "l:");
	string scmd = findcommand(shead, "c:");
	string shand= findcommand(shead, "h:");
	string sid  = findcommand(shead, "i:");
	uint32_t data_len = atoi(slen.c_str());
	if(data_len <= (len - shead.length() - 2))
	 {		
		pPdu = new CImPdu();
		pPdu->m_pdu_header.command_id = atoi(scmd.c_str());
		pPdu->m_pdu_header.head_len = shead.length() + 2;
		pPdu->m_pdu_header.socket_handle = 0;
		pPdu->m_pdu_header.user_id = 0;
		if(false == shand.empty())
			pPdu->m_pdu_header.socket_handle = atoi(shand.c_str());
		if(false == sid.empty())
			pPdu->m_pdu_header.user_id = atoi(sid.c_str());
		pPdu->Write(buf, (shead.length() + 2 + data_len));
	} 
	return pPdu;
}

void CImPdu::SetServiceId(uint16_t service_id)
{

}

void CImPdu::SetCommandId(uint16_t command_id)
{

}

void CImPdu::SetHeadLeng(uint32_t head_len)
{

}

uchar_t* CImPdu::GetBuffer()
{
	return m_buf.GetBuffer();
}

uint32_t CImPdu::GetLength()
{
	return m_buf.GetWriteOffset();
}

uchar_t* CImPdu::GetBodyData()
{
	return m_buf.GetBuffer() + m_pdu_header.head_len;
}

uint32_t CImPdu::GetBodyLength()
{
	uint32_t body_length = 0;
	body_length = m_buf.GetWriteOffset() - m_pdu_header.head_len;
	return body_length;
}


