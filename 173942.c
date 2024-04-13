IsIa32PaeSupported (
  VOID
  )
{
  UINT32                    RegEax;
  CPUID_VERSION_INFO_EDX    RegEdx;

  AsmCpuid (CPUID_SIGNATURE, &RegEax, NULL, NULL, NULL);
  if (RegEax >= CPUID_VERSION_INFO) {
    AsmCpuid (CPUID_VERSION_INFO, NULL, NULL, NULL, &RegEdx.Uint32);
    if (RegEdx.Bits.PAE != 0) {
      return TRUE;
    }
  }

  return FALSE;
}