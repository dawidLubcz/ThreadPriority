#include <ntddk.h>
#include "DebugLogging.h"
#include "ThreadPriority.h"

//
// Global
//
const wchar_t* g_pwDeviceName = L"\\Device\\ThreadPriority"; // device name
const wchar_t* g_pwLinkdevName = L"\\??\\ThreadPriority";    // link to device name

//
// Declarations
//
void ThreadPriorityUnload(PDRIVER_OBJECT);
NTSTATUS ThreadPriorityCreateClose(PDEVICE_OBJECT /*DeviceObject*/, PIRP a_sIrp);
NTSTATUS ThreadPriorityDeviceControl(PDEVICE_OBJECT /*DeviceObject*/, PIRP a_sIrp);

//
// Driver entry
// 
extern "C" // using C linkage
NTSTATUS DriverEntry(PDRIVER_OBJECT a_oDriverObject, PUNICODE_STRING /*RegistryPath*/)
{
	AUTO_ENTER_LEAVE();

	NTSTATUS sStatus = STATUS_SUCCESS; // result of creating a device
	PDEVICE_OBJECT DeviceObject;		
	UNICODE_STRING devName, win32Name; // device name, symbolic link name

	a_oDriverObject->DriverUnload = ThreadPriorityUnload;  // pointer to driver unload function
	a_oDriverObject->MajorFunction[IRP_MJ_CREATE] = 
		a_oDriverObject->MajorFunction[IRP_MJ_CLOSE] = ThreadPriorityCreateClose;			// pointer to create and close handle to driver function
	a_oDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ThreadPriorityDeviceControl;	// pointer function for controling the driver
	
	RtlInitUnicodeString(&devName, g_pwDeviceName);	// create save unicode string
	sStatus = IoCreateDevice(a_oDriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject); // creating a device

	if (STATUS_SUCCESS == sStatus)
	{
		RtlInitUnicodeString(&win32Name, g_pwLinkdevName); // create save unicode string
		IoCreateSymbolicLink(&win32Name, &devName);				   // create symbolic link to the device for using CreateFile function from user mode
	}
	else
	{
		KdPrint((THREAD_PRIORITY_PREFIX "Error creating device %d\n", sStatus)); // print if error occured
	}	

	return sStatus;
}

//
// ThreadPriorityUnload
//
void ThreadPriorityUnload(PDRIVER_OBJECT a_oDriverObject)
{
	AUTO_ENTER_LEAVE();

	UNICODE_STRING win32Name;
	RtlInitUnicodeString(&win32Name, g_pwLinkdevName);
	IoDeleteSymbolicLink(&win32Name); 
	IoDeleteDevice(a_oDriverObject->DeviceObject);
}

//
// ThreadPriorityUnload
//
NTSTATUS ThreadPriorityCreateClose(PDEVICE_OBJECT /*DeviceObject*/, PIRP a_sIrp)
{
	AUTO_ENTER_LEAVE();

	a_sIrp->IoStatus.Status = STATUS_SUCCESS;
	a_sIrp->IoStatus.Information = 0;

	IoCompleteRequest(a_sIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

//
// ThreadPriosityDeviceControl
//
NTSTATUS ThreadPriorityDeviceControl(PDEVICE_OBJECT /*DeviceObject*/, PIRP a_sIrp)
{
	AUTO_ENTER_LEAVE();

	NTSTATUS sStatus = STATUS_UNSUCCESSFUL;

	PIO_STACK_LOCATION sStackLocation = IoGetCurrentIrpStackLocation(a_sIrp); //equal to: a_sIrp->Tail.Overlay.CurrentStackLocation->Parameters.DeviceIoControl.IoControlCode

	switch (sStackLocation->Parameters.DeviceIoControl.IoControlCode)
	{
		case IOCTL_THREAD_PRIORITY:
		{			
			if (sStackLocation->Parameters.DeviceIoControl.InputBufferLength < sizeof(ThreadPriorityData))
			{
				sStatus = STATUS_INVALID_BLOCK_LENGTH;
				break;
			}
			ThreadPriorityData* sData = reinterpret_cast<ThreadPriorityData*>(a_sIrp->AssociatedIrp.SystemBuffer);

			PKTHREAD pThread;
			sStatus = ObReferenceObjectByHandle(sData->hThread, 
												THREAD_SET_INFORMATION, 
												*PsThreadType, 
												UserMode, 
												(PVOID*)&pThread, 
												nullptr);
			if (STATUS_SUCCESS != sStatus)
			{	
				break;
			}
			KeSetPriorityThread(pThread, sData->iPriority);
			ObDereferenceObject(pThread);
		}
		break;
	}
	
	a_sIrp->IoStatus.Status = sStatus;
	a_sIrp->IoStatus.Information = 0;
	IoCompleteRequest(a_sIrp, IO_NO_INCREMENT);

	return sStatus;
}