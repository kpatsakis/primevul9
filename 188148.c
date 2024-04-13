MagickExport MagickBooleanType InvokeDelegate(ImageInfo *image_info,
  Image *image,const char *decode,const char *encode,ExceptionInfo *exception)
{
  char
    *command,
    **commands,
    input_filename[MagickPathExtent],
    output_filename[MagickPathExtent];

  const DelegateInfo
    *delegate_info;

  MagickBooleanType
    status,
    temporary;

  PolicyRights
    rights;

  register ssize_t
    i;

  /*
    Get delegate.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  rights=ExecutePolicyRights;
  if (IsRightsAuthorized(DelegatePolicyDomain,rights,decode) == MagickFalse)
    {
      errno=EPERM;
      (void) ThrowMagickException(exception,GetMagickModule(),PolicyError,
        "NotAuthorized","`%s'",decode);
      return(MagickFalse);
    }
  if (IsRightsAuthorized(DelegatePolicyDomain,rights,encode) == MagickFalse)
    {
      errno=EPERM;
      (void) ThrowMagickException(exception,GetMagickModule(),PolicyError,
        "NotAuthorized","`%s'",encode);
      return(MagickFalse);
    }
  temporary=*image->filename == '\0' ? MagickTrue : MagickFalse;
  if ((temporary != MagickFalse) && (AcquireUniqueFilename(image->filename) ==
      MagickFalse))
    {
      ThrowFileException(exception,FileOpenError,"UnableToCreateTemporaryFile",
        image->filename);
      return(MagickFalse);
    }
  delegate_info=GetDelegateInfo(decode,encode,exception);
  if (delegate_info == (DelegateInfo *) NULL)
    {
      if (temporary != MagickFalse)
        (void) RelinquishUniqueFileResource(image->filename);
      (void) ThrowMagickException(exception,GetMagickModule(),DelegateError,
        "NoTagFound","`%s'",decode ? decode : encode);
      return(MagickFalse);
    }
  if (*image_info->filename == '\0')
    {
      if (AcquireUniqueFilename(image_info->filename) == MagickFalse)
        {
          if (temporary != MagickFalse)
            (void) RelinquishUniqueFileResource(image->filename);
          ThrowFileException(exception,FileOpenError,
            "UnableToCreateTemporaryFile",image_info->filename);
          return(MagickFalse);
        }
      image_info->temporary=MagickTrue;
    }
  if ((delegate_info->mode != 0) && (((decode != (const char *) NULL) &&
        (delegate_info->encode != (char *) NULL)) ||
       ((encode != (const char *) NULL) &&
        (delegate_info->decode != (char *) NULL))))
    {
      char
        *magick;

      ImageInfo
        *clone_info;

      register Image
        *p;

      /*
        Delegate requires a particular image format.
      */
      if (AcquireUniqueFilename(image_info->unique) == MagickFalse)
        {
          ThrowFileException(exception,FileOpenError,
            "UnableToCreateTemporaryFile",image_info->unique);
          return(MagickFalse);
        }
      magick=InterpretImageProperties(image_info,image,decode != (char *) NULL ?
        delegate_info->encode : delegate_info->decode,exception);
      if (magick == (char *) NULL)
        {
          (void) RelinquishUniqueFileResource(image_info->unique);
          if (temporary != MagickFalse)
            (void) RelinquishUniqueFileResource(image->filename);
          (void) ThrowMagickException(exception,GetMagickModule(),
            DelegateError,"DelegateFailed","`%s'",decode ? decode : encode);
          return(MagickFalse);
        }
      LocaleUpper(magick);
      clone_info=CloneImageInfo(image_info);
      (void) CopyMagickString((char *) clone_info->magick,magick,
        MagickPathExtent);
      if (LocaleCompare(magick,"NULL") != 0)
        (void) CopyMagickString(image->magick,magick,MagickPathExtent);
      magick=DestroyString(magick);
      (void) FormatLocaleString(clone_info->filename,MagickPathExtent,"%s:",
        delegate_info->decode);
      (void) SetImageInfo(clone_info,(unsigned int) GetImageListLength(image),
        exception);
      (void) CopyMagickString(clone_info->filename,image_info->filename,
        MagickPathExtent);
      (void) CopyMagickString(image_info->filename,image->filename,
        MagickPathExtent);
      for (p=image; p != (Image *) NULL; p=GetNextImageInList(p))
      {
        (void) FormatLocaleString(p->filename,MagickPathExtent,"%s:%s",
          delegate_info->decode,clone_info->filename);
        status=WriteImage(clone_info,p,exception);
        if (status == MagickFalse)
          {
            (void) RelinquishUniqueFileResource(image_info->unique);
            if (temporary != MagickFalse)
              (void) RelinquishUniqueFileResource(image->filename);
            clone_info=DestroyImageInfo(clone_info);
            (void) ThrowMagickException(exception,GetMagickModule(),
              DelegateError,"DelegateFailed","`%s'",decode ? decode : encode);
            return(MagickFalse);
          }
        if (clone_info->adjoin != MagickFalse)
          break;
      }
      (void) RelinquishUniqueFileResource(image_info->unique);
      clone_info=DestroyImageInfo(clone_info);
    }
  /*
    Invoke delegate.
  */
  commands=StringToList(delegate_info->commands);
  if (commands == (char **) NULL)
    {
      if (temporary != MagickFalse)
        (void) RelinquishUniqueFileResource(image->filename);
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",
        decode ? decode : encode);
      return(MagickFalse);
    }
  command=(char *) NULL;
  status=MagickTrue;
  (void) CopyMagickString(output_filename,image_info->filename,
    MagickPathExtent);
  (void) CopyMagickString(input_filename,image->filename,MagickPathExtent);
  for (i=0; commands[i] != (char *) NULL; i++)
  {
    (void) AcquireUniqueSymbolicLink(output_filename,image_info->filename);
    if (AcquireUniqueFilename(image_info->unique) == MagickFalse)
      {
        ThrowFileException(exception,FileOpenError,
          "UnableToCreateTemporaryFile",image_info->unique);
        break;
      }
    if (LocaleCompare(decode,"SCAN") != 0)
      {
        status=AcquireUniqueSymbolicLink(input_filename,image->filename);
        if (status == MagickFalse)
          {
            ThrowFileException(exception,FileOpenError,
              "UnableToCreateTemporaryFile",input_filename);
            break;
          }
      }
    status=MagickTrue;
    command=InterpretDelegateProperties(image_info,image,commands[i],exception);
    if (command != (char *) NULL)
      {
        /*
          Execute delegate.
        */
        if (ExternalDelegateCommand(delegate_info->spawn,image_info->verbose,
          command,(char *) NULL,exception) != 0)
          status=MagickFalse;
        if (delegate_info->spawn != MagickFalse)
          {
            ssize_t
              count;

            /*
              Wait for input file to 'disappear', or maximum 2 seconds.
            */
            count=20;
            while ((count-- > 0) && (access_utf8(image->filename,F_OK) == 0))
              (void) MagickDelay(100);  /* sleep 0.1 seconds */
          }
        command=DestroyString(command);
      }
    if (LocaleCompare(decode,"SCAN") != 0)
      {
        if (CopyDelegateFile(image->filename,input_filename,MagickFalse) == MagickFalse)
          (void) RelinquishUniqueFileResource(input_filename);
      }
    if ((strcmp(input_filename,output_filename) != 0) &&
        (CopyDelegateFile(image_info->filename,output_filename,MagickTrue) == MagickFalse))
      (void) RelinquishUniqueFileResource(output_filename);
    if (image_info->temporary != MagickFalse)
      (void) RelinquishUniqueFileResource(image_info->filename);
    (void) RelinquishUniqueFileResource(image_info->unique);
    (void) RelinquishUniqueFileResource(image_info->filename);
    (void) RelinquishUniqueFileResource(image->filename);
    if (status == MagickFalse)
      {
        (void) ThrowMagickException(exception,GetMagickModule(),DelegateError,
          "DelegateFailed","`%s'",commands[i]);
        break;
      }
    commands[i]=DestroyString(commands[i]);
  }
  (void) CopyMagickString(image_info->filename,output_filename,
    MagickPathExtent);
  (void) CopyMagickString(image->filename,input_filename,MagickPathExtent);
  /*
    Relinquish resources.
  */
  for ( ; commands[i] != (char *) NULL; i++)
    commands[i]=DestroyString(commands[i]);
  commands=(char **) RelinquishMagickMemory(commands);
  if (temporary != MagickFalse)
    (void) RelinquishUniqueFileResource(image->filename);
  return(status);
}