MemoryDiscoveredPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS      Status;
  BOOLEAN         InitStackGuard;

  //
  // Paging must be setup first. Otherwise the exception TSS setup during MP
  // initialization later will not contain paging information and then fail
  // the task switch (for the sake of stack switch).
  //
  InitStackGuard = FALSE;
  if (IsIa32PaeSupported () && PcdGetBool (PcdCpuStackGuard)) {
    EnablePaging ();
    InitStackGuard = TRUE;
  }

  Status = InitializeCpuMpWorker ((CONST EFI_PEI_SERVICES **)PeiServices);
  ASSERT_EFI_ERROR (Status);

  if (InitStackGuard) {
    SetupStackGuardPage ();
  }

  return Status;
}