#include <Scheduler.h>
Thread* running;
Thread* queue;
volatile size_t scheduledCounter;
size_t nextPID;
extern PageDirectory* kernelDirectory;
void _kill()
{
    running->done = true;
    while (true);
}
Thread::Thread(void(*entry)(), const char* name, bool user)
{
    pid = nextPID++;
    dpl = user ? 3 : 0;
    this->entry = entry;
    this->name = name;
    memset(&state, 0, sizeof(state));
    state.rsp = kmalloc_a(0x1000) + 0xFF8;
    *(uint64_t*)state.rsp = (uint64_t)_kill;
    state.cs = user ? 0x1B : 0x8;
    state.ds = state.es = state.fs = state.gs = state.ss = user ? 0x23 : 0x10;
    state.rip = (uint64_t)entry;
    state.rflags = 0x202;
    asleep = false;
    joined = false;
    done = false;
    dir = kernelDirectory;
    heap = (Heap*)kmalloc(sizeof(Heap));
    *heap = Heap(0x10000);
    next = NULL;
}
void addThread(Thread* thread)
{
    Thread* t = queue;
    while (t->next) t = t->next;
    t->next = thread;
}
void schedule(CPURegisters* regs)
{
    memcpy(&running->state, regs, sizeof(CPURegisters));
find_thread:
    if (running->next) running = running->next;
    else running = queue;
    if (running->joined)
    {
        if (running->joinTarget->done)
        {
            running->joined = false;
        }
        else if (running != queue) goto find_thread;
    }
    if (running->asleep)
    {
        if (running->sleepTimeout <= scheduledCounter)
        {
            running->asleep = false;
        }
        else if (running != queue) goto find_thread;
    }
    if (running->done) goto find_thread;
    scheduledCounter++;
    switchDirectory(running->dir);
    setCurrentHeap(running->heap);
    memcpy(regs, &running->state, sizeof(CPURegisters));
}
void initializeScheduler()
{
    scheduledCounter = 0;
    nextPID = 0;
    queue = running = (Thread*)kmalloc(sizeof(Thread));
    running->pid = nextPID++;
    running->dpl = 0;
    running->entry = 0;
    running->name = "KERNEL";
    memset(&running->state, 0, sizeof(CPURegisters));
    running->state.rsp = kmalloc_a(0x1000) + 0xFF8;
    *(uint64_t*)running->state.rsp = (uint64_t)_kill;
    running->state.cs = 0x8;
    running->state.ds = running->state.es = running->state.fs = running->state.gs
        = running->state.ss = 0x10;
    running->state.rip = (uint64_t)running->entry;
    running->state.rflags = 0x202;
    running->asleep = false;
    running->joined = false;
    running->done = false;
    running->dir = kernelDirectory->clone();
    running->heap = (Heap*)kmalloc(sizeof(Heap));
    *running->heap = Heap(0x10000);
    running->next = NULL;
}
extern "C" void sleep(uint64_t millis)
{
    running->sleepTimeout = millis + scheduledCounter;
    running->asleep = true;
    while (running->sleepTimeout > scheduledCounter);
}