GetPageTableTopLevelType (
  VOID
  )
{
  MSR_IA32_EFER_REGISTER      MsrEfer;

  MsrEfer.Uint64 = AsmReadMsr64 (MSR_CORE_IA32_EFER);

  return (MsrEfer.Bits.LMA == 1) ? Page512G : Page1G;
}