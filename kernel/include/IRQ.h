#ifndef IRQ_H
#define IRQ_H
#include <SmartOS.h>
#ifdef __cplusplus
void initializePIC();
void initializeAPIC();
typedef void(*IRQHandler)(CPURegisters*);
void registerIRQHandler(uint32_t irqNum, IRQHandler handler);
#endif
#endif