MagickExport MagickBooleanType WriteImage(const ImageInfo *image_info,
  Image *image,ExceptionInfo *exception)
{
  char
    filename[MagickPathExtent];

  const char
    *option;

  const DelegateInfo
    *delegate_info;

  const MagickInfo
    *magick_info;

  ExceptionInfo
    *sans_exception;

  ImageInfo
    *write_info;

  MagickBooleanType
    status,
    temporary;

  PolicyDomain
    domain;

  PolicyRights
    rights;

  /*
    Determine image type from filename prefix or suffix (e.g. image.jpg).
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(exception != (ExceptionInfo *) NULL);
  sans_exception=AcquireExceptionInfo();
  write_info=CloneImageInfo(image_info);
  (void) CopyMagickString(write_info->filename,image->filename,
    MagickPathExtent);
  (void) SetImageInfo(write_info,1,sans_exception);
  if (*write_info->magick == '\0')
    (void) CopyMagickString(write_info->magick,image->magick,MagickPathExtent);
  (void) CopyMagickString(filename,image->filename,MagickPathExtent);
  (void) CopyMagickString(image->filename,write_info->filename,
    MagickPathExtent);
  domain=CoderPolicyDomain;
  rights=WritePolicyRights;
  if (IsRightsAuthorized(domain,rights,write_info->magick) == MagickFalse)
    {
      sans_exception=DestroyExceptionInfo(sans_exception);
      write_info=DestroyImageInfo(write_info);
      errno=EPERM;
      ThrowBinaryException(PolicyError,"NotAuthorized",filename);
    }
  /*
    Call appropriate image writer based on image type.
  */
  magick_info=GetMagickInfo(write_info->magick,sans_exception);
  sans_exception=DestroyExceptionInfo(sans_exception);
  if (magick_info != (const MagickInfo *) NULL)
    {
      if (GetMagickEndianSupport(magick_info) == MagickFalse)
        image->endian=UndefinedEndian;
      else
        if ((image_info->endian == UndefinedEndian) &&
            (GetMagickRawSupport(magick_info) != MagickFalse))
          {
            unsigned long
              lsb_first;

            lsb_first=1;
            image->endian=(*(char *) &lsb_first) == 1 ? LSBEndian : MSBEndian;
         }
    }
  (void) SyncImageProfiles(image);
  DisassociateImageStream(image);
  option=GetImageOption(image_info,"delegate:bimodal");
  if ((IsStringTrue(option) != MagickFalse) &&
      (write_info->page == (char *) NULL) &&
      (GetPreviousImageInList(image) == (Image *) NULL) &&
      (GetNextImageInList(image) == (Image *) NULL) &&
      (IsTaintImage(image) == MagickFalse) )
    {
      delegate_info=GetDelegateInfo(image->magick,write_info->magick,exception);
      if ((delegate_info != (const DelegateInfo *) NULL) &&
          (GetDelegateMode(delegate_info) == 0) &&
          (IsPathAccessible(image->magick_filename) != MagickFalse))
        {
          /*
            Process image with bi-modal delegate.
          */
          (void) CopyMagickString(image->filename,image->magick_filename,
            MagickPathExtent);
          status=InvokeDelegate(write_info,image,image->magick,
            write_info->magick,exception);
          write_info=DestroyImageInfo(write_info);
          (void) CopyMagickString(image->filename,filename,MagickPathExtent);
          return(status);
        }
    }
  status=MagickFalse;
  temporary=MagickFalse;
  if ((magick_info != (const MagickInfo *) NULL) &&
      (GetMagickSeekableStream(magick_info) != MagickFalse))
    {
      char
        image_filename[MagickPathExtent];

      (void) CopyMagickString(image_filename,image->filename,MagickPathExtent);
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
      (void) CopyMagickString(image->filename, image_filename,MagickPathExtent);
      if (status != MagickFalse)
        {
          if (IsBlobSeekable(image) == MagickFalse)
            {
              /*
                A seekable stream is required by the encoder.
              */
              write_info->adjoin=MagickTrue;
              (void) CopyMagickString(write_info->filename,image->filename,
                MagickPathExtent);
              (void) AcquireUniqueFilename(image->filename);
              temporary=MagickTrue;
            }
          (void) CloseBlob(image);
        }
    }
  if ((magick_info != (const MagickInfo *) NULL) &&
      (GetImageEncoder(magick_info) != (EncodeImageHandler *) NULL))
    {
      /*
        Call appropriate image writer based on image type.
      */
      if (GetMagickEncoderThreadSupport(magick_info) == MagickFalse)
        LockSemaphoreInfo(magick_info->semaphore);
      status=GetImageEncoder(magick_info)(write_info,image,exception);
      if (GetMagickEncoderThreadSupport(magick_info) == MagickFalse)
        UnlockSemaphoreInfo(magick_info->semaphore);
    }
  else
    {
      delegate_info=GetDelegateInfo((char *) NULL,write_info->magick,exception);
      if (delegate_info != (DelegateInfo *) NULL)
        {
          /*
            Process the image with delegate.
          */
          *write_info->filename='\0';
          if (GetDelegateThreadSupport(delegate_info) == MagickFalse)
            LockSemaphoreInfo(delegate_info->semaphore);
          status=InvokeDelegate(write_info,image,(char *) NULL,
            write_info->magick,exception);
          if (GetDelegateThreadSupport(delegate_info) == MagickFalse)
            UnlockSemaphoreInfo(delegate_info->semaphore);
          (void) CopyMagickString(image->filename,filename,MagickPathExtent);
        }
      else
        {
          sans_exception=AcquireExceptionInfo();
          magick_info=GetMagickInfo(write_info->magick,sans_exception);
          sans_exception=DestroyExceptionInfo(sans_exception);
          if ((write_info->affirm == MagickFalse) &&
              (magick_info == (const MagickInfo *) NULL))
            {
              (void) CopyMagickString(write_info->magick,image->magick,
                MagickPathExtent);
              magick_info=GetMagickInfo(write_info->magick,exception);
            }
          if ((magick_info == (const MagickInfo *) NULL) ||
              (GetImageEncoder(magick_info) == (EncodeImageHandler *) NULL))
            {
              char
                extension[MagickPathExtent];

              GetPathComponent(image->filename,ExtensionPath,extension);
              if (*extension != '\0')
                magick_info=GetMagickInfo(extension,exception);
              else
                magick_info=GetMagickInfo(image->magick,exception);
              (void) CopyMagickString(image->filename,filename,
                MagickPathExtent);
            }
          if ((magick_info == (const MagickInfo *) NULL) ||
              (GetImageEncoder(magick_info) == (EncodeImageHandler *) NULL))
            {
              magick_info=GetMagickInfo(image->magick,exception);
              if ((magick_info == (const MagickInfo *) NULL) ||
                  (GetImageEncoder(magick_info) == (EncodeImageHandler *) NULL))
                (void) ThrowMagickException(exception,GetMagickModule(),
                  MissingDelegateError,"NoEncodeDelegateForThisImageFormat",
                  "`%s'",write_info->magick);
              else
                (void) ThrowMagickException(exception,GetMagickModule(),
                  MissingDelegateWarning,"NoEncodeDelegateForThisImageFormat",
                  "`%s'",write_info->magick);
            }
          if ((magick_info != (const MagickInfo *) NULL) &&
              (GetImageEncoder(magick_info) != (EncodeImageHandler *) NULL))
            {
              /*
                Call appropriate image writer based on image type.
              */
              if (GetMagickEncoderThreadSupport(magick_info) == MagickFalse)
                LockSemaphoreInfo(magick_info->semaphore);
              status=GetImageEncoder(magick_info)(write_info,image,exception);
              if (GetMagickEncoderThreadSupport(magick_info) == MagickFalse)
                UnlockSemaphoreInfo(magick_info->semaphore);
            }
        }
    }
  if (temporary != MagickFalse)
    {
      /*
        Copy temporary image file to permanent.
      */
      status=OpenBlob(write_info,image,ReadBinaryBlobMode,exception);
      if (status != MagickFalse)
        {
          (void) RelinquishUniqueFileResource(write_info->filename);
          status=ImageToFile(image,write_info->filename,exception);
        }
      (void) CloseBlob(image);
      (void) RelinquishUniqueFileResource(image->filename);
      (void) CopyMagickString(image->filename,write_info->filename,
        MagickPathExtent);
    }
  if ((LocaleCompare(write_info->magick,"info") != 0) &&
      (write_info->verbose != MagickFalse))
    (void) IdentifyImage(image,stdout,MagickFalse,exception);
  write_info=DestroyImageInfo(write_info);
  return(status);
}