GetPhysicalAddressWidth (
  VOID
  )
{
  UINT32          RegEax;

  if (sizeof(UINTN) == 4) {
    return 32;
  }

  AsmCpuid(CPUID_EXTENDED_FUNCTION, &RegEax, NULL, NULL, NULL);
  if (RegEax >= CPUID_VIR_PHY_ADDRESS_SIZE) {
    AsmCpuid (CPUID_VIR_PHY_ADDRESS_SIZE, &RegEax, NULL, NULL, NULL);
    RegEax &= 0xFF;
    if (RegEax > 48) {
      return 48;
    }

    return (UINTN)RegEax;
  }

  return 36;
}