static inline void WriteVIPSPixel(Image *image, const Quantum value)
{
  if (image->depth == 16)
    (void) WriteBlobShort(image,ScaleQuantumToShort(value));
  else
    (void) WriteBlobByte(image,ScaleQuantumToChar(value));
}