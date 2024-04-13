SplitPage (
  IN  UINT64                            *PageEntry,
  IN  PAGE_ATTRIBUTE                    PageAttribute,
  IN  PAGE_ATTRIBUTE                    SplitAttribute,
  IN  BOOLEAN                           Recursively
  )
{
  UINT64            BaseAddress;
  UINT64            *NewPageEntry;
  UINTN             Index;
  UINT64            AddressEncMask;
  PAGE_ATTRIBUTE    SplitTo;

  if (SplitAttribute == PageNone || SplitAttribute >= PageAttribute) {
    ASSERT (SplitAttribute != PageNone);
    ASSERT (SplitAttribute < PageAttribute);
    return RETURN_INVALID_PARAMETER;
  }

  NewPageEntry = AllocatePageTableMemory (1);
  if (NewPageEntry == NULL) {
    ASSERT (NewPageEntry != NULL);
    return RETURN_OUT_OF_RESOURCES;
  }

  //
  // One level down each step to achieve more compact page table.
  //
  SplitTo = PageAttribute - 1;
  AddressEncMask = PcdGet64 (PcdPteMemoryEncryptionAddressOrMask) &
                   mPageAttributeTable[SplitTo].AddressMask;
  BaseAddress    = *PageEntry &
                   ~PcdGet64 (PcdPteMemoryEncryptionAddressOrMask) &
                   mPageAttributeTable[PageAttribute].AddressMask;
  for (Index = 0; Index < SIZE_4KB / sizeof(UINT64); Index++) {
    NewPageEntry[Index] = BaseAddress | AddressEncMask |
                          ((*PageEntry) & PAGE_PROGATE_BITS);

    if (SplitTo != PageMin) {
      NewPageEntry[Index] |= IA32_PG_PS;
    }

    if (Recursively && SplitTo > SplitAttribute) {
      SplitPage (&NewPageEntry[Index], SplitTo, SplitAttribute, Recursively);
    }

    BaseAddress += mPageAttributeTable[SplitTo].Length;
  }

  (*PageEntry) = (UINT64)(UINTN)NewPageEntry | AddressEncMask | PAGE_ATTRIBUTE_BITS;

  return RETURN_SUCCESS;
}