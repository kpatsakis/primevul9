PeiSwitchBSP (
  IN  CONST EFI_PEI_SERVICES   **PeiServices,
  IN  EFI_PEI_MP_SERVICES_PPI  *This,
  IN  UINTN                    ProcessorNumber,
  IN  BOOLEAN                  EnableOldBSP
  )
{
  return MpInitLibSwitchBSP (ProcessorNumber, EnableOldBSP);
}