PeiGetProcessorInfo (
  IN  CONST EFI_PEI_SERVICES     **PeiServices,
  IN  EFI_PEI_MP_SERVICES_PPI    *This,
  IN  UINTN                      ProcessorNumber,
  OUT EFI_PROCESSOR_INFORMATION  *ProcessorInfoBuffer
  )
{
  return MpInitLibGetProcessorInfo (ProcessorNumber, ProcessorInfoBuffer, NULL);
}