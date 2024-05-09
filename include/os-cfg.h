#ifndef OSCFG_H
#define OSCFG_H

#define MLQ_SCHED 1
#define MAX_PRIO 140

#define CPU_TLB
#define MM_PAGING
// #define CPUTLB_FIXED_TLBSZ
#define MM_FIXED_MEMSZ
#define VMDBG 1
#define MMDBG 1
#define IODUMP 1
#define PAGETBL_DUMP 1

#endif

// MM_FIXED_MEMSZ bật lên để chạy file input mà không có dòng memsz ví dụ : 1048576 16777216 0 0 0
// CPUTLB_FIXED_TLBSZ bật lên để chạy file input mà không có dòng định nghĩa kích thước tlb