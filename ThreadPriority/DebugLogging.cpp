#include "DebugLogging.h"

#if DEBUG==1
AutoEnterLeaveFunction::AutoEnterLeaveFunction(LPSTR a_pFunction)
#else
AutoEnterLeaveFunction::AutoEnterLeaveFunction(LPSTR)
#endif
{
#if DEBUG==1
	KdPrint((THREAD_PRIORITY_PREFIX "Enter: %s\n", m_pFunction = a_pFunction));
#endif
}

AutoEnterLeaveFunction::~AutoEnterLeaveFunction()
{
#if DEBUG==1
	KdPrint((THREAD_PRIORITY_PREFIX "Leave: %s\n", m_pFunction));
#endif
}
