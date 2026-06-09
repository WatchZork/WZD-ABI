#pragma once
#ifndef WZD_IOCTL_H
#define WZD_IOCTL_H

// Conditional compilation prevents WDK and Windows SDK type collisions
#ifdef _KERNEL_MODE
#include <ntddk.h>
#else
#include <windows.h>
#include <winioctl.h> // Strictly required for User Mode CTL_CODE macro
#endif

// 1. Device Names & Paths
#define WZD_DEVICE_NAME L"\\Device\\WZDK"
#define WZD_SYMBOLIC_LINK_NAME L"\\DosDevices\\WZDK"
#define WZD_USER_DEVICE_NAME L"\\\\.\\WZDK"

// 2. Device Type
#define WZD_DEVICE_TYPE 0x8000

// 3. Function Codes
#define WZD_FUNC_TEST_CONNECTION 0x800

// 4. IOCTL Macros
#define IOCTL_WZD_TEST_CONNECTION CTL_CODE(WZD_DEVICE_TYPE,          \
                                           WZD_FUNC_TEST_CONNECTION, \
                                           METHOD_BUFFERED,          \
                                           FILE_ANY_ACCESS)

#define IOCTL_WZD_MAP_MEMORY CTL_CODE(WZD_DEVICE_TYPE,              \
                                      WZD_FUNC_TEST_CONNECTION + 1, \
                                      METHOD_BUFFERED,              \
                                      FILE_ANY_ACCESS)

#endif // WZD_IOCTL_H