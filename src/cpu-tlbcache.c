/*
 * Copyright (C) 2024 pdnguyen of the HCMC University of Technology
 */
/*
 * Source Code License Grant: Authors hereby grants to Licensee
 * a personal to use and modify the Licensed Source Code for
 * the sole purpose of studying during attending the course CO2018.
 */
// #ifdef MM_TLB
/*
 * Memory physical based TLB Cache
 * TLB cache module tlb/tlbcache.c
 *
 * TLB cache is physically memory phy
 * supports random access
 * and runs at high speed
 */

#include "mm.h"
#include <stdlib.h>
#include <stdio.h>

#define init_tlbcache(mp, sz, ...) init_memphy(mp, sz, (1, ##__VA_ARGS__))

int tlb_cache_setup(struct pcb_t *proc, int pid, int pgn, int *fpn)
{
   // * kiểm tra pte của pgn đã có trong tlb chưa
   //* có trả về fpn (HIT)
   // * k co MISS
   // * xu li page fault neu co

   struct memphy_struct *tlb = proc->tlb;
   if (tlb == NULL)
   {
      printf("chua cap phat tlb");
      exit(1);
   }
   if (pg_getpage(proc->mm, pgn, fpn, proc) != 0)
   {
      printf("fail get page");
      exit(1);
   }
   // int pgn = PAGING_PGN(vmaddr);
   int pgsizeTLB = tlb->maxsz / (int)PAGE_SIZE;
   // printf("pte mm: %08x , pte tlb: %08x, pgn: %d, fgn : %d \n", proc->mm->pgd[pgn], tlb->pgd[pgn % pgsizeTLB].pte, pgn, PTE_FPN(proc->mm->pgd[pgn]));
   if (proc->mm->pgd[pgn] == tlb->pgd[pgn % pgsizeTLB].pte && pid == tlb->pgd[pgn % pgsizeTLB].pid)
   {
      // hit return 0;
      // accessible to fpn
      // if (pg_getpage(proc->mm, pgn, fpn, proc) != 0) // get_page trong ram
      //    return -3000;
      // page fault

      // tlb->pgd[pgn % pgsizeTLB].pte = proc->mm->pgd[pgn];

      // uint32_t pte = tlb->pgd[pgn % pgsizeTLB].pte;
      // *fpn = PAGING_FPN(pte);
      // printf("HIT \n");

      return 0;
   }
   else
   {
      // miss return -1;
      // if (pg_getpage(proc->mm, pgn, fpn, proc) != 0) // get_page trong ram
      //    return -3000;                               /* invalid page access */
      // printf("pte trong setup_cache-miss: %08x\n", proc->mm->pgd[pgn]);

      tlb->pgd[pgn % pgsizeTLB].pte = proc->mm->pgd[pgn];
      tlb->pgd[pgn % pgsizeTLB].pid = proc->pid;
      // printf("MISS \n");
      return -1;
   }
   return 0;
}
/*
 *  tlb_cache_read read TLB cache device
 *  @mp: memphy struct
 *  @pid: process id
 *  @pgnum: page number
 *  @value: obtained value
 */
int tlb_cache_read(struct pcb_t *proc, int pid, int32_t vmaddr, BYTE *value)
{
   /* TODO: the identify info is mapped to
    *      cache line by employing:
    *      direct mapped, associated mapping etc.
    */
   // int pgn = PAGING_PGN(vmaddr);

   int off = PAGING_OFFST(vmaddr);
   int pgn = PAGING_PGN(vmaddr);
   int fpn;
   struct memphy_struct *tlb = proc->tlb;
   int frameNum = tlb->maxsz / PAGE_SIZE;
   if (tlb == NULL)
      return -3000;
   if (tlb_cache_setup(proc, pid, pgn, &fpn) != 0)
   {
      // miss
      int phyaddr = ((fpn % frameNum) << PAGING_ADDR_FPN_LOBIT) + off;
      TLBMEMPHY_read(tlb, phyaddr, value);
      return -1;
   }
   else
   {
      // hit
      int phyaddr = ((fpn % frameNum) << PAGING_ADDR_FPN_LOBIT) + off;
      TLBMEMPHY_read(tlb, phyaddr, value);
      return 0;
   }
   // uint32_t pte = proc->mm->pgd[pgn];
   // fpn = PAGING_SWP(pte);

   return 0;
}

