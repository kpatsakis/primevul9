WandExport PixelWand **PixelGetCurrentIteratorRow(PixelIterator *iterator,
  size_t *number_wands)
{
  register const Quantum
    *pixels;

  register ssize_t
    x;

  assert(iterator != (PixelIterator *) NULL);
  assert(iterator->signature == MagickWandSignature);
  if (iterator->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",iterator->name);
  *number_wands=0;
  iterator->active=MagickTrue;
  pixels=GetCacheViewVirtualPixels(iterator->view,iterator->region.x,
    iterator->region.y+iterator->y,iterator->region.width,1,
    iterator->exception);
  if (pixels == (const Quantum *) NULL)
    return((PixelWand **) NULL);
  for (x=0; x < (ssize_t) iterator->region.width; x++)
  {
    PixelSetQuantumPixel(GetCacheViewImage(iterator->view),pixels,
      iterator->pixel_wands[x]);
    pixels+=GetPixelChannels(GetCacheViewImage(iterator->view));
  }
  *number_wands=iterator->region.width;
  return(iterator->pixel_wands);
}