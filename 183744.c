static void *DestroyRegistryNode(void *registry_info)
{
  register RegistryInfo
    *p;

  p=(RegistryInfo *) registry_info;
  switch (p->type)
  {
    case StringRegistryType:
    default:
    {
      p->value=RelinquishMagickMemory(p->value);
      break;
    }
    case ImageRegistryType:
    {
      p->value=(void *) DestroyImageList((Image *) p->value);
      break;
    }
    case ImageInfoRegistryType:
    {
      p->value=(void *) DestroyImageInfo((ImageInfo *) p->value);
      break;
    }
  }
  return(RelinquishMagickMemory(p));
}