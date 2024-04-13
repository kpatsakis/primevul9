static void XDitherImage(Image *image,XImage *ximage,ExceptionInfo *exception)
{
  static const short int
    dither_red[2][16]=
    {
      {-16,  4, -1, 11,-14,  6, -3,  9,-15,  5, -2, 10,-13,  7, -4,  8},
      { 15, -5,  0,-12, 13, -7,  2,-10, 14, -6,  1,-11, 12, -8,  3, -9}
    },
    dither_green[2][16]=
    {
      { 11,-15,  7, -3,  8,-14,  4, -2, 10,-16,  6, -4,  9,-13,  5, -1},
      {-12, 14, -8,  2, -9, 13, -5,  1,-11, 15, -7,  3,-10, 12, -6,  0}
    },
    dither_blue[2][16]=
    {
      { -3,  9,-13,  7, -1, 11,-15,  5, -4,  8,-14,  6, -2, 10,-16,  4},
      {  2,-10, 12, -8,  0,-12, 14, -6,  3, -9, 13, -7,  1,-11, 15, -5}
    };

  CacheView
    *image_view;

  int
    value,
    y;

  PixelInfo
    color;

  register char
    *q;

  register const Quantum
    *p;

  register int
    i,
    j,
    x;

  unsigned int
    scanline_pad;

  register size_t
    pixel;

  unsigned char
    *blue_map[2][16],
    *green_map[2][16],
    *red_map[2][16];

  /*
    Allocate and initialize dither maps.
  */
  for (i=0; i < 2; i++)
    for (j=0; j < 16; j++)
    {
      red_map[i][j]=(unsigned char *) AcquireCriticalMemory(256UL*
        sizeof(*red_map));
      green_map[i][j]=(unsigned char *) AcquireCriticalMemory(256UL*
        sizeof(*green_map));
      blue_map[i][j]=(unsigned char *) AcquireCriticalMemory(256UL*
        sizeof(*blue_map));
    }
  /*
    Initialize dither tables.
  */
  for (i=0; i < 2; i++)
    for (j=0; j < 16; j++)
      for (x=0; x < 256; x++)
      {
        value=x-16;
        if (x < 48)
          value=x/2+8;
        value+=dither_red[i][j];
        red_map[i][j][x]=(unsigned char)
          ((value < 0) ? 0 : (value > 255) ? 255 : value);
        value=x-16;
        if (x < 48)
          value=x/2+8;
        value+=dither_green[i][j];
        green_map[i][j][x]=(unsigned char)
          ((value < 0) ? 0 : (value > 255) ? 255 : value);
        value=x-32;
        if (x < 112)
          value=x/2+24;
        value+=((size_t) dither_blue[i][j] << 1);
        blue_map[i][j][x]=(unsigned char)
          ((value < 0) ? 0 : (value > 255) ? 255 : value);
      }
  /*
    Dither image.
  */
  scanline_pad=(unsigned int) (ximage->bytes_per_line-
    ((size_t) (ximage->width*ximage->bits_per_pixel) >> 3));
  i=0;
  j=0;
  q=ximage->data;
  image_view=AcquireVirtualCacheView(image,exception);
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetCacheViewVirtualPixels(image_view,0,(ssize_t) y,image->columns,1,
      exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      color.red=(double) ClampToQuantum((double) (red_map[i][j][
        (int) ScaleQuantumToChar(GetPixelRed(image,p))] << 8));
      color.green=(double) ClampToQuantum((double) (green_map[i][j][
        (int) ScaleQuantumToChar(GetPixelGreen(image,p))] << 8));
      color.blue=(double) ClampToQuantum((double) (blue_map[i][j][
        (int) ScaleQuantumToChar(GetPixelBlue(image,p))] << 8));
      pixel=(size_t) (((size_t) color.red & 0xe0) |
        (((size_t) color.green & 0xe0) >> 3) |
        (((size_t) color.blue & 0xc0) >> 6));
      *q++=(char) pixel;
      p+=GetPixelChannels(image);
      j++;
      if (j == 16)
        j=0;
    }
    q+=scanline_pad;
    i++;
    if (i == 2)
      i=0;
  }
  image_view=DestroyCacheView(image_view);
  /*
    Free allocated memory.
  */
  for (i=0; i < 2; i++)
    for (j=0; j < 16; j++)
    {
      green_map[i][j]=(unsigned char *) RelinquishMagickMemory(green_map[i][j]);
      blue_map[i][j]=(unsigned char *) RelinquishMagickMemory(blue_map[i][j]);
      red_map[i][j]=(unsigned char *) RelinquishMagickMemory(red_map[i][j]);
    }
}