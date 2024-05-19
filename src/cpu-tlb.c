/*
 * Copyright (C) 2024 pdnguyen of the HCMC University of Technology
 */
/*
 * Source Code License Grant: Authors hereby grants to Licensee
 * a personal to use and modify the Licensed Source Code for
 * the sole purpose of studying during attending the course CO2018.
 */
// #ifdef CPU_TLB
/*
 * CPU TLB
 * TLB module cpu/cpu-tlb.c
 */

#include "mm.h"
#include <stdlib.h>
#include <stdio.h>

int tlb_change_all_page_tables_of(struct pcb_t *proc, struct memphy_struct *mp)
{
  /* TODO update all page table directory info
   *      in flush or wipe TLB (if needed)
   */

  return 0;
}

int tlb_flush_tlb_of(struct memphy_struct *mp)
{
  /* TODO flush tlb cached*/
  if (mp == NULL || mp->pgd == NULL)
    return -1;
  // printf("Flush cache:\n");
  // struct memphy_struct *tlb = proc->tlb;
  // int fpnum = mp->maxsz / PAGE_SIZE;
  // for (int i = 0; i < fpnum; i++)
  // {
  //   // printf("pte: %d , pid: %d \n", mp->pgd[i].pte, mp->pgd[i].pid);
  // }
  free(mp->pgd);
  // sem_destroy(&mp->rw_mutex);
  // sem_destroy(&mp->mutex);
  mp->pgd = NULL;

  return 0;
}

/*tlballoc - CPU TLB-based allocate a region memory
 *@proc:  Process executing the instruction
 *@size: allocated size
 *@reg_index: memory region ID (used to identify variable in symbole table)
 */
int tlballoc(struct pcb_t *proc, uint32_t size, uint32_t reg_index)
{
  int addr, val;
  // printf("tlb_alloc \n");
  if (proc->mm->symrgtbl[reg_index].rg_start != proc->mm->symrgtbl[reg_index].rg_end)
  {
    // free reg_index
    tlbfree_data(proc, reg_index);
  }

  /* By default using vmaid = 0 */
  val = __alloc(proc, 0, reg_index, size, &addr); // thực hiện alloc vào vma có id = 0
                                                  // __ alloc map vma to ram khi inc limit
                                                  //

  /* TODO update TLB CACHED frame num of the new allocated page(s)*/

  int fpn;
  int pgn_start = PAGING_PGN(addr);
  int pgn_end = PAGING_PGN((addr + size));
  printf("----PID:%d ALLOC region: %d size: %d\n", proc->pid, reg_index, size);
  printf("pgnum-start: %d, pgnum-end: %d, addr_start: %d, addr_end: %d \n", pgn_start, pgn_end, addr, (addr + size));
  // struct memphy_struct *tlb = proc->tlb;
  // int frameNum = tlb->maxsz / PAGE_SIZE;

  for (int pgn = pgn_start; pgn <= pgn_end; pgn++)
  {
    tlb_cache_setup(proc, proc->pid, pgn, &fpn);
    // * collect garbage
    // int base_addr = ((fpn % frameNum) << PAGING_ADDR_FPN_LOBIT);
    // for (int i = 0; i < PAGE_SIZE; i++)
    // {
    //   tlb->storage[base_addr + i] = 0;
    // }
  }
  // TLBMEMPHY_dump(proc->tlb);
  return val;
}

/*pgfree - CPU TLB-based free a region memory
 *@proc: Process executing the instruction
 *@size: allocated size
 *@reg_index: memory region ID (used to identify variable in symbole table)
 */
