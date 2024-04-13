static inline Quantum ScaleCharToQuantum(const unsigned char value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) (16843009UL*value));
#else
  return((Quantum) (16843009.0*value));
#endif
}