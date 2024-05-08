#ifndef OSMM_H
#define OSMM_H

#define MM_PAGING
#define PAGING_MAX_MMSWP 4 /* max number of supported swapped space */
#define PAGING_MAX_SYMTBL_SZ 30

typedef char BYTE;
typedef uint32_t addr_t;
// typedef unsigned int uint32_t;

struct pgn_t
{
   int pgn;
   struct pgn_t *pg_next;
};

/*
 *  Memory region struct
 */
struct vm_rg_struct
{
   // *một vùng virtual memory:
   // *có khoảng đầu và khoảng cuối có con trỏ trỏ đến vùng vm tiếp theo
   // * đây là một khoảng nhỏ trong vm_area_struct
   unsigned long rg_start;
   unsigned long rg_end;

   struct vm_rg_struct *rg_next;
};

/*
 *  Memory area struct
 */
struct vm_area_struct
{
   /*
    * vm_area chứa nhiều vm_rg_struct bên trong
    */
   unsigned long vm_id;
   unsigned long vm_start;
   unsigned long vm_end;

   unsigned long sbrk; // điểm giới hạn trong vm_area
   /*
    * Derived field
    * unsigned long vm_limit = vm_end - vm_start
    */
   struct mm_struct *vm_mm;             // cai này để làm gì v ???
   struct vm_rg_struct *vm_freerg_list; // link list vm_rg_struct trống
   struct vm_area_struct *vm_next;
};

/*
 * Memory management struct
 */
struct mm_struct
{
   // MEMORY MAPPING
   uint32_t *pgd; // pagetable directory chứa tất cả các page table entry ánh xạ giữa page number sang frame number

   struct vm_area_struct *mmap; // list memory area cho một pcb

   /* Currently we support a fixed number of symbol */
   struct vm_rg_struct symrgtbl[PAGING_MAX_SYMTBL_SZ]; // ĐỊnh nghĩa số biến trong một proc
   // memory regions in a separated contiguous memory area

   /* list of free page */
   struct pgn_t *fifo_pgn;
};

/*
 * FRAME/MEM PHY struct
 */
struct framephy_struct
{
   // chủ yếu lưu trữ frame number
   int fpn;
   struct framephy_struct *fp_next;

   /* Resereed for tracking allocated framed */
   struct mm_struct *owner;
};

struct node_pte
{
   uint32_t pte;
   int pid;
};

struct memphy_struct
{
   /* Basic field of data and size */
   BYTE *storage; // một array chứa các BYTE địa chỉ
   int maxsz;     // Kích thước tối đa của storage

   /* Sequential device fields */
   int rdmflg;            // định nghĩa bộ nhớ truy cập ngẫu nhiên hay tuần tự
   int cursor;            // COn trỏ để theo dõi vị trí hiện tạitrong quá trình đọc ghi dl vào storage
   struct node_pte **pgd; // list pte only TLB;
   /* Management structure */
   struct framephy_struct *free_fp_list;
   struct framephy_struct *used_fp_list;
};

#endif
