WandExport PixelIterator *ClonePixelIterator(const PixelIterator *iterator)
{
  PixelIterator
    *clone_iterator;

  assert(iterator != (PixelIterator *) NULL);
  assert(iterator->signature == MagickWandSignature);
  if (iterator->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",iterator->name);
  clone_iterator=(PixelIterator *) AcquireMagickMemory(sizeof(*clone_iterator));
  if (clone_iterator == (PixelIterator *) NULL)
    ThrowWandFatalException(ResourceLimitFatalError,"MemoryAllocationFailed",
      iterator->name);
  (void) memset(clone_iterator,0,sizeof(*clone_iterator));
  clone_iterator->id=AcquireWandId();
  (void) FormatLocaleString(clone_iterator->name,MagickPathExtent,"%s-%.20g",
    PixelIteratorId,(double) clone_iterator->id);
  clone_iterator->exception=AcquireExceptionInfo();
  InheritException(clone_iterator->exception,iterator->exception);
  clone_iterator->view=CloneCacheView(iterator->view);
  clone_iterator->region=iterator->region;
  clone_iterator->active=iterator->active;
  clone_iterator->y=iterator->y;
  clone_iterator->pixel_wands=ClonePixelWands((const PixelWand **)
    iterator->pixel_wands,iterator->region.width);
  clone_iterator->debug=iterator->debug;
  if (clone_iterator->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",
      clone_iterator->name);
  clone_iterator->signature=MagickWandSignature;
  return(clone_iterator);
}