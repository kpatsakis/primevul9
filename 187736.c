WandExport MagickBooleanType DrawComposite(DrawingWand *wand,
  const CompositeOperator compose,const double x,const double y,
  const double width,const double height,MagickWand *magick_wand)
{
  char
    *base64,
    *media_type;

  const char
    *mode;

  ImageInfo
    *image_info;

  Image
    *clone_image,
    *image;

  register char
    *p;

  register ssize_t
    i;

  size_t
    blob_length,
    encoded_length;

  unsigned char
    *blob;

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  assert(magick_wand != (MagickWand *) NULL);
  image=GetImageFromMagickWand(magick_wand);
  if (image == (Image *) NULL)
    return(MagickFalse);
  clone_image=CloneImage(image,0,0,MagickTrue,wand->exception);
  if (clone_image == (Image *) NULL)
    return(MagickFalse);
  image_info=AcquireImageInfo();
  (void) CopyMagickString(image_info->magick,"MIFF",MagickPathExtent);
  blob_length=2048;
  blob=(unsigned char *) ImageToBlob(image_info,clone_image,&blob_length,
    wand->exception);
  image_info=DestroyImageInfo(image_info);
  clone_image=DestroyImageList(clone_image);
  if (blob == (void *) NULL)
    return(MagickFalse);
  encoded_length=0;
  base64=Base64Encode(blob,blob_length,&encoded_length);
  blob=(unsigned char *) RelinquishMagickMemory(blob);
  if (base64 == (char *) NULL)
    {
      char
        buffer[MagickPathExtent];

      (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g bytes",(double)
        (4L*blob_length/3L+4L));
      ThrowDrawException(ResourceLimitWarning,"MemoryAllocationFailed",
        wand->name);
      return(MagickFalse);
    }
  mode=CommandOptionToMnemonic(MagickComposeOptions,(ssize_t) compose);
  media_type=MagickToMime(image->magick);
  (void) MVGPrintf(wand,"image %s %.20g %.20g %.20g %.20g 'data:%s;base64,\n",
    mode,x,y,width,height,media_type);
  p=base64;
  for (i=(ssize_t) encoded_length; i > 0; i-=76)
  {
    (void) MVGPrintf(wand,"%.76s",p);
    p+=76;
    if (i > 76)
      (void) MVGPrintf(wand,"\n");
  }
  (void) MVGPrintf(wand,"'\n");
  media_type=DestroyString(media_type);
  base64=DestroyString(base64);
  return(MagickTrue);
}