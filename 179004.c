MagickExport Image *MergeImageLayers(Image *image,const LayerMethod method,
  ExceptionInfo *exception)
{
#define MergeLayersTag  "Merge/Layers"

  Image
    *canvas;

  MagickBooleanType
    proceed;

  RectangleInfo
    page;

  register const Image
    *next;

  size_t
    number_images,
    height,
    width;

  ssize_t
    scene;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  /*
    Determine canvas image size, and its virtual canvas size and offset
  */
  page=image->page;
  width=image->columns;
  height=image->rows;
  switch (method)
  {
    case TrimBoundsLayer:
    case MergeLayer:
    default:
    {
      next=GetNextImageInList(image);
      for ( ; next != (Image *) NULL;  next=GetNextImageInList(next))
      {
        if (page.x > next->page.x)
          {
            width+=page.x-next->page.x;
            page.x=next->page.x;
          }
        if (page.y > next->page.y)
          {
            height+=page.y-next->page.y;
            page.y=next->page.y;
          }
        if ((ssize_t) width < (next->page.x+(ssize_t) next->columns-page.x))
          width=(size_t) next->page.x+(ssize_t) next->columns-page.x;
        if ((ssize_t) height < (next->page.y+(ssize_t) next->rows-page.y))
          height=(size_t) next->page.y+(ssize_t) next->rows-page.y;
      }
      break;
    }
    case FlattenLayer:
    {
      if (page.width > 0)
        width=page.width;
      if (page.height > 0)
        height=page.height;
      page.x=0;
      page.y=0;
      break;
    }
    case MosaicLayer:
    {
      if (page.width > 0)
        width=page.width;
      if (page.height > 0)
        height=page.height;
      for (next=image; next != (Image *) NULL; next=GetNextImageInList(next))
      {
        if (method == MosaicLayer)
          {
            page.x=next->page.x;
            page.y=next->page.y;
            if ((ssize_t) width < (next->page.x+(ssize_t) next->columns))
              width=(size_t) next->page.x+next->columns;
            if ((ssize_t) height < (next->page.y+(ssize_t) next->rows))
              height=(size_t) next->page.y+next->rows;
          }
      }
      page.width=width;
      page.height=height;
      page.x=0;
      page.y=0;
    }
    break;
  }
  /*
    Set virtual canvas size if not defined.
  */
  if (page.width == 0)
    page.width=page.x < 0 ? width : width+page.x;
  if (page.height == 0)
    page.height=page.y < 0 ? height : height+page.y;
  /*
    Handle "TrimBoundsLayer" method separately to normal 'layer merge'.
  */
  if (method == TrimBoundsLayer)
    {
      number_images=GetImageListLength(image);
      for (scene=0; scene < (ssize_t) number_images; scene++)
      {
        image->page.x-=page.x;
        image->page.y-=page.y;
        image->page.width=width;
        image->page.height=height;
        proceed=SetImageProgress(image,MergeLayersTag,(MagickOffsetType) scene,
          number_images);
        if (proceed == MagickFalse)
          break;
        image=GetNextImageInList(image);
        if (image == (Image *) NULL)
          break;
      }
      return((Image *) NULL);
    }
  /*
    Create canvas size of width and height, and background color.
  */
  canvas=CloneImage(image,width,height,MagickTrue,exception);
  if (canvas == (Image *) NULL)
    return((Image *) NULL);
  (void) SetImageBackgroundColor(canvas,exception);
  canvas->page=page;
  canvas->dispose=UndefinedDispose;
  /*
    Compose images onto canvas, with progress monitor
  */
  number_images=GetImageListLength(image);
  for (scene=0; scene < (ssize_t) number_images; scene++)
  {
    (void) CompositeImage(canvas,image,image->compose,MagickTrue,image->page.x-
      canvas->page.x,image->page.y-canvas->page.y,exception);
    proceed=SetImageProgress(image,MergeLayersTag,(MagickOffsetType) scene,
      number_images);
    if (proceed == MagickFalse)
      break;
    image=GetNextImageInList(image);
    if (image == (Image *) NULL)
      break;
  }
  return(canvas);
}