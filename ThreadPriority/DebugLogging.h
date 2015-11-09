#pragma once

#include <ntddk.h>	

//
// DEBUG_ENABLE = 1
// DEBUG_DISABLE = 0
//
#define DEBUG 1

#define THREAD_PRIORITY_PREFIX "ThreadPriority: "

class AutoEnterLeaveFunction
{
	LPSTR m_pFunction;
public:
	AutoEnterLeaveFunction(LPSTR a_pFunction);	
	~AutoEnterLeaveFunction();
};

#define AUTO_ENTER_LEAVE() AutoEnterLeaveFunction _aelf(__FUNCTION__)