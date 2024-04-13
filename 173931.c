InitializeMpExceptionStackSwitchHandlers (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           Index;
  UINTN                           Bsp;
  UINTN                           ExceptionNumber;
  UINTN                           OldGdtSize;
  UINTN                           NewGdtSize;
  UINTN                           NewStackSize;
  IA32_DESCRIPTOR                 Gdtr;
  CPU_EXCEPTION_INIT_DATA         EssData;
  UINT8                           *GdtBuffer;
  UINT8                           *StackTop;
  UINTN                           NumberOfProcessors;

  if (!PcdGetBool (PcdCpuStackGuard)) {
    return;
  }

  MpInitLibGetNumberOfProcessors(&NumberOfProcessors, NULL);
  MpInitLibWhoAmI (&Bsp);

  ExceptionNumber = FixedPcdGetSize (PcdCpuStackSwitchExceptionList);
  NewStackSize = FixedPcdGet32 (PcdCpuKnownGoodStackSize) * ExceptionNumber;

  Status = PeiServicesAllocatePool (
             NewStackSize * NumberOfProcessors,
             (VOID **)&StackTop
             );
  ASSERT(StackTop != NULL);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return;
  }
  StackTop += NewStackSize  * NumberOfProcessors;

  //
  // The default exception handlers must have been initialized. Let's just skip
  // it in this method.
  //
  EssData.Ia32.Revision = CPU_EXCEPTION_INIT_DATA_REV;
  EssData.Ia32.InitDefaultHandlers = FALSE;

  EssData.Ia32.StackSwitchExceptions = FixedPcdGetPtr(PcdCpuStackSwitchExceptionList);
  EssData.Ia32.StackSwitchExceptionNumber = ExceptionNumber;
  EssData.Ia32.KnownGoodStackSize = FixedPcdGet32(PcdCpuKnownGoodStackSize);

  //
  // Initialize Gdtr to suppress incorrect compiler/analyzer warnings.
  //
  Gdtr.Base = 0;
  Gdtr.Limit = 0;
  for (Index = 0; Index < NumberOfProcessors; ++Index) {
    //
    // To support stack switch, we need to re-construct GDT but not IDT.
    //
    if (Index == Bsp) {
      GetGdtr(&Gdtr);
    } else {
      //
      // AP might have different size of GDT from BSP.
      //
      MpInitLibStartupThisAP (GetGdtr, Index, NULL, 0, (VOID *)&Gdtr, NULL);
    }

    //
    // X64 needs only one TSS of current task working for all exceptions
    // because of its IST feature. IA32 needs one TSS for each exception
    // in addition to current task. Since AP is not supposed to allocate
    // memory, we have to do it in BSP. To simplify the code, we allocate
    // memory for IA32 case to cover both IA32 and X64 exception stack
    // switch.
    //
    // Layout of memory to allocate for each processor:
    //    --------------------------------
    //    |            Alignment         |  (just in case)
    //    --------------------------------
    //    |                              |
    //    |        Original GDT          |
    //    |                              |
    //    --------------------------------
    //    |    Current task descriptor   |
    //    --------------------------------
    //    |                              |
    //    |  Exception task descriptors  |  X ExceptionNumber
    //    |                              |
    //    --------------------------------
    //    |  Current task-state segment  |
    //    --------------------------------
    //    |                              |
    //    | Exception task-state segment |  X ExceptionNumber
    //    |                              |
    //    --------------------------------
    //
    OldGdtSize = Gdtr.Limit + 1;
    EssData.Ia32.ExceptionTssDescSize = sizeof (IA32_TSS_DESCRIPTOR) *
                                        (ExceptionNumber + 1);
    EssData.Ia32.ExceptionTssSize = sizeof (IA32_TASK_STATE_SEGMENT) *
                                    (ExceptionNumber + 1);
    NewGdtSize = sizeof (IA32_TSS_DESCRIPTOR) +
                 OldGdtSize +
                 EssData.Ia32.ExceptionTssDescSize +
                 EssData.Ia32.ExceptionTssSize;

    Status = PeiServicesAllocatePool (
               NewGdtSize,
               (VOID **)&GdtBuffer
               );
    ASSERT (GdtBuffer != NULL);
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return;
    }

    //
    // Make sure GDT table alignment
    //
    EssData.Ia32.GdtTable = ALIGN_POINTER(GdtBuffer, sizeof (IA32_TSS_DESCRIPTOR));
    NewGdtSize -= ((UINT8 *)EssData.Ia32.GdtTable - GdtBuffer);
    EssData.Ia32.GdtTableSize = NewGdtSize;

    EssData.Ia32.ExceptionTssDesc = ((UINT8 *)EssData.Ia32.GdtTable + OldGdtSize);
    EssData.Ia32.ExceptionTss = ((UINT8 *)EssData.Ia32.GdtTable + OldGdtSize +
                                 EssData.Ia32.ExceptionTssDescSize);

    EssData.Ia32.KnownGoodStackTop = (UINTN)StackTop;
    DEBUG ((DEBUG_INFO,
            "Exception stack top[cpu%lu]: 0x%lX\n",
            (UINT64)(UINTN)Index,
            (UINT64)(UINTN)StackTop));

    if (Index == Bsp) {
      InitializeExceptionStackSwitchHandlers (&EssData);
    } else {
      MpInitLibStartupThisAP (
        InitializeExceptionStackSwitchHandlers,
        Index,
        NULL,
        0,
        (VOID *)&EssData,
        NULL
        );
    }

    StackTop  -= NewStackSize;
  }
}