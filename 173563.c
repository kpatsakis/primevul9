WandExport PixelIterator *NewPixelIterator(MagickWand *wand)
{
  const char
    *quantum;

  ExceptionInfo
    *exception;

  Image
    *image;

  PixelIterator
    *iterator;

  size_t
    depth;

  CacheView
    *view;

  depth=MAGICKCORE_QUANTUM_DEPTH;
  quantum=GetMagickQuantumDepth(&depth);
  if (depth != MAGICKCORE_QUANTUM_DEPTH)
    ThrowWandFatalException(WandError,"QuantumDepthMismatch",quantum);
  assert(wand != (MagickWand *) NULL);
  image=GetImageFromMagickWand(wand);
  if (image == (Image *) NULL)
    return((PixelIterator *) NULL);
  exception=AcquireExceptionInfo();
  view=AcquireVirtualCacheView(image,exception);
  if (view == (CacheView *) NULL)
    return((PixelIterator *) NULL);
  iterator=(PixelIterator *) AcquireMagickMemory(sizeof(*iterator));
  if (iterator == (PixelIterator *) NULL)
    ThrowWandFatalException(ResourceLimitFatalError,"MemoryAllocationFailed",
      GetExceptionMessage(errno));
  (void) memset(iterator,0,sizeof(*iterator));
  iterator->id=AcquireWandId();
  (void) FormatLocaleString(iterator->name,MagickPathExtent,"%s-%.20g",
    PixelIteratorId,(double) iterator->id);
  iterator->exception=exception;
  iterator->view=view;
  SetGeometry(image,&iterator->region);
  iterator->region.width=image->columns;
  iterator->region.height=image->rows;
  iterator->region.x=0;
  iterator->region.y=0;
  iterator->pixel_wands=NewPixelWands(iterator->region.width);
  iterator->y=0;
  iterator->debug=IsEventLogging();
  if (iterator->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",iterator->name);
  iterator->signature=MagickWandSignature;
  return(iterator);
}