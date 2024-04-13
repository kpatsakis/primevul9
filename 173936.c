CpuMpPeimInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS           Status;

  //
  // For the sake of special initialization needing to be done right after
  // memory discovery.
  //
  Status = PeiServicesNotifyPpi (&mPostMemNotifyList[0]);
  ASSERT_EFI_ERROR (Status);

  return Status;
}