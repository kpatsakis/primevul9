GetMaxMemoryPage (
  IN  PAGE_ATTRIBUTE  TopLevelType
  )
{
  UINT32          RegEax;
  UINT32          RegEdx;

  if (TopLevelType == Page512G) {
    AsmCpuid (CPUID_EXTENDED_FUNCTION, &RegEax, NULL, NULL, NULL);
    if (RegEax >= CPUID_EXTENDED_CPU_SIG) {
      AsmCpuid (CPUID_EXTENDED_CPU_SIG, NULL, NULL, NULL, &RegEdx);
      if ((RegEdx & BIT26) != 0) {
        return Page1G;
      }
    }
  }

  return Page2M;
}