#pragma once

#define IOCTL_THREAD_PRIORITY CTL_CODE(0x800, 0x800, METHOD_BUFFERED, FILE_WRITE_ACCESS)

struct ThreadPriorityData
{
	HANDLE hThread;
	int iPriority;
};