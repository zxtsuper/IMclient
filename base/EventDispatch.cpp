#include "EventDispatch.h"
#include "BaseSocket.h"




CEventDispatch* CEventDispatch::m_pEventDispatch = NULL;

CEventDispatch* CEventDispatch::Instance()
{
	if(m_pEventDispatch == NULL) {
		m_pEventDispatch = new CEventDispatch();
	}

	return m_pEventDispatch;
}

CEventDispatch::CEventDispatch()
{
	running = false;
#ifdef _WIN32

#elif __APPLE__

#else
	m_epfd = epoll_create(1024);
	if(-1 == m_epfd) {
		//log
	}
#endif
}

CEventDispatch::~CEventDispatch()
{
#ifdef _WIN32

#elif __APPLE__

#else
	if(-1 != m_epfd)
		close(m_epfd);
#endif
}

void CEventDispatch::AddTimer(callback_t callback, void* user_data, uint64_t interval)
{
	list<TimerItem*>::iterator it;
	for(it = m_timer_list.begin(); m_timer_list.end() != it; ++it) {
		TimerItem* pItem = *it;
		if((pItem->callback == callback) && (pItem->user_data == user_data)) {
			pItem->interval = interval;
			pItem->next_tick = get_tick_count() + interval;
			return;
		}
	}

	TimerItem* pItem = new TimerItem;
	pItem->callback = callback;
	pItem->user_data = user_data;
	pItem->interval = interval;
	pItem->next_tick = get_tick_count() + interval;
	m_timer_list.push_back(pItem);
	printf("EventDispatch: AddTimer end.\n");
}

void CEventDispatch::RemoveTimer(callback_t callback, void* user_data)
{
	list<TimerItem*>::iterator it;
	for(it = m_timer_list.begin(); m_timer_list.end() != it; ++it) {
		TimerItem* pItem = *it;
		if((pItem->callback == callback) && (pItem->user_data == user_data)) {
			m_timer_list.erase(it);
			delete pItem;
			return;
		}
	}
}

void CEventDispatch::AddLoop(callback_t callback,void* user_data)
{
	TimerItem* pItem = new TimerItem;
	pItem->callback = callback;
	pItem->user_data = user_data;
	m_loop_list.push_back(pItem);
}

void CEventDispatch::_CheckTimer()
{
	uint64_t curr_tick = get_tick_count();
	list<TimerItem*>::iterator it;

	for(it = m_timer_list.begin(); m_timer_list.end() != it; ) {
		TimerItem* pItem = *it;
		it++;
		if(pItem->next_tick <= curr_tick) {
			pItem->next_tick += pItem->interval;
			pItem->callback(pItem->user_data,NETLIB_MSG_TIMER,0,NULL);
		}
	}
}

void CEventDispatch::_CheckLoop()
{
	for(list<TimerItem*>::iterator it = m_loop_list.begin(); m_loop_list.end() != it; ++it) {
		TimerItem* pItem = *it;
		pItem->callback(pItem->user_data,NETLIB_MSG_LOOP,0,NULL);
	}
}

#ifdef _WIN32

#elif __APPLE__

#else

void CEventDispatch::AddEvent(SOCKET fd, uint8_t socket_event)
{
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLPRI | EPOLLERR | EPOLLHUP;
	ev.data.fd = fd;
	if(0 != epoll_ctl(m_epfd,EPOLL_CTL_ADD,fd,&ev)) {
		//log
	}
}

void CEventDispatch::RemoveEvent(SOCKET fd, uint8_t socket_event)
{
	if(0 != epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, NULL)) {
		//
	}
}

void CEventDispatch::StartDispatch(uint32_t wait_timeout)
{
	struct epoll_event events[1024];
	int nfds = 0;

	if(running)
		return;

	running = true;

	while(running) {
		nfds = epoll_wait(m_epfd,events,1024,wait_timeout);
		for(int i = 0 ; i < nfds ; ++i) {
			int ev_fd = events[i].data.fd;
			CBaseSocket* pSocket = FindBaseSocket(ev_fd);
			if(!pSocket)
				continue;

			#ifdef EPOLLRDHUP
			if(events[i].events & EPOLLRDHUP) {
				pSocket->OnClose();
			}
			#endif

			if(events[i].events & EPOLLIN) {
				pSocket->OnRead();
			}

			if(events[i].events & EPOLLOUT) {
				pSocket->OnWrite();
			}

			if(events[i].events & (EPOLLPRI | EPOLLERR | EPOLLHUP)) {
				pSocket->OnClose();
			}

			pSocket->ReleaseRef();
		}

		_CheckTimer();
		_CheckLoop();
	}
}

void CEventDispatch::StopDispatch()
{
	running = false;
}

#endif