int tlbfree_data(struct pcb_t *proc, uint32_t reg_index)
{

  /* TODO update TLB CACHED frame num of freed page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/
  // printf("tlb_free \n");

  // int fpn;
  // unsigned long rg_start = proc->mm->symrgtbl[reg_index].rg_start;
  // unsigned long rg_end = proc->mm->symrgtbl[reg_index].rg_end;
  // int fpnumTLB = tlb->maxsz / PAGE_SIZE; // số frame trong tlb
  // int pgn_start = PAGING_PGN(rg_start);
  // int pgn_end = PAGING_PGN((rg_end));
  // struct memphy_struct *tlb = proc->tlb;
  // int frameNum = tlb->maxsz / PAGE_SIZE;
  // printf("pgnum-start: %d \n", pgn_start);
  // printf("pgnum-end: %d \n", pgn_end);
  // printf("reg_index: %d \n", reg_index);

  // for (int pgn = pgn_start; pgn <= pgn_end; pgn++)
  // {
  // printf("vong lap o free\n");
  // bỏ pte để tránh truy cập vào vùng nhớ đã được thu hồi
  // int fpn = PTE_FPN(tlb->pgd[pgn].pte);
  // int base_addr = ((fpn % frameNum) << PAGING_ADDR_FPN_LOBIT);
  // for (int i = 0; i < PAGE_SIZE; i++)
  // {
  //   tlb->storage[base_addr + i] = 0;
  // }
  // tlb->pgd[pgn].pte = 0;
  // tlb->pgd[pgn].pid = 0;
  // }
  __free(proc, 0, reg_index);
  printf("----PID: %d FREE region: %d \n", proc->pid, reg_index);
  // TLBMEMPHY_dump(proc->tlb);

  return 0;
}

/*tlbread - CPU TLB-based read a region memory
 *@proc: Process executing the instruction
 *@source: index of source register
 *@offset: source address = [source] + [offset]
 *@destination: destination storage
 */
int tlbread(struct pcb_t *proc, uint32_t source,
            uint32_t offset, uint32_t destination)

{
  printf("----PID: %d READ region: %d offset: %d destination: %d\n", proc->pid, source, offset, destination);
  BYTE data;
  unsigned long rg_start = proc->mm->symrgtbl[source].rg_start;
  unsigned long rg_end = proc->mm->symrgtbl[source].rg_end;
  // printf("rg_start: %ld, rg_end: %ld", rg_start, rg_end);
  // if (rg_start == rg_end)
  // {
  //   printf("TLB  read region=%d offset=%d segmentation fault \n",
  //          source, offset);
  //   exit(1);
  // }

  int32_t vmaddr = rg_start + offset;
  if (vmaddr < rg_start || vmaddr > rg_end)
  {
    printf("TLB  read region=%d offset=%d out of range region memory \n",
           source, offset);
    exit(1);
  }

  /* TODO retrieve TLB CACHED frame num of accessing page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/
  /* frmnum is return value of tlb_cache_read/write value*/
  int val = __read(proc, 0, source, offset, &data);

#ifdef IODUMP
  if (tlb_cache_read(proc, proc->pid, vmaddr, &data) >= 0)
    printf("TLB hit at read region=%d offset=%d value=%d\n",
           source, offset, data);
  else
    printf("TLB miss at read region=%d offset=%d value=%d\n",
           source, offset, data);
#ifdef PAGETBL_DUMP
  print_pgtbl(proc, 0, -1); // print max TBL
#endif
  MEMPHY_dump(proc->mram);
#endif

  destination = (uint32_t)data;
  TLBMEMPHY_dump(proc->tlb);

  /* TODO update TLB CACHED with frame num of recent accessing page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/

  return val;
}

/*tlbwrite - CPU TLB-based write a region memory
 *@proc: Process executing the instruction
 *@data: data to be wrttien into memory
 *@destination: index of destination register
 *@offset: destination address = [destination] + [offset]
 */
int tlbwrite(struct pcb_t *proc, BYTE data,
             uint32_t destination, uint32_t offset)
{
  printf("----PID: %d WRITE region: %d offset: %d data: %d \n", proc->pid, destination, offset, data);
  int val;
  if (proc == NULL)
  {
    printf("proc is null");
    exit(1);
  }
  // if (proc->mm == NULL)
  // {
  //   printf("proc -> mm is null");
  //   exit(1);
  // }
  // if (proc->mm->symrgtbl == NULL)
  // {
  //   printf("proc -> mm->symrgtlb is null");
  //   exit(1);
  // }

  unsigned long rg_start = proc->mm->symrgtbl[destination].rg_start;
  unsigned long rg_end = proc->mm->symrgtbl[destination].rg_end;
  int32_t vmaddr = rg_start + offset;
  printf("rg_start: %ld , rg_end: %ld, vm_addr: %d \n", rg_start, rg_end, vmaddr);

  // if (rg_start == rg_end)
  // {
  //   printf("TLB  write region=%d offset=%d segmentation fault \n",
  //          destination, offset);
  //   exit(1);
  // }

  if (vmaddr < rg_start || vmaddr > rg_end)
  {
    printf("TLB  write region=%d offset=%d out of range region memory \n",
           destination, offset);
    exit(1);
  }

  /* TODO retrieve TLB CACHED frame num of accessing page(s))*/
  /* by using tlb_cache_read()/tlb_cache_write()
  frmnum is return value of tlb_cache_read/write value*/

  val = __write(proc, 0, destination, offset, data);
#ifdef IODUMP
  if (tlb_cache_write(proc, proc->pid, vmaddr, &data) >= 0)
    printf("TLB hit at write region=%d offset=%d value=%d\n",
           destination, offset, data);
  else
    printf("TLB miss at write region=%d offset=%d value=%d\n",
           destination, offset, data);
#ifdef PAGETBL_DUMP
  print_pgtbl(proc, 0, -1); // print max TBL
#endif
  MEMPHY_dump(proc->mram);
#endif
  TLBMEMPHY_dump(proc->tlb);

  /* TODO update TLB CACHED with frame num of recent accessing page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/

  return val;
}

// #endif
