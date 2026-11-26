#include "MemoryEngine.h"
#include "SDK.h"
#include <dirent.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/uio.h>

pid_t MemoryEngine::s_Pid = -1;
uintptr_t MemoryEngine::s_LibBase = 0;

static pid_t FindPackagePid(const char* pkg) {
    DIR* dir = opendir("/proc");
    if (!dir) return -1;
    struct dirent* ent;
    while ((ent = readdir(dir)) != nullptr) {
        pid_t pid = atoi(ent->d_name);
        if (pid <= 0) continue;
        char path[64], cmdline[256];
        snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
        FILE* f = fopen(path, "r");
        if (!f) continue;
        size_t n = fread(cmdline, 1, sizeof(cmdline) - 1, f);
        fclose(f);
        if (n > 0) {
            cmdline[n] = '\0';
            if (strcmp(cmdline, pkg) == 0) {
                closedir(dir);
                return pid;
            }
        }
    }
    closedir(dir);
    return -1;
}

static uintptr_t FindRemoteLibBase(pid_t pid, const char* libName) {
    char path[64], line[512];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    FILE* f = fopen(path, "r");
    if (!f) return 0;
    uintptr_t base = 0;
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, libName) && strstr(line, "r-xp")) {
            base = strtoull(line, nullptr, 16);
            break;
        }
    }
    fclose(f);
    return base;
}

bool MemoryEngine::AttachGame() {
    pid_t pid = FindPackagePid("com.dts.freefireth");
    if (pid <= 0) pid = FindPackagePid("com.dts.freefiremax");
    if (pid <= 0) {
        s_Pid = -1;
        s_LibBase = 0;
        return false;
    }
    s_Pid = pid;
    s_LibBase = FindRemoteLibBase(pid, "libil2cpp.so");
    return s_LibBase != 0;
}

bool MemoryEngine::IsAttached() {
    return s_Pid > 0 && s_LibBase != 0;
}

pid_t MemoryEngine::GetPid() { return s_Pid; }
uintptr_t MemoryEngine::GetLibBase() { return s_LibBase; }

bool MemoryEngine::ReadMem(uintptr_t addr, void* out, size_t size) {
    if (!IsAttached() || !addr) return false;
    struct iovec local{ out, size };
    struct iovec remote{ (void*)addr, size };
    return process_vm_readv(s_Pid, &local, 1, &remote, 1, 0) == (ssize_t)size;
}

bool MemoryEngine::WriteMem(uintptr_t addr, const void* data, size_t size) {
    if (!IsAttached() || !addr) return false;
    struct iovec local{ const_cast<void*>(data), size };
    struct iovec remote{ (void*)addr, size };
    if (process_vm_writev(s_Pid, &local, 1, &remote, 1, 0) == (ssize_t)size) {
        return true;
    }
    char memPath[64];
    snprintf(memPath, sizeof(memPath), "/proc/%d/mem", s_Pid);
    int fd = open(memPath, O_WRONLY);
    if (fd < 0) return false;
    ssize_t written = pwrite(fd, data, size, (off_t)addr);
    close(fd);
    return written == (ssize_t)size;
}

void MemoryEngine::SyncFeatures() {
    if (!IsAttached()) return;

    const uint8_t retZero[8] = { 0x00, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6 };
    const uint8_t retFloatOne[8] = { 0x00, 0x10, 0x2E, 0x1E, 0xC0, 0x03, 0x5F, 0xD6 };

    if (g_Config.noRecoil && Offsets::WeaponRecoil != 0x0) {
        WriteMem(s_LibBase + Offsets::WeaponRecoil, retZero, sizeof(retZero));
    }
    if (g_Config.noSpread && Offsets::WeaponSpread != 0x0) {
        WriteMem(s_LibBase + Offsets::WeaponSpread, retZero, sizeof(retZero));
    }
    if (g_Config.fastReload && Offsets::FastReload != 0x0) {
        WriteMem(s_LibBase + Offsets::FastReload, retFloatOne, sizeof(retFloatOne));
    }
}
