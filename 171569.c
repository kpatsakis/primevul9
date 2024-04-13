static inline Quantum ScaleShortToQuantum(const unsigned short value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) ((value+128U)/257U));
#else
  return((Quantum) (value/257.0));
#endif
}