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

int tlb_flush_tlb_of(struct pcb_t *proc, struct memphy_struct *mp)
{
  /* TODO flush tlb cached*/
  if (mp == NULL || proc == NULL)
    return -1;
  printf("Flush cache:\n");
  // struct memphy_struct *tlb = proc->tlb;
  int fpnum = mp->maxsz / PAGE_SIZE;
  for (int i = 0; i < fpnum; i++)
  {
    free(mp->pgd[i]);
  }
  free(mp->pgd);

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

  /* By default using vmaid = 0 */
  val = __alloc(proc, 0, reg_index, size, &addr); // thực hiện alloc vào vma có id = 0
                                                  // __ alloc map vma to ram khi inc limit
                                                  //

  /* TODO update TLB CACHED frame num of the new allocated page(s)*/
  // tlb_cache_setup(proc,proc->pid, )
  /* by using tlb_cache_read()/tlb_cache_write()*/
  // printf("pgnum-start: %d", PAGING_PGN(addr));
  // printf("pgnum-end: %d", PAGING_PGN((addr + size)));
  // struct memphy_struct *tlb = proc->tlb;
  // if (tlb == NULL)
  //   return -1;
  // int fpnumTLB = DIV_ROUND_UP(tlb->maxsz, PAGE_SIZE); // số frame trong tlb
  int fpn;
  int pgn_start = PAGING_PGN(addr);
  int pgn_end = PAGING_PGN((addr + size));
  printf("pgnum-start: %d \n", PAGING_PGN((addr)));
  printf("pgnum-end: %d \n", PAGING_PGN((addr + size)));
  printf("reg_index: %d \n", reg_index);

  for (int pgn = pgn_start; pgn <= pgn_end; pgn++)
  {
    printf("vong lap o alloc\n");
    tlb_cache_setup(proc, proc->pid, pgn, &fpn);
  }

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

  int fpn;
  unsigned long rg_start = proc->mm->symrgtbl[reg_index].rg_start;
  unsigned long rg_end = proc->mm->symrgtbl[reg_index].rg_end;
  // int fpnumTLB = tlb->maxsz / PAGE_SIZE; // số frame trong tlb
  int pgn_start = PAGING_PGN(rg_start);
  int pgn_end = PAGING_PGN((rg_end));
  printf("pgnum-start: %d \n", pgn_start);
  printf("pgnum-end: %d \n", pgn_end);
  printf("reg_index: %d \n", reg_index);

  for (int pgn = pgn_start; pgn <= pgn_end; pgn++)
  {
    printf("vong lap o free\n");

    tlb_cache_setup(proc, proc->pid, pgn, &fpn);
  }
  __free(proc, 0, reg_index);

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
  BYTE data;
  int32_t vmaddr = source + offset;

  /* TODO retrieve TLB CACHED frame num of accessing page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/
  /* frmnum is return value of tlb_cache_read/write value*/

#ifdef IODUMP
  if (tlb_cache_read(proc, proc->pid, vmaddr, &data) >= 0)
    printf("TLB hit at read region=%d offset=%d\n",
           source, offset);
  else
    printf("TLB miss at read region=%d offset=%d\n",
           source, offset);
#ifdef PAGETBL_DUMP
  print_pgtbl(proc, 0, -1); // print max TBL
#endif
  MEMPHY_dump(proc->mram);
#endif

  int val = __read(proc, 0, source, offset, &data);

  destination = (uint32_t)data;

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
  int val;

  int32_t vmaddr = destination + offset;
  /* TODO retrieve TLB CACHED frame num of accessing page(s))*/
  /* by using tlb_cache_read()/tlb_cache_write()
  frmnum is return value of tlb_cache_read/write value*/

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

  val = __write(proc, 0, destination, offset, data);

  /* TODO update TLB CACHED with frame num of recent accessing page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/

  return val;
}

// #endif
