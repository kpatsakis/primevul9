MagickExport Image *ReadImages(ImageInfo *image_info,const char *filename,
  ExceptionInfo *exception)
{
  char
    read_filename[MagickPathExtent];

  Image
    *image,
    *images;

  ImageInfo
    *read_info;

  /*
    Read image list from a file.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  read_info=CloneImageInfo(image_info);
  *read_info->magick='\0';
  (void) SetImageOption(read_info,"filename",filename);
  (void) CopyMagickString(read_info->filename,filename,MagickPathExtent);
  (void) InterpretImageFilename(read_info,(Image *) NULL,filename,
    (int) read_info->scene,read_filename,exception);
  if (LocaleCompare(read_filename,read_info->filename) != 0)
    {
      ExceptionInfo
        *sans;

      ssize_t
        extent,
        scene;

      /*
        Images of the form image-%d.png[1-5].
      */
      sans=AcquireExceptionInfo();
      (void) SetImageInfo(read_info,0,sans);
      sans=DestroyExceptionInfo(sans);
      if (read_info->number_scenes != 0)
        {
          (void) CopyMagickString(read_filename,read_info->filename,
            MagickPathExtent);
          images=NewImageList();
          extent=(ssize_t) (read_info->scene+read_info->number_scenes);
          scene=(ssize_t) read_info->scene;
          for ( ; scene < (ssize_t) extent; scene++)
          {
            (void) InterpretImageFilename(image_info,(Image *) NULL,
              read_filename,(int) scene,read_info->filename,exception);
            image=ReadImage(read_info,exception);
            if (image == (Image *) NULL)
              continue;
            AppendImageToList(&images,image);
          }
          read_info=DestroyImageInfo(read_info);
          return(images);
        }
    }
  (void) CopyMagickString(read_info->filename,filename,MagickPathExtent);
  image=ReadImage(read_info,exception);
  read_info=DestroyImageInfo(read_info);
  return(image);
}