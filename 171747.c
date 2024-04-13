static Image *OverviewImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  Image
    *montage_image;

  MontageInfo
    *montage_info;

  register Image
    *p;

  /*
    Create the PCD Overview image.
  */
  for (p=image; p != (Image *) NULL; p=p->next)
  {
    (void) DeleteImageProperty(p,"label");
    (void) SetImageProperty(p,"label",DefaultTileLabel);
  }
  montage_info=CloneMontageInfo(image_info,(MontageInfo *) NULL);
  (void) CopyMagickString(montage_info->filename,image_info->filename,
    MaxTextExtent);
  montage_image=MontageImageList(image_info,montage_info,image,exception);
  montage_info=DestroyMontageInfo(montage_info);
  if (montage_image == (Image *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  image=DestroyImageList(image);
  return(montage_image);
}