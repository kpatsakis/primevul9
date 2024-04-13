static inline Quantum ScaleLongToQuantum(const unsigned int value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) ((value)/MagickULLConstant(65537)));
#else
  return((Quantum) (value/65537.0));
#endif
}