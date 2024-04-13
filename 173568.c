WandExport MagickBooleanType PixelClearIteratorException(
  PixelIterator *iterator)
{
  assert(iterator != (PixelIterator *) NULL);
  assert(iterator->signature == MagickWandSignature);
  if (iterator->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",iterator->name);
  ClearMagickException(iterator->exception);
  return(MagickTrue);
}