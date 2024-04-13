static size_t PCLDeltaCompressImage(const size_t length,
  const unsigned char *previous_pixels,const unsigned char *pixels,
  unsigned char *compress_pixels)
{
  int
    delta,
    j,
    replacement;

  register ssize_t
    i,
    x;

  register unsigned char
    *q;

  q=compress_pixels;
  for (x=0; x < (ssize_t) length; )
  {
    j=0;
    for (i=0; x < (ssize_t) length; x++)
    {
      if (*pixels++ != *previous_pixels++)
        {
          i=1;
          break;
        }
      j++;
    }
    while (x < (ssize_t) length)
    {
      x++;
      if (*pixels == *previous_pixels)
        break;
      i++;
      previous_pixels++;
      pixels++;
    }
    if (i == 0)
      break;
    replacement=j >= 31 ? 31 : j;
    j-=replacement;
    delta=i >= 8 ? 8 : i;
    *q++=(unsigned char) (((delta-1) << 5) | replacement);
    if (replacement == 31)
      {
        for (replacement=255; j != 0; )
        {
          if (replacement > j)
            replacement=j;
          *q++=(unsigned char) replacement;
          j-=replacement;
        }
        if (replacement == 255)
          *q++='\0';
      }
    for (pixels-=i; i != 0; )
    {
      for (i-=delta; delta != 0; delta--)
        *q++=(*pixels++);
      if (i == 0)
        break;
      delta=(int) i;
      if (i >= 8)
        delta=8;
      *q++=(unsigned char) ((delta-1) << 5);
    }
  }
  return((size_t) (q-compress_pixels));
}