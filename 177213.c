static char *ReadBlobStringWithLongSize(Image *image,char *string,size_t max,
  ExceptionInfo *exception)
{
  int
    c;

  MagickOffsetType
    offset;

  register ssize_t
    i;

  size_t
    length;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(max != 0);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  length=ReadBlobMSBLong(image);
  for (i=0; i < (ssize_t) MagickMin(length,max-1); i++)
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      return((char *) NULL);
    string[i]=(char) c;
  }
  string[i]='\0';
  offset=SeekBlob(image,(MagickOffsetType) (length-i),SEEK_CUR);
  if (offset < 0)
    (void) ThrowMagickException(exception,GetMagickModule(),
      CorruptImageError,"ImproperImageHeader","`%s'",image->filename);
  return(string);
}