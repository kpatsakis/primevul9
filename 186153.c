static MagickBooleanType WritePS3MaskImage(const ImageInfo *image_info,
  Image *image,const CompressionType compression)
{
  char
    buffer[MaxTextExtent];

  Image
    *mask_image;

  MagickBooleanType
    status;

  MagickOffsetType
    offset,
    start,
    stop;

  MemoryInfo
    *pixel_info;

  register ssize_t
    i;

  size_t
    length;

  unsigned char
    *pixels;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(image->matte != MagickFalse);
  status=MagickTrue;
  /*
    Note BeginData DSC comment for update later.
  */
  start=TellBlob(image);
  if (start < 0)
    ThrowWriterException(CorruptImageError,"ImproperImageHeader");
  (void) FormatLocaleString(buffer,MaxTextExtent,
    "%%%%BeginData:%13ld %s Bytes\n",0L,compression == NoCompression ?
    "ASCII" : "BINARY");
  (void) WriteBlobString(image,buffer);
  stop=TellBlob(image);
  if (stop < 0)
    ThrowWriterException(CorruptImageError,"ImproperImageHeader");
  /*
    Only lossless compressions for the mask.
  */
  switch (compression)
  {
    case NoCompression:
    default:
    {
      (void) FormatLocaleString(buffer,MaxTextExtent,"currentfile %.20g %.20g "
        PS3_NoCompression " ByteStreamDecodeFilter\n",(double) image->columns,
        (double) image->rows);
      break;
    }
    case FaxCompression:
    case Group4Compression:
    {
      (void) FormatLocaleString(buffer,MaxTextExtent,"currentfile %.20g %.20g "
        PS3_FaxCompression " ByteStreamDecodeFilter\n",(double) image->columns,
        (double) image->rows);
      break;
    }
    case LZWCompression:
    {
      (void) FormatLocaleString(buffer,MaxTextExtent,"currentfile %.20g %.20g "
        PS3_LZWCompression " ByteStreamDecodeFilter\n",(double) image->columns,
        (double) image->rows);
      break;
    }
    case RLECompression:
    {
      (void) FormatLocaleString(buffer,MaxTextExtent,"currentfile %.20g %.20g "
        PS3_RLECompression " ByteStreamDecodeFilter\n",(double) image->columns,
        (double) image->rows);
      break;
    }
    case ZipCompression:
    {
      (void) FormatLocaleString(buffer,MaxTextExtent,"currentfile %.20g %.20g "
        PS3_ZipCompression " ByteStreamDecodeFilter\n",(double) image->columns,
        (double) image->rows);
      break;
    }
  }
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"/ReusableStreamDecode filter\n");
  mask_image=CloneImage(image,0,0,MagickTrue,&image->exception);
  if (mask_image == (Image *) NULL)
    ThrowWriterException(CoderError,image->exception.reason);
  status=SeparateImageChannel(mask_image,OpacityChannel);
  if (status == MagickFalse)
    {
      mask_image=DestroyImage(mask_image);
      return(MagickFalse);
    }
  (void) SetImageType(mask_image,BilevelType);
  (void) SetImageType(mask_image,PaletteType);
  mask_image->matte=MagickFalse;
  pixels=(unsigned char *) NULL;
  length=0;
  switch (compression)
  {
    case NoCompression:
    default:
    {
      status=SerializeImageChannel(image_info,mask_image,&pixel_info,&length);
      if (status == MagickFalse)
        break;
      pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
      Ascii85Initialize(image);
      for (i=0; i < (ssize_t) length; i++)
        Ascii85Encode(image,pixels[i]);
      Ascii85Flush(image);
      pixel_info=RelinquishVirtualMemory(pixel_info);
      break;
    }
    case FaxCompression:
    case Group4Compression:
    {
      if ((compression == FaxCompression) ||
          (LocaleCompare(CCITTParam,"0") == 0))
        status=HuffmanEncodeImage(image_info,image,mask_image);
      else
        status=Huffman2DEncodeImage(image_info,image,mask_image);
      break;
    }
    case LZWCompression:
    {
      status=SerializeImageChannel(image_info,mask_image,&pixel_info,&length);
      if (status == MagickFalse)
        break;
      pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
      status=LZWEncodeImage(image,length,pixels);
      pixel_info=RelinquishVirtualMemory(pixel_info);
      break;
    }
    case RLECompression:
    {
      status=SerializeImageChannel(image_info,mask_image,&pixel_info,&length);
      if (status == MagickFalse)
        break;
      pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
      status=PackbitsEncodeImage(image,length,pixels);
      pixel_info=RelinquishVirtualMemory(pixel_info);
      break;
    }
    case ZipCompression:
    {
      status=SerializeImageChannel(image_info,mask_image,&pixel_info,&length);
      if (status == MagickFalse)
        break;
      pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
      status=ZLIBEncodeImage(image,length,pixels);
      pixel_info=RelinquishVirtualMemory(pixel_info);
      break;
    }
  }
  mask_image=DestroyImage(mask_image);
  (void) WriteBlobByte(image,'\n');
  length=(size_t) (TellBlob(image)-stop);
  stop=TellBlob(image);
  if (stop < 0)
    ThrowWriterException(CorruptImageError,"ImproperImageHeader");
  offset=SeekBlob(image,start,SEEK_SET);
  if (offset < 0)
    ThrowWriterException(CorruptImageError,"ImproperImageHeader");
  (void) FormatLocaleString(buffer,MaxTextExtent,
    "%%%%BeginData:%13ld %s Bytes\n",(long) length,
    compression == NoCompression ? "ASCII" : "BINARY");
  (void) WriteBlobString(image,buffer);
  offset=SeekBlob(image,stop,SEEK_SET);
  if (offset < 0)
    ThrowWriterException(CorruptImageError,"ImproperImageHeader");
  (void) WriteBlobString(image,"%%EndData\n");
  (void) WriteBlobString(image, "/mask_stream exch def\n");
  return(status);
}