static inline Quantum ScaleAnyToQuantum(const QuantumAny quantum,
  const QuantumAny range)
{
  if (quantum > range)
    return(QuantumRange);
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) (((double) QuantumRange*quantum)*
    PerceptibleReciprocal((double) range)+0.5));
#else
  return((Quantum) (((double) QuantumRange*quantum)*
    PerceptibleReciprocal((double) range)));
#endif
}