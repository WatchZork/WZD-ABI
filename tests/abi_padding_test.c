// tests/abi_padding_test.c
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
// GCC Mock Environment for Windows Kernel Types
// -----------------------------------------------------------------------------
typedef uint32_t ULONG;
typedef int32_t LONG;
typedef uint16_t WCHAR;
typedef uint8_t BOOLEAN;
typedef uint8_t UCHAR;

// Duplicate constants to bypass <ntddk.h> requirement on Linux CI
#define WZD_MAX_PATH_LENGTH 512
#define WZD_MAX_CMD_LENGTH 1024
#define WZD_MAX_SID_SIZE 68
#define WZD_MAX_EVENTS 1024

typedef enum _WZD_EVENT_TYPE
{
    WZDEventProcessTerminate = 0,
    WZDEventProcessCreate = 1
} WZD_EVENT_TYPE;

// -----------------------------------------------------------------------------
// WatchZork Telemetry Structures (Explicitly Padded)
// -----------------------------------------------------------------------------
#pragma pack(push, 8)

typedef struct _WZD_PROCESS_EVENT
{
    WZD_EVENT_TYPE EventType;    // Offset: 0
    ULONG ProcessId;             // Offset: 4
    ULONG ParentProcessId;       // Offset: 8
    ULONG TrueCreatorPid;        // Offset: 12
    ULONG TrueCreatorTid;        // Offset: 16
    ULONG SessionId;             // Offset: 20
    ULONG SidLength;             // Offset: 24
    BOOLEAN Is32Bit;             // Offset: 28
    UCHAR Sid[WZD_MAX_SID_SIZE]; // Offset: 29
    // 1 BYTE IMPLICIT COMPILER PADDING         // Offset: 97
    WCHAR ImageFileName[WZD_MAX_PATH_LENGTH]; // Offset: 98
    WCHAR CommandLine[WZD_MAX_CMD_LENGTH];    // Offset: 1122
    // 2 BYTE IMPLICIT COMPILER TAIL PADDING    // Offset: 3170
} WZD_PROCESS_EVENT, *PWZD_PROCESS_EVENT; // Total Size: 3172

typedef struct _WZD_RING_BUFFER
{
    // 64-Byte Cache Line 1
    volatile LONG Head;  // Offset: 0, Size: 4
    UCHAR _Padding1[60]; // Offset: 4, Size: 60

    // 64-Byte Cache Line 2
    volatile LONG DroppedEventsCount; // Offset: 64, Size: 4
    UCHAR _Padding2[60];              // Offset: 68, Size: 60

    // 64-Byte Cache Line 3
    volatile LONG Tail;  // Offset: 128, Size: 4
    UCHAR _Padding3[60]; // Offset: 132, Size: 60

    // Contiguous Array Block
    volatile LONG SlotStates[WZD_MAX_EVENTS]; // Offset: 192, Size: 4096

    // Event Payload Array (Starts on a 64-byte boundary: 192 + 4096 = 4288)
    WZD_PROCESS_EVENT Events[WZD_MAX_EVENTS]; // Offset: 4288, Size: 3248128
} WZD_RING_BUFFER, *PWZD_RING_BUFFER;

#pragma pack(pop)

// -----------------------------------------------------------------------------
// Zero-Copy ABI Alignment Contracts
// -----------------------------------------------------------------------------
_Static_assert(sizeof(WZD_PROCESS_EVENT) == 3172, "FATAL: WZD_PROCESS_EVENT size drift detected.");
_Static_assert(offsetof(WZD_PROCESS_EVENT, ImageFileName) == 98, "ABI Drift: ImageFileName missing 1-byte padding.");

// Core Pipeline Struct Bounds Tests
_Static_assert(offsetof(WZD_RING_BUFFER, Head) == 0, "ABI Drift: Head alignment");
_Static_assert(offsetof(WZD_RING_BUFFER, DroppedEventsCount) == 64, "ABI Drift: Cache Line False Sharing vulnerability.");
_Static_assert(offsetof(WZD_RING_BUFFER, Tail) == 128, "ABI Drift: Tail alignment");
_Static_assert(offsetof(WZD_RING_BUFFER, SlotStates) == 192, "ABI Drift: SlotStates alignment");
_Static_assert(offsetof(WZD_RING_BUFFER, Events) == 4288, "ABI Drift: Events array offset misalignment.");
_Static_assert(sizeof(WZD_RING_BUFFER) == 3252416, "FATAL: Ring buffer maximum memory boundary shifted.");

int main(void)
{
    printf("[+] WatchZork WZD-ABI Static Alignment Checks Passed Successfully on x86 and x64 targets.\n");
    return 0;
}