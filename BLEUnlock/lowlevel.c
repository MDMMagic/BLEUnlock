#include "lowlevel.h"
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/IOKitLib.h>
#include <spawn.h>

void wakeDisplay(void)
{
    static IOPMAssertionID assertionID;
    IOPMAssertionDeclareUserActivity(CFSTR("BLEUnlock"), kIOPMUserActiveLocal, &assertionID);
}

void sleepDisplay(void)
{
    // IODisplayWrangler was removed in macOS 13; this path works on macOS 12 and earlier
    io_registry_entry_t reg = IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/IOResources/IODisplayWrangler");
    if (reg) {
        IORegistryEntrySetCFProperty(reg, CFSTR("IORequestIdle"), kCFBooleanTrue);
        IOObjectRelease(reg);
        return;
    }
    // Fallback for macOS 13+ where IODisplayWrangler was removed
    extern char **environ;
    char * const args[] = {(char *)"/usr/bin/pmset", (char *)"displaysleepnow", NULL};
    pid_t pid;
    posix_spawn(&pid, args[0], NULL, NULL, args, environ);
}
