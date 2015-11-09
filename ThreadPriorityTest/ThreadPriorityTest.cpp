// ThreadPriorityTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\ThreadPriority\ThreadPriority.h"

int PrintError(int code = GetLastError())
{
	printf("Error: %d\n", code);
	return 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
	{
		printf("Usage: ThreadPrioritytest <threadID> <priority>");
		return 1;
	}

	HANDLE hFile = CreateFile(L"\\\\.\\ThreadPriority", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		printf("Error: %d\n", GetLastError());
		return 1;
	}

	printf("Handle open. \n");

	ThreadPriorityData data;
	data.hThread = OpenThread(THREAD_SET_INFORMATION, FALSE, _ttoi(argv[1]));
	if (data.hThread == nullptr)
	{
		return PrintError();
	}
	data.iPriority = _ttoi(argv[2]);

	DWORD dwResponse;
	BOOL fResult =  DeviceIoControl(hFile, 
									IOCTL_THREAD_PRIORITY, 
									&data, 
									sizeof(data), 
									nullptr, 
									0, 
									&dwResponse, 
									nullptr
					);
	if (!fResult)
	{
		return PrintError();
	}

	printf("Succeeded. \n");

	CloseHandle(hFile);

	return 0;
}

