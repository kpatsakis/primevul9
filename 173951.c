InitializeExceptionStackSwitchHandlers (
  IN OUT VOID *Buffer
  )
{
  CPU_EXCEPTION_INIT_DATA           *EssData;
  IA32_DESCRIPTOR                   Idtr;
  EFI_STATUS                        Status;

  EssData = Buffer;
  //
  // We don't plan to replace IDT table with a new one, but we should not assume
  // the AP's IDT is the same as BSP's IDT either.
  //
  AsmReadIdtr (&Idtr);
  EssData->Ia32.IdtTable = (VOID *)Idtr.Base;
  EssData->Ia32.IdtTableSize = Idtr.Limit + 1;
  Status = InitializeCpuExceptionHandlersEx (NULL, EssData);
  ASSERT_EFI_ERROR (Status);
}