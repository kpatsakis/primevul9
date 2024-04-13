static inline Quantum ScaleCharToQuantum(const unsigned char value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) (257U*value));
#else
  return((Quantum) (257.0*value));
#endif
}