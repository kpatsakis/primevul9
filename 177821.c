    sizeof(max_align_t)
    : sizeof(void *) + sizeof(size_t);

#define ADD_TO_POINTER(p, d) ((void *)((uintptr_t)(p) + (d)))

void *
ffi_closure_alloc (size_t size, void **code)
{
  static size_t page_size;
  size_t rounded_size;
  void *codeseg, *dataseg;
  int prot;

  /* Expect that PAX mprotect is active and a separate code mapping is necessary. */
  if (!code)
    return NULL;

  /* Obtain system page size. */
  if (!page_size)
    page_size = sysconf(_SC_PAGESIZE);

  /* Round allocation size up to the next page, keeping in mind the size field and pointer to code map. */
  rounded_size = (size + overhead + page_size - 1) & ~(page_size - 1);

  /* Primary mapping is RW, but request permission to switch to PROT_EXEC later. */
  prot = PROT_READ | PROT_WRITE | PROT_MPROTECT(PROT_EXEC);
  dataseg = mmap(NULL, rounded_size, prot, MAP_ANON | MAP_PRIVATE, -1, 0);
  if (dataseg == MAP_FAILED)
    return NULL;

  /* Create secondary mapping and switch it to RX. */
  codeseg = mremap(dataseg, rounded_size, NULL, rounded_size, MAP_REMAPDUP);
  if (codeseg == MAP_FAILED) {
    munmap(dataseg, rounded_size);
    return NULL;
  }
  if (mprotect(codeseg, rounded_size, PROT_READ | PROT_EXEC) == -1) {
    munmap(codeseg, rounded_size);
    munmap(dataseg, rounded_size);
    return NULL;
  }

  /* Remember allocation size and location of the secondary mapping for ffi_closure_free. */
  memcpy(dataseg, &rounded_size, sizeof(rounded_size));
  memcpy(ADD_TO_POINTER(dataseg, sizeof(size_t)), &codeseg, sizeof(void *));
  *code = ADD_TO_POINTER(codeseg, overhead);
  return ADD_TO_POINTER(dataseg, overhead);
}