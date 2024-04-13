MagickExport void OptimizeImageTransparency(const Image *image,
     ExceptionInfo *exception)
{
  Image
    *dispose_image;

  register Image
    *next;

  /*
    Run the image through the animation sequence
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  next=GetFirstImageInList(image);
  dispose_image=CloneImage(next,next->page.width,next->page.height,
    MagickTrue,exception);
  if (dispose_image == (Image *) NULL)
    return;
  dispose_image->page=next->page;
  dispose_image->page.x=0;
  dispose_image->page.y=0;
  dispose_image->dispose=NoneDispose;
  dispose_image->background_color.alpha_trait=BlendPixelTrait;
  dispose_image->background_color.alpha=(MagickRealType) TransparentAlpha;
  (void) SetImageBackgroundColor(dispose_image,exception);

  while ( next != (Image *) NULL )
  {
    Image
      *current_image;

    /*
      Overlay this frame's image over the previous disposal image
    */
    current_image=CloneImage(dispose_image,0,0,MagickTrue,exception);
    if (current_image == (Image *) NULL)
      {
        dispose_image=DestroyImage(dispose_image);
        return;
      }
    (void) CompositeImage(current_image,next,next->alpha_trait != UndefinedPixelTrait ?
      OverCompositeOp : CopyCompositeOp,MagickTrue,next->page.x,next->page.y,
      exception);
    /*
      At this point the image would be displayed, for the delay period
    **
      Work out the disposal of the previous image
    */
    if (next->dispose == BackgroundDispose)
      {
        RectangleInfo
          bounds=next->page;

        bounds.width=next->columns;
        bounds.height=next->rows;
        if (bounds.x < 0)
          {
            bounds.width+=bounds.x;
            bounds.x=0;
          }
        if ((ssize_t) (bounds.x+bounds.width) > (ssize_t) current_image->columns)
          bounds.width=current_image->columns-bounds.x;
        if (bounds.y < 0)
          {
            bounds.height+=bounds.y;
            bounds.y=0;
          }
        if ((ssize_t) (bounds.y+bounds.height) > (ssize_t) current_image->rows)
          bounds.height=current_image->rows-bounds.y;
        ClearBounds(current_image,&bounds,exception);
      }
    if (next->dispose != PreviousDispose)
      {
        dispose_image=DestroyImage(dispose_image);
        dispose_image=current_image;
      }
    else
      current_image=DestroyImage(current_image);

    /*
      Optimize Transparency of the next frame (if present)
    */
    next=GetNextImageInList(next);
    if (next != (Image *) NULL) {
      (void) CompositeImage(next,dispose_image,ChangeMaskCompositeOp,
        MagickTrue,-(next->page.x),-(next->page.y),exception);
    }
  }
  dispose_image=DestroyImage(dispose_image);
  return;
}