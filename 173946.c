GetPageTableEntry (
  IN  PHYSICAL_ADDRESS                  Address,
  OUT PAGE_ATTRIBUTE                    *PageAttribute
  )
{
  INTN                  Level;
  UINTN                 Index;
  UINT64                *PageTable;
  UINT64                AddressEncMask;

  AddressEncMask = PcdGet64 (PcdPteMemoryEncryptionAddressOrMask);
  PageTable = (UINT64 *)(UINTN)(AsmReadCr3 () & PAGING_4K_ADDRESS_MASK_64);
  for (Level = (INTN)GetPageTableTopLevelType (); Level > 0; --Level) {
    Index = (UINTN)RShiftU64 (Address, mPageAttributeTable[Level].AddressBitOffset);
    Index &= PAGING_PAE_INDEX_MASK;

    //
    // No mapping?
    //
    if (PageTable[Index] == 0) {
      *PageAttribute = PageNone;
      return NULL;
    }

    //
    // Page memory?
    //
    if ((PageTable[Index] & IA32_PG_PS) != 0 || Level == PageMin) {
      *PageAttribute = (PAGE_ATTRIBUTE)Level;
      return &PageTable[Index];
    }

    //
    // Page directory or table
    //
    PageTable = (UINT64 *)(UINTN)(PageTable[Index] &
                                  ~AddressEncMask &
                                  PAGING_4K_ADDRESS_MASK_64);
  }

  *PageAttribute = PageNone;
  return NULL;
}