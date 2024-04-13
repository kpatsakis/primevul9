WandExport MagickBooleanType IsDrawingWand(const DrawingWand *wand)
{
  if (wand == (const DrawingWand *) NULL)
    return(MagickFalse);
  if (wand->signature != MagickWandSignature)
    return(MagickFalse);
  if (LocaleNCompare(wand->name,DrawingWandId,strlen(DrawingWandId)) != 0)
    return(MagickFalse);
  return(MagickTrue);
}