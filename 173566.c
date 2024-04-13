WandExport void ClearPixelIterator(PixelIterator *iterator)
{
  assert(iterator != (const PixelIterator *) NULL);
  assert(iterator->signature == MagickWandSignature);
  if (iterator->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",iterator->name);
  iterator->pixel_wands=DestroyPixelWands(iterator->pixel_wands,
    iterator->region.width);
  ClearMagickException(iterator->exception);
  iterator->pixel_wands=NewPixelWands(iterator->region.width);
  iterator->active=MagickFalse;
  iterator->y=0;
  iterator->debug=IsEventLogging();
}