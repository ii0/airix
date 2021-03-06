#ifndef VMM_H
#define VMM_H

#include <kernel/base.h>

/* Number of entries in page directory/table */
#define NUM_PDE 1024
#define NUM_PTE 1024

/* Page directory/table entry flags */
enum vmm_flag
{
    VMM_PRESENT = 0x1,
    VMM_WRITABLE = 0x2,
    VMM_USER = 0x4,
};

/* Page directory entry type */
typedef uint32_t pde_t;
/* Page table entry type */
typedef uint32_t pte_t;

/* Page directory, size is 4KB */
struct page_directory
{
    pde_t entries[NUM_PDE];
};

/* Page table, size is 4KB */
struct page_table
{
    pte_t entries[NUM_PTE];
};

#define VMM_PDE_INDEX(vaddr) \
    (((uint32_t)(vaddr) >> 22) & 0x3FF)

#define VMM_PTE_INDEX(vaddr) \
    (((uint32_t)(vaddr) >> 12) & 0x3FF)

struct page_directory * vmm_alloc_vaddr_space();

void vmm_free_vaddr_space(struct page_directory *page_dir);

struct page_table * vmm_alloc_page_table();

void vmm_free_page_table(struct page_table *page_tab);

void vmm_map_page_table_index(struct page_directory *page_dir, uint32_t index,
                              struct page_table *page_tab, uint32_t flag);

struct page_table * vmm_unmap_page_table_index(struct page_directory *page_dir,
                                               uint32_t index, uint32_t flag);

struct page_table * vmm_get_page_table_index(struct page_directory *page_dir,
                                             uint32_t index, uint32_t *flag);

static inline
void vmm_map_page_table(struct page_directory *page_dir, void *vaddr,
                        struct page_table *page_tab, uint32_t flag)
{
    vmm_map_page_table_index(page_dir, VMM_PDE_INDEX(vaddr), page_tab, flag);
}

static inline
struct page_table * vmm_unmap_page_table(struct page_directory *page_dir,
                                         void *vaddr, uint32_t flag)
{
    return vmm_unmap_page_table_index(page_dir, VMM_PDE_INDEX(vaddr), flag);
}

void vmm_map_page_index(struct page_table *page_tab, uint32_t index,
                        physical_addr_t paddr, uint32_t flag);

physical_addr_t vmm_unmap_page_index(struct page_table *page_tab,
                                     uint32_t index, uint32_t flag);

physical_addr_t vmm_get_page_index(struct page_table *page_tab,
                                   uint32_t index, uint32_t *flag);

static inline
void vmm_map_page(struct page_table *page_tab, void *vaddr,
                  physical_addr_t paddr, uint32_t flag)
{
    vmm_map_page_index(page_tab, VMM_PTE_INDEX(vaddr), paddr, flag);
}

static inline
physical_addr_t vmm_unmap_page(struct page_table *page_tab,
                               void *vaddr, uint32_t flag)
{
    return vmm_unmap_page_index(page_tab, VMM_PTE_INDEX(vaddr), flag);
}

/*
 * Map physical address into virtual address in the page directory.
 * Returns value is negative if map failure,
 * otherwise map success, and the return value is extra used physical
 * memory pages.
 */
int vmm_map(struct page_directory *page_dir, void *vaddr,
            physical_addr_t paddr, uint32_t flag);

#endif /* VMM_H */
