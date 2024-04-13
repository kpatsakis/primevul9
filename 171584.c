static inline Quantum ScaleLongLongToQuantum(const MagickSizeType value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) (value/MagickULLConstant(72340172838076673)));
#else
  return((Quantum) (value/72340172838076673.0));
#endif
}