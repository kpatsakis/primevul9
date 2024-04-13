static inline size_t ColorToNodeId(const CubeInfo *cube_info,
  const DoublePixelPacket *pixel,size_t index)
{
  size_t
    id;

  id=(size_t) (((ScaleQuantumToChar(ClampPixel(GetPixelRed(pixel))) >> index) &
    0x01) | ((ScaleQuantumToChar(ClampPixel(GetPixelGreen(pixel))) >> index) &
    0x01) << 1 | ((ScaleQuantumToChar(ClampPixel(GetPixelBlue(pixel))) >>
    index) & 0x01) << 2);
  if (cube_info->associate_alpha != MagickFalse)
    id|=((ScaleQuantumToChar(ClampPixel(GetPixelOpacity(pixel))) >> index) &
      0x1) << 3;
  return(id);
}