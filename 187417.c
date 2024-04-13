static DoublePixelPacket **AcquirePixelThreadSet(const size_t count)
{
  DoublePixelPacket
    **pixels;

  register ssize_t
    i;

  size_t
    number_threads;

  number_threads=(size_t) GetMagickResourceLimit(ThreadResource);
  pixels=(DoublePixelPacket **) AcquireQuantumMemory(number_threads,
    sizeof(*pixels));
  if (pixels == (DoublePixelPacket **) NULL)
    return((DoublePixelPacket **) NULL);
  (void) memset(pixels,0,number_threads*sizeof(*pixels));
  for (i=0; i < (ssize_t) number_threads; i++)
  {
    pixels[i]=(DoublePixelPacket *) AcquireQuantumMemory(count,
      2*sizeof(**pixels));
    if (pixels[i] == (DoublePixelPacket *) NULL)
      return(DestroyPixelThreadSet(pixels));
  }
  return(pixels);
}