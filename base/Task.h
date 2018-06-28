#ifndef __TASK_H__
#define __TASK_H__

class CTask
{
public:
	CTask() {}
	virtual ~CTask() {}

	virtual void Run() = 0;

private:
};

#endif
