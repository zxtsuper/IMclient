#include "imconn.h"

static CImConn* FindImConn(ConnMap_t* imconn_map, net_handle_t handle)
{
	CImConn* pConn = NULL;
	ConnMap_t::iterator iter = imconn_map->find(handle);
	if(imconn_map->end() != iter) {
		pConn = iter->second;
		pConn->AddRef();
	}

	return pConn;
}

void imconn_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	NOTUSED_ARG(handle);
	NOTUSED_ARG(pParam);

	if(!callback_data)
		return;

	ConnMap_t* conn_map = (ConnMap_t*)callback_data;
	CImConn* pConn = FindImConn(conn_map,handle);
	if(!pConn)
		return;

	switch(msg) {
	case NETLIB_MSG_CONFIRM:
		pConn->OnConfirm();
		break;
	case NETLIB_MSG_READ:
		pConn->OnRead();
		break;
	case NETLIB_MSG_WRITE:
		pConn->OnWrite();
		break;
	case NETLIB_MSG_CLOSE:
		pConn->OnClose();
		break;
	default:
		break;
	}

	pConn->ReleaseRef();
}

CImConn::CImConn()
{
	m_busy = false;
	m_handle = NETLIB_INVALID_HANDLE;
	m_recv_bytes = 0;

	m_last_send_tick = m_last_recv_tick = get_tick_count();
}

CImConn::~CImConn()
{

}

int CImConn::Send(void* data, int len)
{	
	m_last_send_tick = get_tick_count();
	if(m_busy) {
		m_out_buf.Write(data,len);
		return len;
	}

	int offset = 0;
	int remain = len;
	while(0 < remain) {
		int send_size = remain;
		if(NETLIB_MAX_SOCKET_BUF_SIZE < send_size) {
			send_size = NETLIB_MAX_SOCKET_BUF_SIZE;
		}
		int ret = netlib_send(m_handle,(char*)data + offset,send_size);

		if(ret <= 0) {
			ret = 0;
			break;
		}

		offset += ret;
		remain -= ret;
	}

	if(0 < remain) {
		m_out_buf.Write((char*)data + offset,remain);
		m_busy = true;
	} else {
		OnWriteCompelete();
	}

	return len;
}

void CImConn::OnRead()
{
	for(;;) {
		uint32_t free_buf_len = m_in_buf.GetAllocSize() - m_in_buf.GetWriteOffset();
		if(free_buf_len < READ_BUF_SIZE) {
			m_in_buf.Extend(READ_BUF_SIZE);
		}

		int ret = netlib_recv(m_handle, m_in_buf.GetBuffer() + m_in_buf.GetWriteOffset(), READ_BUF_SIZE);
		if(ret <= 0)
			break;

		m_recv_bytes += ret;
		m_in_buf.IncWriteOffset(ret);

		m_last_recv_tick = get_tick_count();
	}

	//Handle data & Dispatch
//	printf("CImConn: (%d)OnRead(%s:%d).\n",m_handle,m_in_buf.GetBuffer(),m_in_buf.GetWriteOffset());
	do {
		const char* pt = (const char*)m_in_buf.GetBuffer();
		if(0 == strncmp(pt, "@@@", 3)) {
			break;
		}

		const char* pb = strstr(pt, "@@@");
		if(NULL == pb) {
			printf("CImConn: OnRead -no @@@ is founded, cast the data, then returned.\n");
			m_in_buf.Read(NULL, m_in_buf.GetWriteOffset());
			return;
		}

		int nlen = pb - pt;
		if(0 < nlen) {
			printf("CImConn: OnRead -move the packet to the begin. rubbish-len(%d).\n", nlen);
			m_in_buf.Read(NULL, nlen);
		}
	} while(1);


	CImPdu* pPdu = NULL;
	try {
		while( NULL != (pPdu = CImPdu::ReadPdu(m_in_buf.GetBuffer(),m_in_buf.GetWriteOffset()) ) ) {
			uint32_t pdu_len = pPdu->GetLength();
			pPdu->AppendNullEnd();
			HandlePdu(pPdu);

			m_in_buf.Read(NULL,pdu_len);
			m_in_buf.clear();//2017.6.21 add by wang
			delete pPdu;
			pPdu = NULL;
			//printf("CImConn: delete pdu length(%d),buf-length(%d).\n",pdu_len,m_in_buf.GetWriteOffset());
		}
		
		//protect myself
		if(READ_BUF_SIZE <= m_in_buf.GetWriteOffset()) {
			//too much rubbish data
			m_in_buf.Read(NULL,m_in_buf.GetWriteOffset());
			m_in_buf.clear();//2017.6.21 add by wang
		}
	} catch(...) {
		//log
		if(pPdu) {
			delete pPdu;
			pPdu = NULL;
		}

		OnClose();
	}
}

void CImConn::OnWrite()
{
	if(!m_busy)
		return;

	while(0 < m_out_buf.GetWriteOffset()) {
		int send_size = m_out_buf.GetWriteOffset();
		if(NETLIB_MAX_SOCKET_BUF_SIZE < send_size) {
			send_size = NETLIB_MAX_SOCKET_BUF_SIZE;
		}

		int ret = netlib_send(m_handle,m_out_buf.GetBuffer(),send_size);
		if(ret <= 0) {
			ret = 0;
			break;
		}

		m_out_buf.Read(NULL,ret);
	}

	if(m_out_buf.GetWriteOffset() == 0) {
		m_busy = false;
	}
	//log
}












