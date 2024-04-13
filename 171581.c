static inline Quantum ScaleMapToQuantum(const MagickRealType value)
{
  if (value <= 0.0)
    return((Quantum) 0);
  if (value >= (Quantum) MaxMap)
    return(QuantumRange);
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) (65537.0*value+0.5));
#else
  return((Quantum) (65537.0*value));
#endif
}