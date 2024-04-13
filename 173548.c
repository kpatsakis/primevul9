WandExport MagickBooleanType IsPixelIterator(const PixelIterator *iterator)
{
  size_t
    length;

  if (iterator == (const PixelIterator *) NULL)
    return(MagickFalse);
  if (iterator->signature != MagickWandSignature)
    return(MagickFalse);
  length=strlen(PixelIteratorId);
  if (LocaleNCompare(iterator->name,PixelIteratorId,length) != 0)
    return(MagickFalse);
  return(MagickTrue);
}