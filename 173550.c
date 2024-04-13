WandExport PixelIterator *NewPixelRegionIterator(MagickWand *wand,
  const ssize_t x,const ssize_t y,const size_t width,const size_t height)
{
  CacheView
    *view;

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

  assert(wand != (MagickWand *) NULL);
  depth=MAGICKCORE_QUANTUM_DEPTH;
  quantum=GetMagickQuantumDepth(&depth);
  if (depth != MAGICKCORE_QUANTUM_DEPTH)
    ThrowWandFatalException(WandError,"QuantumDepthMismatch",quantum);
  if ((width == 0) || (height == 0))
    ThrowWandFatalException(WandError,"ZeroRegionSize",quantum);
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
      wand->name);
  (void) memset(iterator,0,sizeof(*iterator));
  iterator->id=AcquireWandId();
  (void) FormatLocaleString(iterator->name,MagickPathExtent,"%s-%.20g",
    PixelIteratorId,(double) iterator->id);
  iterator->exception=exception;
  iterator->view=view;
  SetGeometry(image,&iterator->region);
  iterator->region.width=width;
  iterator->region.height=height;
  iterator->region.x=x;
  iterator->region.y=y;
  iterator->pixel_wands=NewPixelWands(iterator->region.width);
  iterator->y=0;
  iterator->debug=IsEventLogging();
  if (iterator->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",iterator->name);
  iterator->signature=MagickWandSignature;
  return(iterator);
}