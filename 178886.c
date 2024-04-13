static MagickBooleanType DecodeImage(const unsigned char *compressed_pixels,
  const size_t length,unsigned char *pixels,size_t extent)
{
  register const unsigned char
    *p;

  register unsigned char
    *q;

  ssize_t
    count;

  unsigned char
    byte;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(compressed_pixels != (unsigned char *) NULL);
  assert(pixels != (unsigned char *) NULL);
  p=compressed_pixels;
  q=pixels;
  while (((size_t) (p-compressed_pixels) < length) &&
         ((size_t) (q-pixels) < extent))
  {
    byte=(*p++);
    if (byte != 128U)
      *q++=byte;
    else
      {
        /*
          Runlength-encoded packet: <count><byte>.
        */
        if (((size_t) (p-compressed_pixels) >= length))
          break;
        count=(*p++);
        if (count > 0)
          {
            if (((size_t) (p-compressed_pixels) >= length))
              break;
            byte=(*p++);
          }
        while ((count >= 0) && ((size_t) (q-pixels) < extent))
        {
          *q++=byte;
          count--;
        }
     }
  }
  return(((size_t) (q-pixels) == extent) ? MagickTrue : MagickFalse);
}