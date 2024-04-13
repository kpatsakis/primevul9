AllocatePageTableMemory (
  IN UINTN           Pages
  )
{
  VOID      *Address;

  Address = AllocatePages(Pages);
  if (Address != NULL) {
    ZeroMem(Address, EFI_PAGES_TO_SIZE (Pages));
  }

  return Address;
}