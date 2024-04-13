MagickExport Image *CompareImagesLayers(const Image *image,
  const LayerMethod method,ExceptionInfo *exception)
{
  Image
    *image_a,
    *image_b,
    *layers;

  RectangleInfo
    *bounds;

  register const Image
    *next;

  register ssize_t
    i;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  assert((method == CompareAnyLayer) ||
         (method == CompareClearLayer) ||
         (method == CompareOverlayLayer));
  /*
    Allocate bounds memory.
  */
  next=GetFirstImageInList(image);
  bounds=(RectangleInfo *) AcquireQuantumMemory((size_t)
    GetImageListLength(next),sizeof(*bounds));
  if (bounds == (RectangleInfo *) NULL)
    ThrowImageException(ResourceLimitError,"MemoryAllocationFailed");
  /*
    Set up first comparision images.
  */
  image_a=CloneImage(next,next->page.width,next->page.height,
    MagickTrue,exception);
  if (image_a == (Image *) NULL)
    {
      bounds=(RectangleInfo *) RelinquishMagickMemory(bounds);
      return((Image *) NULL);
    }
  image_a->background_color.alpha=(MagickRealType) TransparentAlpha;
  (void) SetImageBackgroundColor(image_a,exception);
  image_a->page=next->page;
  image_a->page.x=0;
  image_a->page.y=0;
  (void) CompositeImage(image_a,next,CopyCompositeOp,MagickTrue,next->page.x,
    next->page.y,exception);
  /*
    Compute the bounding box of changes for the later images
  */
  i=0;
  next=GetNextImageInList(next);
  for ( ; next != (const Image *) NULL; next=GetNextImageInList(next))
  {
    image_b=CloneImage(image_a,0,0,MagickTrue,exception);
    if (image_b == (Image *) NULL)
      {
        image_a=DestroyImage(image_a);
        bounds=(RectangleInfo *) RelinquishMagickMemory(bounds);
        return((Image *) NULL);
      }
    (void) CompositeImage(image_a,next,CopyCompositeOp,MagickTrue,next->page.x,
      next->page.y,exception);
    bounds[i]=CompareImagesBounds(image_b,image_a,method,exception);
    image_b=DestroyImage(image_b);
    i++;
  }
  image_a=DestroyImage(image_a);
  /*
    Clone first image in sequence.
  */
  next=GetFirstImageInList(image);
  layers=CloneImage(next,0,0,MagickTrue,exception);
  if (layers == (Image *) NULL)
    {
      bounds=(RectangleInfo *) RelinquishMagickMemory(bounds);
      return((Image *) NULL);
    }
  /*
    Deconstruct the image sequence.
  */
  i=0;
  next=GetNextImageInList(next);
  for ( ; next != (const Image *) NULL; next=GetNextImageInList(next))
  {
    if ((bounds[i].x == -1) && (bounds[i].y == -1) &&
        (bounds[i].width == 1) && (bounds[i].height == 1))
      {
        /*
          An empty frame is returned from CompareImageBounds(), which means the
          current frame is identical to the previous frame.
        */
        i++;
        continue;
      }
    image_a=CloneImage(next,0,0,MagickTrue,exception);
    if (image_a == (Image *) NULL)
      break;
    image_b=CropImage(image_a,&bounds[i],exception);
    image_a=DestroyImage(image_a);
    if (image_b == (Image *) NULL)
      break;
    AppendImageToList(&layers,image_b);
    i++;
  }
  bounds=(RectangleInfo *) RelinquishMagickMemory(bounds);
  if (next != (Image *) NULL)
    {
      layers=DestroyImageList(layers);
      return((Image *) NULL);
    }
  return(GetFirstImageInList(layers));
}