WandExport PixelIterator *DestroyPixelIterator(PixelIterator *iterator)
{
  assert(iterator != (const PixelIterator *) NULL);
  assert(iterator->signature == MagickWandSignature);
  if (iterator->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",iterator->name);
  iterator->view=DestroyCacheView(iterator->view);
  iterator->pixel_wands=DestroyPixelWands(iterator->pixel_wands,
    iterator->region.width);
  iterator->exception=DestroyExceptionInfo(iterator->exception);
  iterator->signature=(~MagickWandSignature);
  RelinquishWandId(iterator->id);
  iterator=(PixelIterator *) RelinquishMagickMemory(iterator);
  return(iterator);
}