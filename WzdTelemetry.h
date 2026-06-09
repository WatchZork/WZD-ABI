#pragma once
#ifndef WZD_TELEMETRY_H
#define WZD_TELEMETRY_H

#ifdef _KERNEL_MODE
#include <ntddk.h>
#else
#include <windows.h>
#endif

// 1. Buffer Constants
#define WZD_MAX_PATH_LENGTH 512
#define WZD_MAX_CMD_LENGTH 1024
#define WZD_MAX_SID_SIZE 68
#define WZD_MAX_EVENTS 1024
#define WZD_EVENT_MASK (WZD_MAX_EVENTS - 1)

// 2. Enums
typedef enum _WZD_SLOT_STATE
{
    WZDRB_SlotFree = 0,
    WZDRB_SlotWriting = -1,
    WZDRB_SlotReady = 1
} WZD_SLOT_STATE;

typedef enum _WZD_EVENT_TYPE
{
    WZDEventProcessTerminate = 0,
    WZDEventProcessCreate = 1
} WZD_EVENT_TYPE;

// 3. Telemetry Structures
#pragma pack(push, 8)

typedef struct _WZD_PROCESS_EVENT
{
    WZD_EVENT_TYPE EventType;
    ULONG ProcessId;
    ULONG ParentProcessId;
    ULONG TrueCreatorPid;
    ULONG TrueCreatorTid;
    ULONG SessionId;
    ULONG SidLength;
    BOOLEAN Is32Bit;
    UCHAR Sid[WZD_MAX_SID_SIZE];
    WCHAR ImageFileName[WZD_MAX_PATH_LENGTH];
    WCHAR CommandLine[WZD_MAX_CMD_LENGTH];
} WZD_PROCESS_EVENT, *PWZD_PROCESS_EVENT;

typedef struct _WZD_RING_BUFFER
{
    DECLSPEC_ALIGN(64)
    volatile LONG Head;
    DECLSPEC_ALIGN(64)
    volatile LONG DroppedEventsCount;
    DECLSPEC_ALIGN(64)
    volatile LONG Tail;
    DECLSPEC_ALIGN(64)
    volatile LONG SlotStates[WZD_MAX_EVENTS];
    WZD_PROCESS_EVENT Events[WZD_MAX_EVENTS];
} WZD_RING_BUFFER, *PWZD_RING_BUFFER;

#pragma pack(pop)

#endif // WZD_TELEMETRY_H