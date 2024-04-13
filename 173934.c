PeiStartupAllAPs (
  IN  CONST EFI_PEI_SERVICES    **PeiServices,
  IN  EFI_PEI_MP_SERVICES_PPI   *This,
  IN  EFI_AP_PROCEDURE          Procedure,
  IN  BOOLEAN                   SingleThread,
  IN  UINTN                     TimeoutInMicroSeconds,
  IN  VOID                      *ProcedureArgument      OPTIONAL
  )
{
  return MpInitLibStartupAllAPs (
           Procedure,
           SingleThread,
           NULL,
           TimeoutInMicroSeconds,
           ProcedureArgument,
           NULL
           );
}