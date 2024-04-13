static MagickBooleanType WritePCDTile(Image *image,const char *page_geometry,
  const char *tile_geometry)
{
  GeometryInfo
    geometry_info;

  Image
    *downsample_image,
    *tile_image;

  MagickBooleanType
    status;

  MagickStatusType
    flags;

  RectangleInfo
    geometry;

  register const PixelPacket
    *p,
    *q;

  register ssize_t
    i,
    x;

  ssize_t
    y;

  /*
    Scale image to tile size.
  */
  SetGeometry(image,&geometry);
  (void) ParseMetaGeometry(page_geometry,&geometry.x,&geometry.y,
    &geometry.width,&geometry.height);
  if ((geometry.width % 2) != 0)
    geometry.width--;
  if ((geometry.height % 2) != 0)
    geometry.height--;
  tile_image=ResizeImage(image,geometry.width,geometry.height,TriangleFilter,
    1.0,&image->exception);
  if (tile_image == (Image *) NULL)
    return(MagickFalse);
  flags=ParseGeometry(page_geometry,&geometry_info);
  geometry.width=(size_t) geometry_info.rho;
  geometry.height=(size_t) geometry_info.sigma;
  if ((flags & SigmaValue) == 0)
    geometry.height=geometry.width;
  if ((tile_image->columns != geometry.width) ||
      (tile_image->rows != geometry.height))
    {
      Image
        *bordered_image;

      RectangleInfo
        border_info;

      /*
        Put a border around the image.
      */
      border_info.width=(geometry.width-tile_image->columns+1) >> 1;
      border_info.height=(geometry.height-tile_image->rows+1) >> 1;
      bordered_image=BorderImage(tile_image,&border_info,&image->exception);
      if (bordered_image == (Image *) NULL)
        return(MagickFalse);
      tile_image=DestroyImage(tile_image);
      tile_image=bordered_image;
    }
  (void) TransformImage(&tile_image,(char *) NULL,tile_geometry);
  (void) TransformImageColorspace(tile_image,YCCColorspace);
  downsample_image=ResizeImage(tile_image,tile_image->columns/2,
    tile_image->rows/2,TriangleFilter,1.0,&image->exception);
  if (downsample_image == (Image *) NULL)
    return(MagickFalse);
  /*
    Write tile to PCD file.
  */
  for (y=0; y < (ssize_t) tile_image->rows; y+=2)
  {
    p=GetVirtualPixels(tile_image,0,y,tile_image->columns,2,
      &tile_image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) (tile_image->columns << 1); x++)
    {
      (void) WriteBlobByte(image,ScaleQuantumToChar(GetPixelRed(p)));
      p++;
    }
    q=GetVirtualPixels(downsample_image,0,y >> 1,downsample_image->columns,
      1,&downsample_image->exception);
    if (q == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) downsample_image->columns; x++)
    {
      (void) WriteBlobByte(image,ScaleQuantumToChar(GetPixelGreen(q)));
      q++;
    }
    q=GetVirtualPixels(downsample_image,0,y >> 1,downsample_image->columns,
      1,&downsample_image->exception);
    if (q == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) downsample_image->columns; x++)
    {
      (void) WriteBlobByte(image,ScaleQuantumToChar(GetPixelBlue(q)));
      q++;
    }
    status=SetImageProgress(image,SaveImageTag,y,tile_image->rows);
    if (status == MagickFalse)
      break;
  }
  for (i=0; i < 0x800; i++)
    (void) WriteBlobByte(image,'\0');
  downsample_image=DestroyImage(downsample_image);
  tile_image=DestroyImage(tile_image);
  return(MagickTrue);
}