static Quantum *GetAuthenticPixelsStream(Image *image,const ssize_t x,
  const ssize_t y,const size_t columns,const size_t rows,
  ExceptionInfo *exception)
{
  Quantum
    *pixels;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  pixels=QueueAuthenticPixelsStream(image,x,y,columns,rows,exception);
  return(pixels);
}