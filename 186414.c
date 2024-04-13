static void MSLCharacters(void *context,const xmlChar *c,int length)
{
  MSLInfo
    *msl_info;

  register char
    *p;

  register ssize_t
    i;

  /*
    Receiving some characters from the parser.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.characters(%s,%d)",c,length);
  msl_info=(MSLInfo *) context;
  if (msl_info->content != (char *) NULL)
    msl_info->content=(char *) ResizeQuantumMemory(msl_info->content,
      strlen(msl_info->content)+length+MaxTextExtent,
      sizeof(*msl_info->content));
  else
    {
      msl_info->content=(char *) NULL;
      if (~(size_t) length >= (MaxTextExtent-1))
        msl_info->content=(char *) AcquireQuantumMemory(length+MaxTextExtent,
          sizeof(*msl_info->content));
      if (msl_info->content != (char *) NULL)
        *msl_info->content='\0';
    }
  if (msl_info->content == (char *) NULL)
    return;
  p=msl_info->content+strlen(msl_info->content);
  for (i=0; i < length; i++)
    *p++=c[i];
  *p='\0';
}