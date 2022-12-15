#ifndef MODULE_H
#define MODULE_H
#include <SmartOS.h>
#include <Scheduler.h>
#ifdef __cplusplus
struct __attribute__((packed)) ModuleHeader
{
    uint32_t magic; // 0xBEEFCAFE
    uint16_t moduleType;
    char moduleName[16];
    uint64_t elfOffset;
    uint64_t elfSize;
    uint64_t dataOffset;
    uint64_t dataSize;
    uint64_t dataLoadAddress;
    uint8_t data[];
};
enum ModuleType
{
    MODULE_LAYER = 0,
    MODULE_DRIVER_INPUT = 1,
    MODULE_DRIVER_OUTPUT = 2,
    MODULE_DRIVER_BOTH = 3,
    MODULE_NETWORK = 4
};
class Module
{
public:
    char moduleName[17];
    uint16_t moduleType;
    Thread* initThread;
    PageDirectory* directory;
    void (*initializationEntry)();
    uint64_t codeVirt, *codePhys, codeLength;
    uint64_t dataVirt, *dataPhys, dataLength;
    Module(const void* data, size_t size);
    void initialize();
    void summarize();
};
extern Vector<Module> modules;
#endif
#endif