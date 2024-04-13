static int IntensityCompare(const void *x,const void *y)
{
  PixelPacket
    *color_1,
    *color_2;

  int
    intensity;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  intensity=PixelPacketIntensity(color_1)-(int) PixelPacketIntensity(color_2);
  return((int) intensity);
}