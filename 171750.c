static void Upsample(const size_t width,const size_t height,
  const size_t scaled_width,unsigned char *pixels)
{
  register ssize_t
    x,
    y;

  register unsigned char
    *p,
    *q,
    *r;

  /*
    Create a new image that is a integral size greater than an existing one.
  */
  assert(pixels != (unsigned char *) NULL);
  for (y=0; y < (ssize_t) height; y++)
  {
    p=pixels+(height-1-y)*scaled_width+(width-1);
    q=pixels+((height-1-y) << 1)*scaled_width+((width-1) << 1);
    *q=(*p);
    *(q+1)=(*(p));
    for (x=1; x < (ssize_t) width; x++)
    {
      p--;
      q-=2;
      *q=(*p);
      *(q+1)=(unsigned char) ((((size_t) *p)+((size_t) *(p+1))+1) >> 1);
    }
  }
  for (y=0; y < (ssize_t) (height-1); y++)
  {
    p=pixels+((size_t) y << 1)*scaled_width;
    q=p+scaled_width;
    r=q+scaled_width;
    for (x=0; x < (ssize_t) (width-1); x++)
    {
      *q=(unsigned char) ((((size_t) *p)+((size_t) *r)+1) >> 1);
      *(q+1)=(unsigned char) ((((size_t) *p)+((size_t) *(p+2))+
        ((size_t) *r)+((size_t) *(r+2))+2) >> 2);
      q+=2;
      p+=2;
      r+=2;
    }
    *q++=(unsigned char) ((((size_t) *p++)+((size_t) *r++)+1) >> 1);
    *q++=(unsigned char) ((((size_t) *p++)+((size_t) *r++)+1) >> 1);
  }
  p=pixels+(2*height-2)*scaled_width;
  q=pixels+(2*height-1)*scaled_width;
  (void) memcpy(q,p,(size_t) (2*width));
}