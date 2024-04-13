static size_t HDRWriteRunlengthPixels(Image *image,unsigned char *pixels)
{
#define MinimumRunlength 4

  register size_t
    p,
    q;

  size_t
    runlength;

  ssize_t
    count,
    previous_count;

  unsigned char
    pixel[2];

  for (p=0; p < image->columns; )
  {
    q=p;
    runlength=0;
    previous_count=0;
    while ((runlength < MinimumRunlength) && (q < image->columns))
    {
      q+=runlength;
      previous_count=(ssize_t) runlength;
      runlength=1;
      while ((pixels[q] == pixels[q+runlength]) &&
             ((q+runlength) < image->columns) && (runlength < 127))
       runlength++;
    }
    if ((previous_count > 1) && (previous_count == (ssize_t) (q-p)))
      {
        pixel[0]=(unsigned char) (128+previous_count);
        pixel[1]=pixels[p];
        if (WriteBlob(image,2*sizeof(*pixel),pixel) < 1)
          break;
        p=q;
      }
    while (p < q)
    {
      count=(ssize_t) (q-p);
      if (count > 128)
        count=128;
      pixel[0]=(unsigned char) count;
      if (WriteBlob(image,sizeof(*pixel),pixel) < 1)
        break;
      if (WriteBlob(image,(size_t) count*sizeof(*pixel),&pixels[p]) < 1)
        break;
      p+=count;
    }
    if (runlength >= MinimumRunlength)
      {
        pixel[0]=(unsigned char) (128+runlength);
        pixel[1]=pixels[q];
        if (WriteBlob(image,2*sizeof(*pixel),pixel) < 1)
          break;
        p+=runlength;
      }
  }
  return(p);
}