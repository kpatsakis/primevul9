MagickExport void *GetImageRegistry(const RegistryType type,const char *key,
  ExceptionInfo *exception)
{
  void
    *value;

  RegistryInfo
    *registry_info;

  if (IsEventLogging() != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",key);
  if (registry == (void *) NULL)
    return((void *) NULL);
  registry_info=(RegistryInfo *) GetValueFromSplayTree(registry,key);
  if (registry_info == (void *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),RegistryError,
        "UnableToGetRegistryID","`%s'",key);
      return((void *) NULL);
    }
  value=(void *) NULL;
  switch (type)
  {
    case ImageRegistryType:
    {
      if (type == registry_info->type)
        value=(void *) CloneImageList((Image *) registry_info->value,exception);
      break;
    }
    case ImageInfoRegistryType:
    {
      if (type == registry_info->type)
        value=(void *) CloneImageInfo((ImageInfo *) registry_info->value);
      break;
    }
    case StringRegistryType:
    {
      switch (registry_info->type)
      {
        case ImageRegistryType:
        {
          value=(Image *) ConstantString(((Image *)
            registry_info->value)->filename);
          break;
        }
        case ImageInfoRegistryType:
        {
          value=(Image *) ConstantString(((ImageInfo *)
            registry_info->value)->filename);
          break;
        }
        case StringRegistryType:
        {
          value=(void *) ConstantString((char *) registry_info->value);
          break;
        }
        default:
          break;
      }
      break;
    }
    default:
      break;
  }
  return(value);
}