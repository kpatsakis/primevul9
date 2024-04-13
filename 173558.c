WandExport ssize_t PixelGetIteratorRow(PixelIterator *iterator)
{
  assert(iterator != (const PixelIterator *) NULL);
  assert(iterator->signature == MagickWandSignature);
  if (iterator->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",iterator->name);
  return(iterator->y);
}