static inline MagickSizeType GetQuantumRange(const size_t depth)
{
  MagickSizeType
    one;

  size_t
    max_depth;

  if (depth == 0)
    return(0);
  one=1;
  max_depth=8*sizeof(MagickSizeType);
  return((MagickSizeType) ((one << (MagickMin(depth,max_depth)-1))+
    ((one << (MagickMin(depth,max_depth)-1))-1)));
}