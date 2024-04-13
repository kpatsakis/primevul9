PeiGetNumberOfProcessors (
  IN  CONST EFI_PEI_SERVICES    **PeiServices,
  IN  EFI_PEI_MP_SERVICES_PPI   *This,
  OUT UINTN                     *NumberOfProcessors,
  OUT UINTN                     *NumberOfEnabledProcessors
  )
{
  if ((NumberOfProcessors == NULL) || (NumberOfEnabledProcessors == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  return MpInitLibGetNumberOfProcessors (
           NumberOfProcessors,
           NumberOfEnabledProcessors
           );
}