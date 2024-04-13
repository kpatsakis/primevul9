static size_t PCLPackbitsCompressImage(const size_t length,
  const unsigned char *pixels,unsigned char *compress_pixels)
{
  int
    count;

  register ssize_t
    x;

  register unsigned char
    *q;

  ssize_t
    j;

  unsigned char
    packbits[128];

  /*
    Compress pixels with Packbits encoding.
  */
  q=compress_pixels;
  for (x=(ssize_t) length; x != 0; )
  {
    switch (x)
    {
      case 1:
      {
        x--;
        *q++=0;
        *q++=(*pixels);
        break;
      }
      case 2:
      {
        x-=2;
        *q++=1;
        *q++=(*pixels);
        *q++=pixels[1];
        break;
      }
      case 3:
      {
        x-=3;
        if ((*pixels == *(pixels+1)) && (*(pixels+1) == *(pixels+2)))
          {
            *q++=(unsigned char) ((256-3)+1);
            *q++=(*pixels);
            break;
          }
        *q++=2;
        *q++=(*pixels);
        *q++=pixels[1];
        *q++=pixels[2];
        break;
      }
      default:
      {
        if ((*pixels == *(pixels+1)) && (*(pixels+1) == *(pixels+2)))
          {
            /*
              Packed run.
            */
            count=3;
            while (((ssize_t) count < x) && (*pixels == *(pixels+count)))
            {
              count++;
              if (count >= 127)
                break;
            }
            x-=count;
            *q++=(unsigned char) ((256-count)+1);
            *q++=(*pixels);
            pixels+=count;
            break;
          }
        /*
          Literal run.
        */
        count=0;
        while ((*(pixels+count) != *(pixels+count+1)) ||
               (*(pixels+count+1) != *(pixels+count+2)))
        {
          packbits[count+1]=pixels[count];
          count++;
          if (((ssize_t) count >= (x-3)) || (count >= 127))
            break;
        }
        x-=count;
        *packbits=(unsigned char) (count-1);
        for (j=0; j <= (ssize_t) count; j++)
          *q++=packbits[j];
        pixels+=count;
        break;
      }
    }
  }
  *q++=128; /* EOD marker */
  return((size_t) (q-compress_pixels));
}