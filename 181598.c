MagickExport Image *ConsolidateCMYKImages(const Image *images,
  ExceptionInfo *exception)
{
  CacheView
    *cmyk_view,
    *image_view;

  Image
    *cmyk_image,
    *cmyk_images;

  register ssize_t
    j;

  ssize_t
    y;

  /*
    Consolidate separate C, M, Y, and K planes into a single image.
  */
  assert(images != (Image *) NULL);
  assert(images->signature == MagickCoreSignature);
  if (images->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",images->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  cmyk_images=NewImageList();
  for (j=0; j < (ssize_t) GetImageListLength(images); j+=4)
  {
    register ssize_t
      i;

    assert(images != (Image *) NULL);
    cmyk_image=CloneImage(images,0,0,MagickTrue,
      exception);
    if (cmyk_image == (Image *) NULL)
      break;
    if (SetImageStorageClass(cmyk_image,DirectClass,exception) == MagickFalse)
      break;
    (void) SetImageColorspace(cmyk_image,CMYKColorspace,exception);
    for (i=0; i < 4; i++)
    {
      image_view=AcquireVirtualCacheView(images,exception);
      cmyk_view=AcquireAuthenticCacheView(cmyk_image,exception);
      for (y=0; y < (ssize_t) images->rows; y++)
      {
        register const Quantum
          *magick_restrict p;

        register ssize_t
          x;

        register Quantum
          *magick_restrict q;

        p=GetCacheViewVirtualPixels(image_view,0,y,images->columns,1,exception);
        q=QueueCacheViewAuthenticPixels(cmyk_view,0,y,cmyk_image->columns,1,
          exception);
        if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
          break;
        for (x=0; x < (ssize_t) images->columns; x++)
        {
          Quantum
            pixel;

          pixel=ClampToQuantum(QuantumRange-GetPixelIntensity(images,p));
          switch (i)
          {
            case 0: SetPixelCyan(cmyk_image,pixel,q);  break;
            case 1: SetPixelMagenta(cmyk_image,pixel,q);  break;
            case 2: SetPixelYellow(cmyk_image,pixel,q);  break;
            case 3: SetPixelBlack(cmyk_image,pixel,q);  break;
            default: break;
          }
          p+=GetPixelChannels(images);
          q+=GetPixelChannels(cmyk_image);
        }
        if (SyncCacheViewAuthenticPixels(cmyk_view,exception) == MagickFalse)
          break;
      }
      cmyk_view=DestroyCacheView(cmyk_view);
      image_view=DestroyCacheView(image_view);
      images=GetNextImageInList(images);
      if (images == (Image *) NULL)
        break;
    }
    AppendImageToList(&cmyk_images,cmyk_image);
  }
  return(cmyk_images);
}