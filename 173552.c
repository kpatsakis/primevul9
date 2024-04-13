WandExport MagickBooleanType PixelSetIteratorRow(PixelIterator *iterator,
  const ssize_t row)
{
  assert(iterator != (const PixelIterator *) NULL);
  assert(iterator->signature == MagickWandSignature);
  if (iterator->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",iterator->name);
  if ((row < 0) || (row >= (ssize_t) iterator->region.height))
    return(MagickFalse);
  iterator->active=MagickTrue;
  iterator->y=row;
  return(MagickTrue);
}