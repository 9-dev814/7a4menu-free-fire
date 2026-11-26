#pragma once
#include <cstdint>
#include <cstddef>
#include <sys/types.h>

class MemoryEngine {
public:
    static bool AttachGame();
    static bool IsAttached();
    static pid_t GetPid();
    static uintptr_t GetLibBase();
    static bool ReadMem(uintptr_t addr, void* out, size_t size);
    static bool WriteMem(uintptr_t addr, const void* data, size_t size);
    static void SyncFeatures();

private:
    static pid_t s_Pid;
    static uintptr_t s_LibBase;
};
