static inline Quantum ScaleLongLongToQuantum(const MagickSizeType value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) ((value)/MagickULLConstant(4294967297)));
#else
  return((Quantum) (value/4294967297.0));
#endif
}