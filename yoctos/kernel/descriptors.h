#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

// Privilege levels
#define DPL_USER    0x3
#define DPL_KERNEL  0x0

// Selectors
//#define LDT_SELECTOR    0x4

// Descriptor types for code and data segments
#define TYPE_DATA_RO    1   // read-only
#define TYPE_DATA_RW    3   // read-write

// Stack segments are data segments which must be read/write segments. Loading the
// SS register with a segment selector for a nonwritable data segment generates a
// general-protection exception (#GP). If the size of a stack segment needs to be
// changed dynamically, the stack segment can be an expand-down data segment
// (expansion-direction flag set). Here, dynamically changing the segment limit causes
// stack space to be added to the bottom of the stack. If the size of a stack segment is
// intended to remain static, the stack segment may be either an expand-up or expand-down type.
#define TYPE_DATA_RW_EXPAND_DOWN  6  // stack segment type

#define TYPE_CODE_EXECONLY     9
#define TYPE_CODE_EXECREAD    11

// Descriptor types for system segments and gates
#define TYPE_LDT               2
#define TYPE_TASK_GATE         5
#define TYPE_TSS               9
#define TYPE_CALL_GATE         12
#define TYPE_TRAP_GATE         15
#define TYPE_INTERRUPT_GATE    14

#endif
