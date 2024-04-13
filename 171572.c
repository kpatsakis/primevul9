static inline Quantum ScaleMapToQuantum(const MagickRealType value)
{
  if (value <= 0.0)
    return((Quantum) 0);
  if (value >= MaxMap)
    return(QuantumRange);
  return((Quantum) (281479271743489.0*value));
}