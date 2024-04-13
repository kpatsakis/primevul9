MagickExport Image *ReadImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    filename[MagickPathExtent],
    magick[MagickPathExtent],
    magick_filename[MagickPathExtent];

  const char
    *value;

  const DelegateInfo
    *delegate_info;

  const MagickInfo
    *magick_info;

  ExceptionInfo
    *sans_exception;

  GeometryInfo
    geometry_info;

  Image
    *image,
    *next;

  ImageInfo
    *read_info;

  MagickStatusType
    flags;

  PolicyDomain
    domain;

  PolicyRights
    rights;

  /*
    Determine image type from filename prefix or suffix (e.g. image.jpg).
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image_info->filename != (char *) NULL);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  read_info=CloneImageInfo(image_info);
  (void) CopyMagickString(magick_filename,read_info->filename,MagickPathExtent);
  (void) SetImageInfo(read_info,0,exception);
  (void) CopyMagickString(filename,read_info->filename,MagickPathExtent);
  (void) CopyMagickString(magick,read_info->magick,MagickPathExtent);
  domain=CoderPolicyDomain;
  rights=ReadPolicyRights;
  if (IsRightsAuthorized(domain,rights,read_info->magick) == MagickFalse)
    {
      errno=EPERM;
      (void) ThrowMagickException(exception,GetMagickModule(),PolicyError,
        "NotAuthorized","`%s'",read_info->filename);
      read_info=DestroyImageInfo(read_info);
      return((Image *) NULL);
    }
  /*
    Call appropriate image reader based on image type.
  */
  sans_exception=AcquireExceptionInfo();
  magick_info=GetMagickInfo(read_info->magick,sans_exception);
  sans_exception=DestroyExceptionInfo(sans_exception);
  if (magick_info != (const MagickInfo *) NULL)
    {
      if (GetMagickEndianSupport(magick_info) == MagickFalse)
        read_info->endian=UndefinedEndian;
      else
        if ((image_info->endian == UndefinedEndian) &&
            (GetMagickRawSupport(magick_info) != MagickFalse))
          {
            unsigned long
              lsb_first;

            lsb_first=1;
            read_info->endian=(*(char *) &lsb_first) == 1 ? LSBEndian :
              MSBEndian;
         }
    }
  if ((magick_info != (const MagickInfo *) NULL) &&
      (GetMagickSeekableStream(magick_info) != MagickFalse))
    {
      MagickBooleanType
        status;

      image=AcquireImage(read_info,exception);
      (void) CopyMagickString(image->filename,read_info->filename,
        MagickPathExtent);
      status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
      if (status == MagickFalse)
        {
          read_info=DestroyImageInfo(read_info);
          image=DestroyImage(image);
          return((Image *) NULL);
        }
      if (IsBlobSeekable(image) == MagickFalse)
        {
          /*
            Coder requires a seekable stream.
          */
          *read_info->filename='\0';
          status=ImageToFile(image,read_info->filename,exception);
          if (status == MagickFalse)
            {
              (void) CloseBlob(image);
              read_info=DestroyImageInfo(read_info);
              image=DestroyImage(image);
              return((Image *) NULL);
            }
          read_info->temporary=MagickTrue;
        }
      (void) CloseBlob(image);
      image=DestroyImage(image);
    }
  image=NewImageList();
  if ((magick_info == (const MagickInfo *) NULL) ||
      (GetImageDecoder(magick_info) == (DecodeImageHandler *) NULL))
    {
      delegate_info=GetDelegateInfo(read_info->magick,(char *) NULL,exception);
      if (delegate_info == (const DelegateInfo *) NULL)
        {
          (void) SetImageInfo(read_info,0,exception);
          (void) CopyMagickString(read_info->filename,filename,
            MagickPathExtent);
          magick_info=GetMagickInfo(read_info->magick,exception);
        }
    }
  if ((magick_info != (const MagickInfo *) NULL) &&
      (GetImageDecoder(magick_info) != (DecodeImageHandler *) NULL))
    {
      if (GetMagickDecoderThreadSupport(magick_info) == MagickFalse)
        LockSemaphoreInfo(magick_info->semaphore);
      image=GetImageDecoder(magick_info)(read_info,exception);
      if (GetMagickDecoderThreadSupport(magick_info) == MagickFalse)
        UnlockSemaphoreInfo(magick_info->semaphore);
    }
  else
    {
      MagickBooleanType
        status;

      delegate_info=GetDelegateInfo(read_info->magick,(char *) NULL,exception);
      if (delegate_info == (const DelegateInfo *) NULL)
        {
          (void) ThrowMagickException(exception,GetMagickModule(),
            MissingDelegateError,"NoDecodeDelegateForThisImageFormat","`%s'",
            read_info->magick);
          if (read_info->temporary != MagickFalse)
            (void) RelinquishUniqueFileResource(read_info->filename);
          read_info=DestroyImageInfo(read_info);
          return((Image *) NULL);
        }
      /*
        Let our decoding delegate process the image.
      */
      image=AcquireImage(read_info,exception);
      if (image == (Image *) NULL)
        {
          read_info=DestroyImageInfo(read_info);
          return((Image *) NULL);
        }
      (void) CopyMagickString(image->filename,read_info->filename,
        MagickPathExtent);
      *read_info->filename='\0';
      if (GetDelegateThreadSupport(delegate_info) == MagickFalse)
        LockSemaphoreInfo(delegate_info->semaphore);
      status=InvokeDelegate(read_info,image,read_info->magick,(char *) NULL,
        exception);
      if (GetDelegateThreadSupport(delegate_info) == MagickFalse)
        UnlockSemaphoreInfo(delegate_info->semaphore);
      image=DestroyImageList(image);
      read_info->temporary=MagickTrue;
      if (status != MagickFalse)
        (void) SetImageInfo(read_info,0,exception);
      magick_info=GetMagickInfo(read_info->magick,exception);
      if ((magick_info == (const MagickInfo *) NULL) ||
          (GetImageDecoder(magick_info) == (DecodeImageHandler *) NULL))
        {
          if (IsPathAccessible(read_info->filename) != MagickFalse)
            (void) ThrowMagickException(exception,GetMagickModule(),
              MissingDelegateError,"NoDecodeDelegateForThisImageFormat","`%s'",
              read_info->magick);
          else
            ThrowFileException(exception,FileOpenError,"UnableToOpenFile",
              read_info->filename);
          read_info=DestroyImageInfo(read_info);
          return((Image *) NULL);
        }
      if (GetMagickDecoderThreadSupport(magick_info) == MagickFalse)
        LockSemaphoreInfo(magick_info->semaphore);
      image=(Image *) (GetImageDecoder(magick_info))(read_info,exception);
      if (GetMagickDecoderThreadSupport(magick_info) == MagickFalse)
        UnlockSemaphoreInfo(magick_info->semaphore);
    }
  if (read_info->temporary != MagickFalse)
    {
      (void) RelinquishUniqueFileResource(read_info->filename);
      read_info->temporary=MagickFalse;
      if (image != (Image *) NULL)
        (void) CopyMagickString(image->filename,filename,MagickPathExtent);
    }
  if (image == (Image *) NULL)
    {
      read_info=DestroyImageInfo(read_info);
      return(image);
    }
  if (exception->severity >= ErrorException)
    (void) LogMagickEvent(ExceptionEvent,GetMagickModule(),
      "Coder (%s) generated an image despite an error (%d), "
      "notify the developers",image->magick,exception->severity);
  if (IsBlobTemporary(image) != MagickFalse)
    (void) RelinquishUniqueFileResource(read_info->filename);
  if ((GetNextImageInList(image) != (Image *) NULL) &&
      (IsSceneGeometry(read_info->scenes,MagickFalse) != MagickFalse))
    {
      Image
        *clones;

      clones=CloneImages(image,read_info->scenes,exception);
      if (clones == (Image *) NULL)
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
          "SubimageSpecificationReturnsNoImages","`%s'",read_info->filename);
      else
        {
          image=DestroyImageList(image);
          image=GetFirstImageInList(clones);
        }
    }
  for (next=image; next != (Image *) NULL; next=GetNextImageInList(next))
  {
    char
      magick_path[MagickPathExtent],
      *property,
      timestamp[MagickPathExtent];

    const char
      *option;

    const StringInfo
      *profile;

    next->taint=MagickFalse;
    GetPathComponent(magick_filename,MagickPath,magick_path);
    if (*magick_path == '\0' && *next->magick == '\0')
      (void) CopyMagickString(next->magick,magick,MagickPathExtent);
    (void) CopyMagickString(next->magick_filename,magick_filename,
      MagickPathExtent);
    if (IsBlobTemporary(image) != MagickFalse)
      (void) CopyMagickString(next->filename,filename,MagickPathExtent);
    if (next->magick_columns == 0)
      next->magick_columns=next->columns;
    if (next->magick_rows == 0)
      next->magick_rows=next->rows;
    value=GetImageProperty(next,"tiff:Orientation",exception);
    if (value == (char *) NULL)
      value=GetImageProperty(next,"exif:Orientation",exception);
    if (value != (char *) NULL)
      {
        next->orientation=(OrientationType) StringToLong(value);
        (void) DeleteImageProperty(next,"tiff:Orientation");
        (void) DeleteImageProperty(next,"exif:Orientation");
      }
    value=GetImageProperty(next,"exif:XResolution",exception);
    if (value != (char *) NULL)
      {
        geometry_info.rho=next->resolution.x;
        geometry_info.sigma=1.0;
        flags=ParseGeometry(value,&geometry_info);
        if (geometry_info.sigma != 0)
          next->resolution.x=geometry_info.rho/geometry_info.sigma;
        (void) DeleteImageProperty(next,"exif:XResolution");
      }
    value=GetImageProperty(next,"exif:YResolution",exception);
    if (value != (char *) NULL)
      {
        geometry_info.rho=next->resolution.y;
        geometry_info.sigma=1.0;
        flags=ParseGeometry(value,&geometry_info);
        if (geometry_info.sigma != 0)
          next->resolution.y=geometry_info.rho/geometry_info.sigma;
        (void) DeleteImageProperty(next,"exif:YResolution");
      }
    value=GetImageProperty(next,"tiff:ResolutionUnit",exception);
    if (value == (char *) NULL)
      value=GetImageProperty(next,"exif:ResolutionUnit",exception);
    if (value != (char *) NULL)
      {
        next->units=(ResolutionType) (StringToLong(value)-1);
        (void) DeleteImageProperty(next,"exif:ResolutionUnit");
        (void) DeleteImageProperty(next,"tiff:ResolutionUnit");
      }
    if (next->page.width == 0)
      next->page.width=next->columns;
    if (next->page.height == 0)
      next->page.height=next->rows;
    option=GetImageOption(read_info,"caption");
    if (option != (const char *) NULL)
      {
        property=InterpretImageProperties(read_info,next,option,exception);
        (void) SetImageProperty(next,"caption",property,exception);
        property=DestroyString(property);
      }
    option=GetImageOption(read_info,"comment");
    if (option != (const char *) NULL)
      {
        property=InterpretImageProperties(read_info,next,option,exception);
        (void) SetImageProperty(next,"comment",property,exception);
        property=DestroyString(property);
      }
    option=GetImageOption(read_info,"label");
    if (option != (const char *) NULL)
      {
        property=InterpretImageProperties(read_info,next,option,exception);
        (void) SetImageProperty(next,"label",property,exception);
        property=DestroyString(property);
      }
    if (LocaleCompare(next->magick,"TEXT") == 0)
      (void) ParseAbsoluteGeometry("0x0+0+0",&next->page);
    if ((read_info->extract != (char *) NULL) &&
        (read_info->stream == (StreamHandler) NULL))
      {
        RectangleInfo
          geometry;

        flags=ParseAbsoluteGeometry(read_info->extract,&geometry);
        if ((next->columns != geometry.width) ||
            (next->rows != geometry.height))
          {
            if (((flags & XValue) != 0) || ((flags & YValue) != 0))
              {
                Image
                  *crop_image;

                crop_image=CropImage(next,&geometry,exception);
                if (crop_image != (Image *) NULL)
                  ReplaceImageInList(&next,crop_image);
              }
            else
              if (((flags & WidthValue) != 0) || ((flags & HeightValue) != 0))
                {
                  Image
                    *size_image;

                  flags=ParseRegionGeometry(next,read_info->extract,&geometry,
                    exception);
                  size_image=ResizeImage(next,geometry.width,geometry.height,
                    next->filter,exception);
                  if (size_image != (Image *) NULL)
                    ReplaceImageInList(&next,size_image);
                }
          }
      }
    profile=GetImageProfile(next,"icc");
    if (profile == (const StringInfo *) NULL)
      profile=GetImageProfile(next,"icm");
    profile=GetImageProfile(next,"iptc");
    if (profile == (const StringInfo *) NULL)
      profile=GetImageProfile(next,"8bim");
    (void) FormatMagickTime(GetBlobProperties(next)->st_mtime,MagickPathExtent,
      timestamp);
    (void) SetImageProperty(next,"date:modify",timestamp,exception);
    (void) FormatMagickTime(GetBlobProperties(next)->st_ctime,MagickPathExtent,
      timestamp);
    (void) SetImageProperty(next,"date:create",timestamp,exception);
    option=GetImageOption(image_info,"delay");
    if (option != (const char *) NULL)
      {
        flags=ParseGeometry(option,&geometry_info);
        if ((flags & GreaterValue) != 0)
          {
            if (next->delay > (size_t) floor(geometry_info.rho+0.5))
              next->delay=(size_t) floor(geometry_info.rho+0.5);
          }
        else
          if ((flags & LessValue) != 0)
            {
              if (next->delay < (size_t) floor(geometry_info.rho+0.5))
                next->ticks_per_second=(ssize_t) floor(geometry_info.sigma+0.5);
            }
          else
            next->delay=(size_t) floor(geometry_info.rho+0.5);
        if ((flags & SigmaValue) != 0)
          next->ticks_per_second=(ssize_t) floor(geometry_info.sigma+0.5);
      }
    option=GetImageOption(image_info,"dispose");
    if (option != (const char *) NULL)
      next->dispose=(DisposeType) ParseCommandOption(MagickDisposeOptions,
        MagickFalse,option);
    if (read_info->verbose != MagickFalse)
      (void) IdentifyImage(next,stderr,MagickFalse,exception);
    image=next;
  }
  read_info=DestroyImageInfo(read_info);
  return(GetFirstImageInList(image));
}