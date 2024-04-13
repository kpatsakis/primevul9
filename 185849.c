static MagickBooleanType WriteXWDImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  const char
    *value;

  MagickBooleanType
    status;

  register const Quantum
    *p;

  register ssize_t
    x;

  register unsigned char
    *q;

  size_t
    bits_per_pixel,
    bytes_per_line,
    length,
    scanline_pad;

  ssize_t
    y;

  unsigned char
    *pixels;

  unsigned long
    lsb_first;

  XWDFileHeader
    xwd_info;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  (void) TransformImageColorspace(image,sRGBColorspace,exception);
  /*
    Initialize XWD file header.
  */
  (void) ResetMagickMemory(&xwd_info,0,sizeof(xwd_info));
  xwd_info.header_size=(CARD32) sz_XWDheader;
  value=GetImageProperty(image,"comment",exception);
  if (value != (const char *) NULL)
    xwd_info.header_size+=(CARD32) strlen(value);
  xwd_info.header_size++;
  xwd_info.file_version=(CARD32) XWD_FILE_VERSION;
  xwd_info.pixmap_format=(CARD32) ZPixmap;
  xwd_info.pixmap_depth=(CARD32) (image->storage_class == DirectClass ? 24 : 8);
  xwd_info.pixmap_width=(CARD32) image->columns;
  xwd_info.pixmap_height=(CARD32) image->rows;
  xwd_info.xoffset=(CARD32) 0;
  xwd_info.byte_order=(CARD32) MSBFirst;
  xwd_info.bitmap_unit=(CARD32) (image->storage_class == DirectClass ? 32 : 8);
  xwd_info.bitmap_bit_order=(CARD32) MSBFirst;
  xwd_info.bitmap_pad=(CARD32) (image->storage_class == DirectClass ? 32 : 8);
  bits_per_pixel=(size_t) (image->storage_class == DirectClass ? 24 : 8);
  xwd_info.bits_per_pixel=(CARD32) bits_per_pixel;
  bytes_per_line=(CARD32) ((((xwd_info.bits_per_pixel*
    xwd_info.pixmap_width)+((xwd_info.bitmap_pad)-1))/
    (xwd_info.bitmap_pad))*((xwd_info.bitmap_pad) >> 3));
  xwd_info.bytes_per_line=(CARD32) bytes_per_line;
  xwd_info.visual_class=(CARD32)
    (image->storage_class == DirectClass ? DirectColor : PseudoColor);
  xwd_info.red_mask=(CARD32)
    (image->storage_class == DirectClass ? 0xff0000 : 0);
  xwd_info.green_mask=(CARD32)
    (image->storage_class == DirectClass ? 0xff00 : 0);
  xwd_info.blue_mask=(CARD32) (image->storage_class == DirectClass ? 0xff : 0);
  xwd_info.bits_per_rgb=(CARD32) (image->storage_class == DirectClass ? 24 : 8);
  xwd_info.colormap_entries=(CARD32)
    (image->storage_class == DirectClass ? 256 : image->colors);
  xwd_info.ncolors=(unsigned int)
    (image->storage_class == DirectClass ? 0 : image->colors);
  xwd_info.window_width=(CARD32) image->columns;
  xwd_info.window_height=(CARD32) image->rows;
  xwd_info.window_x=0;
  xwd_info.window_y=0;
  xwd_info.window_bdrwidth=(CARD32) 0;
  /*
    Write XWD header.
  */
  lsb_first=1;
  if ((int) (*(char *) &lsb_first) != 0)
    MSBOrderLong((unsigned char *) &xwd_info,sizeof(xwd_info));
  (void) WriteBlob(image,sz_XWDheader,(unsigned char *) &xwd_info);
  if (value != (const char *) NULL)
    (void) WriteBlob(image,strlen(value),(unsigned char *) value);
  (void) WriteBlob(image,1,(const unsigned char *) "\0");
  if (image->storage_class == PseudoClass)
    {
      register ssize_t
        i;

      XColor
        *colors;

      XWDColor
        color;

      /*
        Dump colormap to file.
      */
      (void) ResetMagickMemory(&color,0,sizeof(color));
      colors=(XColor *) AcquireQuantumMemory((size_t) image->colors,
        sizeof(*colors));
      if (colors == (XColor *) NULL)
        ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
      for (i=0; i < (ssize_t) image->colors; i++)
      {
        colors[i].pixel=(unsigned long) i;
        colors[i].red=ScaleQuantumToShort(ClampToQuantum(
          image->colormap[i].red));
        colors[i].green=ScaleQuantumToShort(ClampToQuantum(
          image->colormap[i].green));
        colors[i].blue=ScaleQuantumToShort(ClampToQuantum(
          image->colormap[i].blue));
        colors[i].flags=(char) (DoRed | DoGreen | DoBlue);
        colors[i].pad='\0';
        if ((int) (*(char *) &lsb_first) != 0)
          {
            MSBOrderLong((unsigned char *) &colors[i].pixel,
              sizeof(colors[i].pixel));
            MSBOrderShort((unsigned char *) &colors[i].red,
              3*sizeof(colors[i].red));
          }
      }
      for (i=0; i < (ssize_t) image->colors; i++)
      {
        color.pixel=(CARD32) colors[i].pixel;
        color.red=colors[i].red;
        color.green=colors[i].green;
        color.blue=colors[i].blue;
        color.flags=(CARD8) colors[i].flags;
        (void) WriteBlob(image,sz_XWDColor,(unsigned char *) &color);
      }
      colors=(XColor *) RelinquishMagickMemory(colors);
    }
  /*
    Allocate memory for pixels.
  */
  length=3*bytes_per_line;
  if (image->storage_class == PseudoClass)
    length=bytes_per_line;
  pixels=(unsigned char *) AcquireQuantumMemory(length,sizeof(*pixels));
  if (pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  (void) ResetMagickMemory(pixels,0,length);
  /*
    Convert MIFF to XWD raster pixels.
  */
  scanline_pad=(bytes_per_line-((image->columns*bits_per_pixel) >> 3));
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    q=pixels;
    if (image->storage_class == PseudoClass)
      {
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          *q++=(unsigned char) GetPixelIndex(image,p);
          p+=GetPixelChannels(image);
        }
      }
    else
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        *q++=ScaleQuantumToChar(GetPixelRed(image,p));
        *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
        *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
        p+=GetPixelChannels(image);
      }
    for (x=0; x < (ssize_t) scanline_pad; x++)
      *q++='\0';
    (void) WriteBlob(image,(size_t) (q-pixels),pixels);
    status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  pixels=(unsigned char *) RelinquishMagickMemory(pixels);
  (void) CloseBlob(image);
  return(MagickTrue);
}