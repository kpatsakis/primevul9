static MagickBooleanType CopyDelegateFile(const char *source,
  const char *destination,const MagickBooleanType overwrite)
{
  int
    destination_file,
    source_file;

  MagickBooleanType
    status;

  register size_t
    i;

  size_t
    length,
    quantum;

  ssize_t
    count;

  struct stat
    attributes;

  unsigned char
    *buffer;

  /*
    Copy source file to destination.
  */
  assert(source != (const char *) NULL);
  assert(destination != (char *) NULL);
  if (overwrite == MagickFalse)
    {
      status=GetPathAttributes(destination,&attributes);
      if (status != MagickFalse)
        return(MagickTrue);
    }
  destination_file=open_utf8(destination,O_WRONLY | O_BINARY | O_CREAT,S_MODE);
  if (destination_file == -1)
    return(MagickFalse);
  source_file=open_utf8(source,O_RDONLY | O_BINARY,0);
  if (source_file == -1)
    {
      (void) close(destination_file);
      return(MagickFalse);
    }
  quantum=(size_t) MagickMaxBufferExtent;
  if ((fstat(source_file,&attributes) == 0) && (attributes.st_size > 0))
    quantum=MagickMin((size_t) attributes.st_size,MagickMaxBufferExtent);
  buffer=(unsigned char *) AcquireQuantumMemory(quantum,sizeof(*buffer));
  if (buffer == (unsigned char *) NULL)
    {
      (void) close(source_file);
      (void) close(destination_file);
      return(MagickFalse);
    }
  length=0;
  for (i=0; ; i+=count)
  {
    count=(ssize_t) read(source_file,buffer,quantum);
    if (count <= 0)
      break;
    length=(size_t) count;
    count=(ssize_t) write(destination_file,buffer,length);
    if ((size_t) count != length)
      break;
  }
  (void) close(destination_file);
  (void) close(source_file);
  buffer=(unsigned char *) RelinquishMagickMemory(buffer);
  return(i != 0 ? MagickTrue : MagickFalse);
}