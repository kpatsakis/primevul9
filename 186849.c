static MagickBooleanType WriteVIDImage(const ImageInfo *image_info,Image *image)
{
  Image
    *montage_image;

  ImageInfo
    *write_info;

  MagickBooleanType
    status;

  MontageInfo
    *montage_info;

  register Image
    *p;

  /*
    Create the visual image directory.
  */
  for (p=image; p != (Image *) NULL; p=GetNextImageInList(p))
    (void) SetImageProperty(p,"label",DefaultTileLabel);
  montage_info=CloneMontageInfo(image_info,(MontageInfo *) NULL);
  montage_image=MontageImageList(image_info,montage_info,image,
    &image->exception);
  montage_info=DestroyMontageInfo(montage_info);
  if (montage_image == (Image *) NULL)
    return(MagickFalse);
  (void) CopyMagickString(montage_image->filename,image_info->filename,
    MaxTextExtent);
  write_info=CloneImageInfo(image_info);
  *write_info->magick='\0';
  (void) SetImageInfo(write_info,1,&image->exception);
  if ((*write_info->magick == '\0') ||
      (LocaleCompare(write_info->magick,"VID") == 0))
    (void) FormatLocaleString(montage_image->filename,MaxTextExtent,
      "miff:%s",write_info->filename);
  status=WriteImage(write_info,montage_image);
  montage_image=DestroyImage(montage_image);
  write_info=DestroyImageInfo(write_info);
  return(status);
}