/*
 *  tlb_cache_write write TLB cache device
 *  @mp: memphy struct
 *  @pid: process id
 *  @pgnum: page number
 *  @value: obtained value
 */
int tlb_cache_write(struct pcb_t *proc, int pid, int vmaddr, BYTE *value)
{
   /* TODO: the identify info is mapped to
    *      cache line by employing:
    *      direct mapped, associated mapping etc.
    */
   int off = PAGING_OFFST(vmaddr);
   int fpn;
   int pgn = PAGING_PGN(vmaddr);
   // printf("page write %d \n", pgn);
   struct memphy_struct *tlb = proc->tlb;
   int frameNum = tlb->maxsz / PAGE_SIZE;
   if (tlb == NULL)
      return -1;
   if (tlb_cache_setup(proc, pid, pgn, &fpn) != 0)
   {
      // miss
      int phyaddr = ((fpn % frameNum) << PAGING_ADDR_FPN_LOBIT) + off;

      TLBMEMPHY_write(tlb, phyaddr, *value);
      return -1;
   }
   else
   {
      // hit

      int phyaddr = ((fpn % frameNum) << PAGING_ADDR_FPN_LOBIT) + off;
      TLBMEMPHY_write(tlb, phyaddr, *value);
      return 0;
   }
   // uint32_t pte = proc->mm->pgd[pgn];
   // fpn = PAGING_SWP(pte);

   return 0;
}

/*
 *  TLBMEMPHY_read natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 *  @addr: address
 *  @value: obtained value
 */
int TLBMEMPHY_read(struct memphy_struct *mp, int addr, BYTE *value)
{
   if (mp == NULL)
      return -1;

   /* TLB cached is random access by native */

   *value = mp->storage[addr];

   return 0;
}

/*
 *  TLBMEMPHY_write natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 *  @addr: address
 *  @data: written data
 */
int TLBMEMPHY_write(struct memphy_struct *mp, int addr, BYTE data)
{
   if (mp == NULL)
      return -1;

   /* TLB cached is random access by native */

   mp->storage[addr] = data;

   return 0;
}

/*
 *  TLBMEMPHY_format natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 */

int TLBMEMPHY_dump(struct memphy_struct *mp)
{
   /*TODO dump memphy contnt mp->storage
    *     for tracing the memory content
    */
   printf("===== TLB MEMORY DUMP =====\n");
   if (mp != NULL)
   {

      for (int i = 0; i < mp->maxsz; ++i)
      {
         if (mp->storage[i] != 0)
         {
            printf("BYTE %08x: %d\n", i, mp->storage[i]);
         }
      }
      printf("-------------\n");
      for (int pgn = 0; pgn < mp->maxsz / PAGE_SIZE; pgn++)
      {
         if (mp->pgd[pgn].pte != 0)
         {
            printf("PTE: %08x PID: %d \n", mp->pgd[pgn].pte, mp->pgd[pgn].pid);
         }
      }
   }

   printf("===== TLB MEMORY END-DUMP =====\n");

   return 0;
}

/*
 *  Init TLBMEMPHY struct
 */
int init_tlbmemphy(struct memphy_struct *mp, int max_size)
{

   mp->storage = (BYTE *)malloc(max_size * sizeof(BYTE));
   mp->maxsz = max_size;

   mp->rdmflg = 1;
   int fgnum = DIV_ROUND_UP(max_size, PAGE_SIZE);
   // MEMPHY_format(mp, PAGE_SIZE); // tạo ra 64 frame trống trong free_list
   mp->pgd = malloc(fgnum * sizeof(struct node_pte));
   for (int i = 0; i < fgnum; i++)
   {

      mp->pgd[i].pid = 0;
      mp->pgd[i].pte = 0;
   }
   sem_init(&mp->rw_mutex, 0, 1);
   sem_init(&mp->mutex, 0, 1);
   mp->read_count = 0;

   printf("TLB cache size: %d , framenum: %d , frame size:%d \n", max_size, fgnum, (int)PAGE_SIZE);
   return 0;
}

// #endif
