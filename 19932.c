static MagickBooleanType WriteCINImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  char
    timestamp[MagickPathExtent];

  const char
    *value;

  CINInfo
    cin;

  const StringInfo
    *profile;

  MagickBooleanType
    status;

  MagickOffsetType
    offset;

  QuantumInfo
    *quantum_info;

  QuantumType
    quantum_type;

  const Quantum
    *p;

  ssize_t
    i;

  size_t
    length;

  ssize_t
    count,
    y;

  struct tm
    utc_time;

  time_t
    seconds;

  unsigned char
    *pixels;

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
  if (image->colorspace != LogColorspace)
    (void) TransformImageColorspace(image,LogColorspace,exception);
  /*
    Write image information.
  */
  (void) memset(&cin,0,sizeof(cin));
  offset=0;
  cin.file.magic=0x802A5FD7UL;
  offset+=WriteBlobLong(image,(unsigned int) cin.file.magic);
  cin.file.image_offset=0x800;
  offset+=WriteBlobLong(image,(unsigned int) cin.file.image_offset);
  cin.file.generic_length=0x400;
  offset+=WriteBlobLong(image,(unsigned int) cin.file.generic_length);
  cin.file.industry_length=0x400;
  offset+=WriteBlobLong(image,(unsigned int) cin.file.industry_length);
  cin.file.user_length=0x00;
  profile=GetImageProfile(image,"dpx:user.data");
  if (profile != (StringInfo *) NULL)
    {
      cin.file.user_length+=(size_t) GetStringInfoLength(profile);
      cin.file.user_length=(((cin.file.user_length+0x2000-1)/0x2000)*0x2000);
    }
  offset+=WriteBlobLong(image,(unsigned int) cin.file.user_length);
  cin.file.file_size=4*image->columns*image->rows+0x2000;
  offset+=WriteBlobLong(image,(unsigned int) cin.file.file_size);
  (void) CopyMagickString(cin.file.version,"V4.5",sizeof(cin.file.version));
  offset+=WriteBlob(image,sizeof(cin.file.version),(unsigned char *)
    cin.file.version);
  value=GetCINProperty(image_info,image,"dpx:file.filename",exception);
  if (value != (const char *) NULL)
    (void) CopyMagickString(cin.file.filename,value,sizeof(cin.file.filename));
  else
    (void) CopyMagickString(cin.file.filename,image->filename,
      sizeof(cin.file.filename));
  offset+=WriteBlob(image,sizeof(cin.file.filename),(unsigned char *)
    cin.file.filename);
  seconds=GetMagickTime();
  GetMagickUTCtime(&seconds,&utc_time);
  (void) memset(timestamp,0,sizeof(timestamp));
  (void) strftime(timestamp,MagickPathExtent,"%Y:%m:%d:%H:%M:%SUTC",&utc_time);
  (void) memset(cin.file.create_date,0,sizeof(cin.file.create_date));
  (void) CopyMagickString(cin.file.create_date,timestamp,11);
  offset+=WriteBlob(image,sizeof(cin.file.create_date),(unsigned char *)
    cin.file.create_date);
  (void) memset(cin.file.create_time,0,sizeof(cin.file.create_time));
  (void) CopyMagickString(cin.file.create_time,timestamp+11,11);
  offset+=WriteBlob(image,sizeof(cin.file.create_time),(unsigned char *)
    cin.file.create_time);
  offset+=WriteBlob(image,sizeof(cin.file.reserve),(unsigned char *)
    cin.file.reserve);
  cin.image.orientation=0x00;
  offset+=WriteBlobByte(image,cin.image.orientation);
  cin.image.number_channels=3;
  offset+=WriteBlobByte(image,cin.image.number_channels);
  offset+=WriteBlob(image,sizeof(cin.image.reserve1),(unsigned char *)
    cin.image.reserve1);
  for (i=0; i < 8; i++)
  {
    cin.image.channel[i].designator[0]=0; /* universal metric */
    offset+=WriteBlobByte(image,cin.image.channel[0].designator[0]);
    cin.image.channel[i].designator[1]=(unsigned char) (i > 3 ? 0 : i+1); /* channel color */;
    offset+=WriteBlobByte(image,cin.image.channel[1].designator[0]);
    cin.image.channel[i].bits_per_pixel=(unsigned char) image->depth;
    offset+=WriteBlobByte(image,cin.image.channel[0].bits_per_pixel);
    offset+=WriteBlobByte(image,cin.image.channel[0].reserve);
    cin.image.channel[i].pixels_per_line=image->columns;
    offset+=WriteBlobLong(image,(unsigned int)
      cin.image.channel[0].pixels_per_line);
    cin.image.channel[i].lines_per_image=image->rows;
    offset+=WriteBlobLong(image,(unsigned int)
      cin.image.channel[0].lines_per_image);
    cin.image.channel[i].min_data=0;
    offset+=WriteBlobFloat(image,cin.image.channel[0].min_data);
    cin.image.channel[i].min_quantity=0.0;
    offset+=WriteBlobFloat(image,cin.image.channel[0].min_quantity);
    cin.image.channel[i].max_data=(float) ((MagickOffsetType)
      GetQuantumRange(image->depth));
    offset+=WriteBlobFloat(image,cin.image.channel[0].max_data);
    cin.image.channel[i].max_quantity=2.048f;
    offset+=WriteBlobFloat(image,cin.image.channel[0].max_quantity);
  }
  offset+=WriteBlobFloat(image,image->chromaticity.white_point.x);
  offset+=WriteBlobFloat(image,image->chromaticity.white_point.y);
  offset+=WriteBlobFloat(image,image->chromaticity.red_primary.x);
  offset+=WriteBlobFloat(image,image->chromaticity.red_primary.y);
  offset+=WriteBlobFloat(image,image->chromaticity.green_primary.x);
  offset+=WriteBlobFloat(image,image->chromaticity.green_primary.y);
  offset+=WriteBlobFloat(image,image->chromaticity.blue_primary.x);
  offset+=WriteBlobFloat(image,image->chromaticity.blue_primary.y);
  value=GetCINProperty(image_info,image,"dpx:image.label",exception);
  if (value != (const char *) NULL)
    (void) CopyMagickString(cin.image.label,value,sizeof(cin.image.label));
  offset+=WriteBlob(image,sizeof(cin.image.label),(unsigned char *)
    cin.image.label);
  offset+=WriteBlob(image,sizeof(cin.image.reserve),(unsigned char *)
    cin.image.reserve);
  /*
    Write data format information.
  */
  cin.data_format.interleave=0; /* pixel interleave (rgbrgbr...) */
  offset+=WriteBlobByte(image,cin.data_format.interleave);
  cin.data_format.packing=5; /* packing ssize_tword (32bit) boundaries */
  offset+=WriteBlobByte(image,cin.data_format.packing);
  cin.data_format.sign=0; /* unsigned data */
  offset+=WriteBlobByte(image,cin.data_format.sign);
  cin.data_format.sense=0; /* image sense: positive image */
  offset+=WriteBlobByte(image,cin.data_format.sense);
  cin.data_format.line_pad=0;
  offset+=WriteBlobLong(image,(unsigned int) cin.data_format.line_pad);
  cin.data_format.channel_pad=0;
  offset+=WriteBlobLong(image,(unsigned int) cin.data_format.channel_pad);
  offset+=WriteBlob(image,sizeof(cin.data_format.reserve),(unsigned char *)
    cin.data_format.reserve);
  /*
    Write origination information.
  */
  cin.origination.x_offset=0UL;
  value=GetCINProperty(image_info,image,"dpx:origination.x_offset",exception);
  if (value != (const char *) NULL)
    cin.origination.x_offset=(ssize_t) StringToLong(value);
  offset+=WriteBlobLong(image,(unsigned int) cin.origination.x_offset);
  cin.origination.y_offset=0UL;
  value=GetCINProperty(image_info,image,"dpx:origination.y_offset",exception);
  if (value != (const char *) NULL)
    cin.origination.y_offset=(ssize_t) StringToLong(value);
  offset+=WriteBlobLong(image,(unsigned int) cin.origination.y_offset);
  value=GetCINProperty(image_info,image,"dpx:origination.filename",exception);
  if (value != (const char *) NULL)
    (void) CopyMagickString(cin.origination.filename,value,
      sizeof(cin.origination.filename));
  else
    (void) CopyMagickString(cin.origination.filename,image->filename,
      sizeof(cin.origination.filename));
  offset+=WriteBlob(image,sizeof(cin.origination.filename),(unsigned char *)
    cin.origination.filename);
  (void) memset(timestamp,0,sizeof(timestamp));
  (void) strftime(timestamp,MagickPathExtent,"%Y:%m:%d:%H:%M:%SUTC",&utc_time);
  (void) memset(cin.origination.create_date,0,
    sizeof(cin.origination.create_date));
  (void) CopyMagickString(cin.origination.create_date,timestamp,11);
  offset+=WriteBlob(image,sizeof(cin.origination.create_date),(unsigned char *)
    cin.origination.create_date);
  (void) memset(cin.origination.create_time,0,
     sizeof(cin.origination.create_time));
  (void) CopyMagickString(cin.origination.create_time,timestamp+11,15);
  offset+=WriteBlob(image,sizeof(cin.origination.create_time),(unsigned char *)
    cin.origination.create_time);
  value=GetCINProperty(image_info,image,"dpx:origination.device",exception);
  if (value != (const char *) NULL)
    (void) CopyMagickString(cin.origination.device,value,
      sizeof(cin.origination.device));
  offset+=WriteBlob(image,sizeof(cin.origination.device),(unsigned char *)
    cin.origination.device);
  value=GetCINProperty(image_info,image,"dpx:origination.model",exception);
  if (value != (const char *) NULL)
    (void) CopyMagickString(cin.origination.model,value,
      sizeof(cin.origination.model));
  offset+=WriteBlob(image,sizeof(cin.origination.model),(unsigned char *)
    cin.origination.model);
  value=GetCINProperty(image_info,image,"dpx:origination.serial",exception);
  if (value != (const char *) NULL)
    (void) CopyMagickString(cin.origination.serial,value,
      sizeof(cin.origination.serial));
  offset+=WriteBlob(image,sizeof(cin.origination.serial),(unsigned char *)
    cin.origination.serial);
  cin.origination.x_pitch=0.0f;
  value=GetCINProperty(image_info,image,"dpx:origination.x_pitch",exception);
  if (value != (const char *) NULL)
    cin.origination.x_pitch=StringToDouble(value,(char **) NULL);
  offset+=WriteBlobFloat(image,cin.origination.x_pitch);
  cin.origination.y_pitch=0.0f;
  value=GetCINProperty(image_info,image,"dpx:origination.y_pitch",exception);
  if (value != (const char *) NULL)
    cin.origination.y_pitch=StringToDouble(value,(char **) NULL);
  offset+=WriteBlobFloat(image,cin.origination.y_pitch);
  cin.origination.gamma=image->gamma;
  offset+=WriteBlobFloat(image,cin.origination.gamma);
  offset+=WriteBlob(image,sizeof(cin.origination.reserve),(unsigned char *)
    cin.origination.reserve);
  /*
    Image film information.
  */
  cin.film.id=0;
  value=GetCINProperty(image_info,image,"dpx:film.id",exception);
  if (value != (const char *) NULL)
    cin.film.id=(char) StringToLong(value);
  offset+=WriteBlobByte(image,(unsigned char) cin.film.id);
  cin.film.type=0;
  value=GetCINProperty(image_info,image,"dpx:film.type",exception);
  if (value != (const char *) NULL)
    cin.film.type=(char) StringToLong(value);
  offset+=WriteBlobByte(image,(unsigned char) cin.film.type);
  cin.film.offset=0;
  value=GetCINProperty(image_info,image,"dpx:film.offset",exception);
  if (value != (const char *) NULL)
    cin.film.offset=(char) StringToLong(value);
  offset+=WriteBlobByte(image,(unsigned char) cin.film.offset);
  offset+=WriteBlobByte(image,(unsigned char) cin.film.reserve1);
  cin.film.prefix=0UL;
  value=GetCINProperty(image_info,image,"dpx:film.prefix",exception);
  if (value != (const char *) NULL)
    cin.film.prefix=StringToUnsignedLong(value);
  offset+=WriteBlobLong(image,(unsigned int) cin.film.prefix);
  cin.film.count=0UL;
  value=GetCINProperty(image_info,image,"dpx:film.count",exception);
  if (value != (const char *) NULL)
    cin.film.count=StringToUnsignedLong(value);
  offset+=WriteBlobLong(image,(unsigned int) cin.film.count);
  value=GetCINProperty(image_info,image,"dpx:film.format",exception);
  if (value != (const char *) NULL)
    (void) CopyMagickString(cin.film.format,value,sizeof(cin.film.format));
  offset+=WriteBlob(image,sizeof(cin.film.format),(unsigned char *)
    cin.film.format);
  cin.film.frame_position=0UL;
  value=GetCINProperty(image_info,image,"dpx:film.frame_position",exception);
  if (value != (const char *) NULL)
    cin.film.frame_position=StringToUnsignedLong(value);
  offset+=WriteBlobLong(image,(unsigned int) cin.film.frame_position);
  cin.film.frame_rate=0.0f;
  value=GetCINProperty(image_info,image,"dpx:film.frame_rate",exception);
  if (value != (const char *) NULL)
    cin.film.frame_rate=StringToDouble(value,(char **) NULL);
  offset+=WriteBlobFloat(image,cin.film.frame_rate);
  value=GetCINProperty(image_info,image,"dpx:film.frame_id",exception);
  if (value != (const char *) NULL)
    (void) CopyMagickString(cin.film.frame_id,value,sizeof(cin.film.frame_id));
  offset+=WriteBlob(image,sizeof(cin.film.frame_id),(unsigned char *)
    cin.film.frame_id);
  value=GetCINProperty(image_info,image,"dpx:film.slate_info",exception);
  if (value != (const char *) NULL)
    (void) CopyMagickString(cin.film.slate_info,value,
      sizeof(cin.film.slate_info));
  offset+=WriteBlob(image,sizeof(cin.film.slate_info),(unsigned char *)
    cin.film.slate_info);
  offset+=WriteBlob(image,sizeof(cin.film.reserve),(unsigned char *)
    cin.film.reserve);
  if (profile != (StringInfo *) NULL)
    offset+=WriteBlob(image,GetStringInfoLength(profile),
      GetStringInfoDatum(profile));
  while (offset < (MagickOffsetType) cin.file.image_offset)
    offset+=WriteBlobByte(image,0x00);
  /*
    Convert pixel packets to CIN raster image.
  */
  quantum_info=AcquireQuantumInfo(image_info,image);
  if (quantum_info == (QuantumInfo *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  quantum_info->quantum=32;
  quantum_info->pack=MagickFalse;
  quantum_type=RGBQuantum;
  pixels=(unsigned char *) GetQuantumPixels(quantum_info);
  length=GetBytesPerRow(image->columns,3,image->depth,MagickTrue);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    (void) ExportQuantumPixels(image,(CacheView *) NULL,quantum_info,
      quantum_type,pixels,exception);
    count=WriteBlob(image,length,pixels);
    if (count != (ssize_t) length)
      break;
    status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  quantum_info=DestroyQuantumInfo(quantum_info);
  (void) CloseBlob(image);
  return(status);
}