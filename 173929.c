GetStackBase (
  IN OUT VOID *Buffer
  )
{
  EFI_PHYSICAL_ADDRESS    StackBase;

  StackBase = (EFI_PHYSICAL_ADDRESS)(UINTN)&StackBase;
  StackBase += BASE_4KB;
  StackBase &= ~((EFI_PHYSICAL_ADDRESS)BASE_4KB - 1);
  StackBase -= PcdGet32(PcdCpuApStackSize);

  *(EFI_PHYSICAL_ADDRESS *)Buffer = StackBase;
}