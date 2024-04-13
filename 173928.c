PeiWhoAmI (
  IN  CONST EFI_PEI_SERVICES   **PeiServices,
  IN  EFI_PEI_MP_SERVICES_PPI  *This,
  OUT UINTN                    *ProcessorNumber
  )
{
  return MpInitLibWhoAmI (ProcessorNumber);
}