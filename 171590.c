static inline Quantum ScaleMapToQuantum(const MagickRealType value)
{
  if (value <= 0.0)
    return((Quantum) 0);
  if (value >= MaxMap)
    return(QuantumRange);
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) (value+0.5));
#else
  return((Quantum) value);
#endif
}