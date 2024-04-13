SetupStackGuardPage (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS        Hob;
  EFI_PHYSICAL_ADDRESS        StackBase;
  UINTN                       NumberOfProcessors;
  UINTN                       Bsp;
  UINTN                       Index;

  //
  // One extra page at the bottom of the stack is needed for Guard page.
  //
  if (PcdGet32(PcdCpuApStackSize) <= EFI_PAGE_SIZE) {
    DEBUG ((DEBUG_ERROR, "PcdCpuApStackSize is not big enough for Stack Guard!\n"));
    ASSERT (FALSE);
  }

  MpInitLibGetNumberOfProcessors(&NumberOfProcessors, NULL);
  MpInitLibWhoAmI (&Bsp);
  for (Index = 0; Index < NumberOfProcessors; ++Index) {
    if (Index == Bsp) {
      Hob.Raw = GetHobList ();
      while ((Hob.Raw = GetNextHob (EFI_HOB_TYPE_MEMORY_ALLOCATION, Hob.Raw)) != NULL) {
        if (CompareGuid (&gEfiHobMemoryAllocStackGuid,
                         &(Hob.MemoryAllocationStack->AllocDescriptor.Name))) {
          StackBase = Hob.MemoryAllocationStack->AllocDescriptor.MemoryBaseAddress;
          break;
        }
        Hob.Raw = GET_NEXT_HOB (Hob);
      }
    } else {
      //
      // Ask AP to return is stack base address.
      //
      MpInitLibStartupThisAP(GetStackBase, Index, NULL, 0, (VOID *)&StackBase, NULL);
    }
    //
    // Set Guard page at stack base address.
    //
    ConvertMemoryPageAttributes(StackBase, EFI_PAGE_SIZE, 0);
    DEBUG ((DEBUG_INFO, "Stack Guard set at %lx [cpu%lu]!\n",
            (UINT64)StackBase, (UINT64)Index));
  }

  //
  // Publish the changes of page table.
  //
  CpuFlushTlb ();
}