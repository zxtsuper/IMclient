#include "ThreadPool.h"


CWorkerThread::CWorkerThread()
{
	m_thread_idx = 0;
	m_task_cnt = 0;
}

CWorkerThread::~CWorkerThread()
{

}

void CWorkerThread::Start()
{
	(void)pthread_create(&m_thread_id,NULL,StartRoutine,this);
}

void CWorkerThread::Execute()
{
	while(true) {
		m_thread_notify.Lock();

		while(m_task_list.empty()) {
			m_thread_notify.Wait();
		}

		CTask* pTask = m_task_list.front();
		m_task_list.pop_front();
		m_thread_notify.Unlock();

		pTask->Run();

		delete pTask;

		m_task_cnt++;
	}
}
	
void CWorkerThread::PushTask(CTask* pTask)
{
	m_thread_notify.Lock();
	m_task_list.push_back(pTask);
	m_thread_notify.Signal();
	m_thread_notify.Unlock();
}

void* CWorkerThread::StartRoutine(void* arg)
{
	CWorkerThread* pThread = (CWorkerThread*)arg;

	pThread->Execute();
	return NULL;
}

CThreadPool::CThreadPool()
{
	m_worker_size = 0;
	m_worker_list = NULL;
}

CThreadPool::~CThreadPool()
{

}

int CThreadPool::Init(uint32_t worker_size)
{
	m_worker_size  = worker_size;
	m_worker_list = new CWorkerThread[m_worker_size];
	if(!m_worker_list) {
		return 1;
	}

	for(uint32_t i = 0 ; i < m_worker_size; ++i) {
		m_worker_list[i].SetThreadIdx(i);
		m_worker_list[i].Start();
		usleep(10);
	}

	return 0;
}

void CThreadPool::AddTask(CTask* pTask)
{
	uint32_t thread_idx = random() % m_worker_size;
	m_worker_list[thread_idx].PushTask(pTask);
}

void CThreadPool::Destory()
{
	if(m_worker_list) {
		delete [] m_worker_list;
		m_worker_list = NULL;
	}
}




