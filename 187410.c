MagickExport MagickBooleanType CompressImageColormap(Image *image)
{
  QuantizeInfo
    quantize_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (IsPaletteImage(image,&image->exception) == MagickFalse)
    return(MagickFalse);
  GetQuantizeInfo(&quantize_info);
  quantize_info.number_colors=image->colors;
  quantize_info.tree_depth=MaxTreeDepth;
  return(QuantizeImage(&quantize_info,image));
}