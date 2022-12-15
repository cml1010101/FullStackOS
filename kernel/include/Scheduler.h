#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <SmartOS.h>
#include <Heap.h>
#include <MMU.h>
#ifdef __cplusplus
class Thread
{
public:
    void(*entry)();
    uint8_t dpl;
    CPURegisters state;
    volatile bool joined, asleep, done;
    Thread* joinTarget;
    size_t sleepTimeout, pid;
    const char* name;
    Thread* next;
    PageDirectory* dir;
    Heap* heap;
    Thread() = default;
    Thread(void(*entry)(), const char* name, bool user = false);
};
void addThread(Thread* thread);
void schedule(CPURegisters* regs);
void initializeScheduler();
extern Thread* running;
#endif
#endif