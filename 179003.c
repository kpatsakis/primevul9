static RectangleInfo CompareImagesBounds(const Image *image1,
  const Image *image2,const LayerMethod method,ExceptionInfo *exception)
{
  RectangleInfo
    bounds;

  PixelInfo
    pixel1,
    pixel2;

  register const Quantum
    *p,
    *q;

  register ssize_t
    x;

  ssize_t
    y;

  /*
    Set bounding box of the differences between images.
  */
  GetPixelInfo(image1,&pixel1);
  GetPixelInfo(image2,&pixel2);
  for (x=0; x < (ssize_t) image1->columns; x++)
  {
    p=GetVirtualPixels(image1,x,0,1,image1->rows,exception);
    q=GetVirtualPixels(image2,x,0,1,image2->rows,exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      break;
    for (y=0; y < (ssize_t) image1->rows; y++)
    {
      GetPixelInfoPixel(image1,p,&pixel1);
      GetPixelInfoPixel(image2,q,&pixel2);
      if (ComparePixels(method,&pixel1,&pixel2))
        break;
      p+=GetPixelChannels(image1);
      q+=GetPixelChannels(image2);
    }
    if (y < (ssize_t) image1->rows)
      break;
  }
  if (x >= (ssize_t) image1->columns)
    {
      /*
        Images are identical, return a null image.
      */
      bounds.x=-1;
      bounds.y=-1;
      bounds.width=1;
      bounds.height=1;
      return(bounds);
    }
  bounds.x=x;
  for (x=(ssize_t) image1->columns-1; x >= 0; x--)
  {
    p=GetVirtualPixels(image1,x,0,1,image1->rows,exception);
    q=GetVirtualPixels(image2,x,0,1,image2->rows,exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      break;
    for (y=0; y < (ssize_t) image1->rows; y++)
    {
      GetPixelInfoPixel(image1,p,&pixel1);
      GetPixelInfoPixel(image2,q,&pixel2);
      if (ComparePixels(method,&pixel1,&pixel2))
        break;
      p+=GetPixelChannels(image1);
      q+=GetPixelChannels(image2);
    }
    if (y < (ssize_t) image1->rows)
      break;
  }
  bounds.width=(size_t) (x-bounds.x+1);
  for (y=0; y < (ssize_t) image1->rows; y++)
  {
    p=GetVirtualPixels(image1,0,y,image1->columns,1,exception);
    q=GetVirtualPixels(image2,0,y,image2->columns,1,exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      break;
    for (x=0; x < (ssize_t) image1->columns; x++)
    {
      GetPixelInfoPixel(image1,p,&pixel1);
      GetPixelInfoPixel(image2,q,&pixel2);
      if (ComparePixels(method,&pixel1,&pixel2))
        break;
      p+=GetPixelChannels(image1);
      q+=GetPixelChannels(image2);
    }
    if (x < (ssize_t) image1->columns)
      break;
  }
  bounds.y=y;
  for (y=(ssize_t) image1->rows-1; y >= 0; y--)
  {
    p=GetVirtualPixels(image1,0,y,image1->columns,1,exception);
    q=GetVirtualPixels(image2,0,y,image2->columns,1,exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      break;
    for (x=0; x < (ssize_t) image1->columns; x++)
    {
      GetPixelInfoPixel(image1,p,&pixel1);
      GetPixelInfoPixel(image2,q,&pixel2);
      if (ComparePixels(method,&pixel1,&pixel2))
        break;
      p+=GetPixelChannels(image1);
      q+=GetPixelChannels(image2);
    }
    if (x < (ssize_t) image1->columns)
      break;
  }
  bounds.height=(size_t) (y-bounds.y+1);
  return(bounds);
}