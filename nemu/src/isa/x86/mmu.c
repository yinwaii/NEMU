#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>
#ifndef isa_vaddr_check

union LinearAddress
{
  struct {
    word_t offset : 12;
    word_t page : 10;
    word_t dir : 10;
  };
  vaddr_t val;
};
union PageTableEntry
{
  struct {
    word_t present : 1;
    word_t read_write : 1;
    word_t user_supervisor : 1;
    word_t : 2;
    word_t A : 1;
    word_t dirty : 1;
    word_t : 2;
    word_t available : 3;
    word_t address : 20;
  };
  vaddr_t val;
};

int isa_vaddr_check(vaddr_t vaddr, int type, int len)
{
  if (cpu.CR0.PG == 1)
    return MEM_RET_NEED_TRANSLATE;
  else
    return MEM_RET_OK;
}
#endif

paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len) {
  union LinearAddress la;
  la.val = vaddr;
  union PageTableEntry directory, page;
  directory.val = paddr_read((cpu.CR3.val & 0xfffff000) + 4 * la.dir, 4);
  // Log("cr3: %x", cpu.CR3.val);
  if (directory.present == 0)
    panic("pagefault at vaddr = 0x%x, directory: %x, from %x", vaddr, directory.val, (cpu.CR3.val & 0xfffff000) + 4 * la.dir);
  page.val = paddr_read((directory.val & 0xfffff000) + 4 * la.page, 4);
  if (page.present == 0)
    panic("pagefault at vaddr = 0x%x, directory: %x, page: %x", vaddr, directory.val, page.val);
  paddr_t paddr = (page.val & 0xfffff000) + la.offset;
  assert(paddr == vaddr);
  return paddr;
}

word_t vaddr_mmu_read(vaddr_t addr, int len, int type) {
  if (((addr + len - 1) & 0xfffff000) > (addr & 0xfffff000))
  {
    word_t res = 0;
    for (int i = 0; i < len; i++)
    {
      vaddr_t vtmp = addr + i;
      paddr_t ptmp = isa_mmu_translate(vtmp, type, 1);
      res += paddr_read(ptmp, 1) << (8 * i);
    }
    return res;
    // int first_len = ((addr + len - 1) & 0xfffff000) - addr;
    // paddr_t first_paddr = isa_mmu_translate(addr, type, first_len);
    // paddr_t second_paddr = isa_mmu_translate((addr + len - 1) & 0xfffff000, type, len - first_len);
    // Log("first length: %d second length: %d", first_len, len - first_len);
    // return paddr_read(first_paddr, first_len) + (paddr_read(second_paddr, len - first_len) << (8 * first_len));
  }
  else
  {
    paddr_t paddr = isa_mmu_translate(addr, type, len);
    return paddr_read(paddr, len);
  }
}
void vaddr_mmu_write(vaddr_t addr, word_t data, int len) {
  if (((addr + len - 1) & 0xfffff000) > (addr & 0xfffff000))
  {
    for (int i = 0; i < len; i++)
    {
      vaddr_t vtmp = addr + i;
      paddr_t ptmp = isa_mmu_translate(vtmp, 0, 1);
      word_t dat = data & 0xff;
      data = data >> 8;
      paddr_write(ptmp, dat, 1);
    }
    // int first_len = ((addr + len - 1) & 0xfffff000) - addr;
    // paddr_t first_paddr = isa_mmu_translate(addr, MEM_TYPE_WRITE, first_len);
    // paddr_t second_paddr = isa_mmu_translate((addr + len - 1) & 0xfffff000, MEM_TYPE_WRITE, len - first_len);
    // word_t second_data = data >> (8 * first_len);
    // word_t first_data = data - (data << (8 * first_len));
    // Log("first length: %d second length: %d", first_len, len - first_len);
    // paddr_write(first_paddr, first_data, first_len);
    // paddr_write(second_paddr, second_data, len - first_len);
  }
  else
  {
    paddr_t paddr = isa_mmu_translate(addr, MEM_TYPE_WRITE, len);
    paddr_write(paddr, data, len);
  }
}