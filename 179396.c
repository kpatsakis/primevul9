static MagickBooleanType WriteMATImage(const ImageInfo *image_info,Image *image)
{
  char
    MATLAB_HDR[0x80];

  ExceptionInfo
    *exception;

  MagickBooleanType
    status;

  MagickOffsetType
    scene;

  size_t
    imageListLength;

  struct tm
    local_time;

  time_t
    current_time;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"enter MAT");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(MagickFalse);
  image->depth=8;

  current_time=time((time_t *) NULL);
#if defined(MAGICKCORE_HAVE_LOCALTIME_R)
  (void) localtime_r(&current_time,&local_time);
#else
  (void) memcpy(&local_time,localtime(&current_time),sizeof(local_time));
#endif
  (void) memset(MATLAB_HDR,' ',MagickMin(sizeof(MATLAB_HDR),124));
  FormatLocaleString(MATLAB_HDR,sizeof(MATLAB_HDR),
    "MATLAB 5.0 MAT-file, Platform: %s, Created on: %s %s %2d %2d:%2d:%2d %d",
    OsDesc,DayOfWTab[local_time.tm_wday],MonthsTab[local_time.tm_mon],
    local_time.tm_mday,local_time.tm_hour,local_time.tm_min,
    local_time.tm_sec,local_time.tm_year+1900);
  MATLAB_HDR[0x7C]=0;
  MATLAB_HDR[0x7D]=1;
  MATLAB_HDR[0x7E]='I';
  MATLAB_HDR[0x7F]='M';
  (void) WriteBlob(image,sizeof(MATLAB_HDR),(unsigned char *) MATLAB_HDR);
  scene=0;
  imageListLength=GetImageListLength(image);
  do
  {
    char
      padding;

    MagickBooleanType
      is_gray;

    QuantumInfo
      *quantum_info;

    size_t
      data_size;

    unsigned char
      *pixels;

    unsigned int
      z;

    (void) TransformImageColorspace(image,sRGBColorspace);
    is_gray=SetImageGray(image,&image->exception);
    z=(is_gray != MagickFalse) ? 0 : 3;

    /*
      Store MAT header.
    */
    data_size=image->rows*image->columns;
    if (is_gray == MagickFalse)
      data_size*=3;
    padding=((unsigned char)(data_size-1) & 0x7) ^ 0x7;

    (void) WriteBlobLSBLong(image,miMATRIX);
    (void) WriteBlobLSBLong(image,(unsigned int) data_size+padding+
      ((is_gray != MagickFalse) ? 48 : 56));
    (void) WriteBlobLSBLong(image,0x6); /* 0x88 */
    (void) WriteBlobLSBLong(image,0x8); /* 0x8C */
    (void) WriteBlobLSBLong(image,0x6); /* 0x90 */
    (void) WriteBlobLSBLong(image,0);
    (void) WriteBlobLSBLong(image,0x5); /* 0x98 */
    (void) WriteBlobLSBLong(image,(is_gray != MagickFalse) ? 0x8 : 0xC); /* 0x9C - DimFlag */
    (void) WriteBlobLSBLong(image,(unsigned int) image->rows);    /* x: 0xA0 */
    (void) WriteBlobLSBLong(image,(unsigned int) image->columns); /* y: 0xA4 */
    if (is_gray == MagickFalse)
      {
        (void) WriteBlobLSBLong(image,3); /* z: 0xA8 */
        (void) WriteBlobLSBLong(image,0);
      }
    (void) WriteBlobLSBShort(image,1);  /* 0xB0 */
    (void) WriteBlobLSBShort(image,1);  /* 0xB2 */
    (void) WriteBlobLSBLong(image,'M'); /* 0xB4 */
    (void) WriteBlobLSBLong(image,0x2); /* 0xB8 */
    (void) WriteBlobLSBLong(image,(unsigned int) data_size); /* 0xBC */

    /*
      Store image data.
    */
    exception=(&image->exception);
    quantum_info=AcquireQuantumInfo(image_info,image);
    if (quantum_info == (QuantumInfo *) NULL)
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    pixels=GetQuantumPixels(quantum_info);
    do
    {
      const PixelPacket
        *p;

      ssize_t
        y;

      for (y=0; y < (ssize_t)image->columns; y++)
      {
        p=GetVirtualPixels(image,y,0,1,image->rows,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        (void) ExportQuantumPixels(image,(const CacheView *) NULL,quantum_info,
          z2qtype[z],pixels,exception);
        (void) WriteBlob(image,image->rows,pixels);
      }
      if (!SyncAuthenticPixels(image,exception))
        break;
    } while (z-- >= 2);
    while (padding-- > 0)
      (void) WriteBlobByte(image,0);
    quantum_info=DestroyQuantumInfo(quantum_info);
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,imageListLength);
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  (void) CloseBlob(image);
  return(status);
}