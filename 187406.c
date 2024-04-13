static inline void SetAssociatedAlpha(const Image *image,CubeInfo *cube_info)
{
  MagickBooleanType
    associate_alpha;

  associate_alpha=image->matte;
  if ((cube_info->quantize_info->number_colors == 2) &&
      ((cube_info->quantize_info->colorspace == LinearGRAYColorspace) ||
       (cube_info->quantize_info->colorspace == GRAYColorspace)))
    associate_alpha=MagickFalse;
  cube_info->associate_alpha=associate_alpha;
}