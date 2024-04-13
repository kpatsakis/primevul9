static MagickBooleanType XSaveImage(Display *display,
  XResourceInfo *resource_info,XWindows *windows,Image *image)
{
  char
    filename[MaxTextExtent];

  ImageInfo
    *image_info;

  MagickStatusType
    status;

  /*
    Request file name from user.
  */
  if (resource_info->write_filename != (char *) NULL)
    (void) CopyMagickString(filename,resource_info->write_filename,
      MaxTextExtent);
  else
    {
      char
        path[MaxTextExtent];

      int
        status;

      GetPathComponent(image->filename,HeadPath,path);
      GetPathComponent(image->filename,TailPath,filename);
      if (*path == '\0')
        (void) CopyMagickString(path,".",MaxTextExtent);
      status=chdir(path);
      if (status == -1)
        (void) ThrowMagickException(&image->exception,GetMagickModule(),
          FileOpenError,"UnableToOpenFile","%s",path);
    }
  XFileBrowserWidget(display,windows,"Save",filename);
  if (*filename == '\0')
    return(MagickTrue);
  if (IsPathAccessible(filename) != MagickFalse)
    {
      int
        status;

      /*
        File exists-- seek user's permission before overwriting.
      */
      status=XConfirmWidget(display,windows,"Overwrite",filename);
      if (status == 0)
        return(MagickTrue);
    }
  image_info=CloneImageInfo(resource_info->image_info);
  (void) CopyMagickString(image_info->filename,filename,MaxTextExtent);
  (void) SetImageInfo(image_info,1,&image->exception);
  if ((LocaleCompare(image_info->magick,"JPEG") == 0) ||
      (LocaleCompare(image_info->magick,"JPG") == 0))
    {
      char
        quality[MaxTextExtent];

      int
        status;

      /*
        Request JPEG quality from user.
      */
      (void) FormatLocaleString(quality,MaxTextExtent,"%.20g",(double)
        image_info->quality);
      status=XDialogWidget(display,windows,"Save","Enter JPEG quality:",
        quality);
      if (*quality == '\0')
        return(MagickTrue);
      image->quality=StringToUnsignedLong(quality);
      image_info->interlace=status != MagickFalse ?  NoInterlace :
        PlaneInterlace;
    }
  if ((LocaleCompare(image_info->magick,"EPS") == 0) ||
      (LocaleCompare(image_info->magick,"PDF") == 0) ||
      (LocaleCompare(image_info->magick,"PS") == 0) ||
      (LocaleCompare(image_info->magick,"PS2") == 0))
    {
      char
        geometry[MaxTextExtent];

      /*
        Request page geometry from user.
      */
      (void) CopyMagickString(geometry,PSPageGeometry,MaxTextExtent);
      if (LocaleCompare(image_info->magick,"PDF") == 0)
        (void) CopyMagickString(geometry,PSPageGeometry,MaxTextExtent);
      if (image_info->page != (char *) NULL)
        (void) CopyMagickString(geometry,image_info->page,MaxTextExtent);
      XListBrowserWidget(display,windows,&windows->widget,PageSizes,"Select",
        "Select page geometry:",geometry);
      if (*geometry != '\0')
        image_info->page=GetPageGeometry(geometry);
    }
  /*
    Write image.
  */
  image=GetFirstImageInList(image);
  status=WriteImages(image_info,image,filename,&image->exception);
  if (status != MagickFalse)
    image->taint=MagickFalse;
  image_info=DestroyImageInfo(image_info);
  XSetCursorState(display,windows,MagickFalse);
  return(status != 0 ? MagickTrue : MagickFalse);
}