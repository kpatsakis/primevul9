static inline ssize_t CacheOffset(CubeInfo *cube_info,
  const DoublePixelPacket *pixel)
{
#define RedShift(pixel) (((pixel) >> CacheShift) << (0*(8-CacheShift)))
#define GreenShift(pixel) (((pixel) >> CacheShift) << (1*(8-CacheShift)))
#define BlueShift(pixel) (((pixel) >> CacheShift) << (2*(8-CacheShift)))
#define AlphaShift(pixel) (((pixel) >> CacheShift) << (3*(8-CacheShift)))

  ssize_t
    offset;

  offset=(ssize_t) (RedShift(ScaleQuantumToChar(ClampPixel(pixel->red))) |
    GreenShift(ScaleQuantumToChar(ClampPixel(pixel->green))) |
    BlueShift(ScaleQuantumToChar(ClampPixel(pixel->blue))));
  if (cube_info->associate_alpha != MagickFalse)
    offset|=AlphaShift(ScaleQuantumToChar(ClampPixel(pixel->opacity)));
  return(offset);
}