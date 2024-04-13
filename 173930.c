PeiStartupThisAP (
  IN  CONST EFI_PEI_SERVICES    **PeiServices,
  IN  EFI_PEI_MP_SERVICES_PPI   *This,
  IN  EFI_AP_PROCEDURE          Procedure,
  IN  UINTN                     ProcessorNumber,
  IN  UINTN                     TimeoutInMicroseconds,
  IN  VOID                      *ProcedureArgument      OPTIONAL
  )
{
  return MpInitLibStartupThisAP (
           Procedure,
           ProcessorNumber,
           NULL,
           TimeoutInMicroseconds,
           ProcedureArgument,
           NULL
           );
}