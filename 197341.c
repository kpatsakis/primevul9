static inline Quantum ScaleXToQuantum(const size_t x,
  const size_t scale)
{
  return((Quantum) (((double) QuantumRange*x)/scale+0.5));
}