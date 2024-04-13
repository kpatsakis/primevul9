MagickExport void CompositeLayers(Image *destination,
  const CompositeOperator compose, Image *source,const ssize_t x_offset,
  const ssize_t y_offset,ExceptionInfo *exception)
{
  assert(destination != (Image *) NULL);
  assert(destination->signature == MagickCoreSignature);
  assert(source != (Image *) NULL);
  assert(source->signature == MagickCoreSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  if (source->debug != MagickFalse || destination->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s - %s",
      source->filename,destination->filename);

  /*
    Overlay single source image over destation image/list
  */
  if ( source->next == (Image *) NULL )
    while ( destination != (Image *) NULL )
    {
      CompositeCanvas(destination, compose, source, x_offset, y_offset,
        exception);
      destination=GetNextImageInList(destination);
    }

  /*
    Overlay source image list over single destination.
    Multiple clones of destination image are created to match source list.
    Original Destination image becomes first image of generated list.
    As such the image list pointer does not require any change in caller.
    Some animation attributes however also needs coping in this case.
  */
  else if ( destination->next == (Image *) NULL )
  {
    Image *dest = CloneImage(destination,0,0,MagickTrue,exception);

    CompositeCanvas(destination, compose, source, x_offset, y_offset,
      exception);
    /* copy source image attributes ? */
    if ( source->next != (Image *) NULL )
      {
        destination->delay = source->delay;
        destination->iterations = source->iterations;
      }
    source=GetNextImageInList(source);

    while ( source != (Image *) NULL )
    {
      AppendImageToList(&destination,
           CloneImage(dest,0,0,MagickTrue,exception));
      destination=GetLastImageInList(destination);

      CompositeCanvas(destination, compose, source, x_offset, y_offset,
        exception);
      destination->delay = source->delay;
      destination->iterations = source->iterations;
      source=GetNextImageInList(source);
    }
    dest=DestroyImage(dest);
  }

  /*
    Overlay a source image list over a destination image list
    until either list runs out of images. (Does not repeat)
  */
  else
    while ( source != (Image *) NULL && destination != (Image *) NULL )
    {
      CompositeCanvas(destination, compose, source, x_offset, y_offset,
        exception);
      source=GetNextImageInList(source);
      destination=GetNextImageInList(destination);
    }
}