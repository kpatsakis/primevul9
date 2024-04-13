static inline Quantum ScaleLongToQuantum(const unsigned int value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) ((value)/16843009UL));
#else
  return((Quantum) (value/16843009.0));
#endif
}