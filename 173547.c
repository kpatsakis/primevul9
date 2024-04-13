WandExport void PixelSetLastIteratorRow(PixelIterator *iterator)
{
  assert(iterator != (PixelIterator *) NULL);
  assert(iterator->signature == MagickWandSignature);
  if (iterator->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",iterator->name);
  iterator->active=MagickFalse;
  iterator->y=(ssize_t) iterator->region.height-1;
}