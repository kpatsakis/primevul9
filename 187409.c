static void ReduceImageColors(const Image *image,CubeInfo *cube_info)
{
#define ReduceImageTag  "Reduce/Image"

  MagickBooleanType
    proceed;

  MagickOffsetType
    offset;

  size_t
    span;

  cube_info->next_threshold=0.0;
  if (cube_info->colors > cube_info->maximum_colors)
    {
      MagickRealType
        *quantize_error;

      /*
        Enable rapid reduction of the number of unique colors.
      */
      quantize_error=(MagickRealType *) AcquireQuantumMemory(cube_info->nodes,
        sizeof(*quantize_error));
      if (quantize_error != (MagickRealType *) NULL)
        {
          (void) QuantizeErrorFlatten(cube_info,cube_info->root,0,
            quantize_error);
          qsort(quantize_error,cube_info->nodes,sizeof(MagickRealType),
            MagickRealTypeCompare);
          if (cube_info->nodes > (110*(cube_info->maximum_colors+1)/100))
            cube_info->next_threshold=quantize_error[cube_info->nodes-110*
              (cube_info->maximum_colors+1)/100];
          quantize_error=(MagickRealType *) RelinquishMagickMemory(
            quantize_error);
        }
  }
  for (span=cube_info->colors; cube_info->colors > cube_info->maximum_colors; )
  {
    cube_info->pruning_threshold=cube_info->next_threshold;
    cube_info->next_threshold=cube_info->root->quantize_error-1;
    cube_info->colors=0;
    Reduce(cube_info,cube_info->root);
    offset=(MagickOffsetType) span-cube_info->colors;
    proceed=SetImageProgress(image,ReduceImageTag,offset,span-
      cube_info->maximum_colors+1);
    if (proceed == MagickFalse)
      break;
  }
}