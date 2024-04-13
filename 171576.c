static inline Quantum ScaleLongLongToQuantum(const MagickSizeType value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) ((value)/MagickULLConstant(281479271743489)));
#else
  return((Quantum) (value/281479271743489.0));
#endif
}