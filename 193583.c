static Image *RenderHoughLines(const ImageInfo *image_info,const size_t columns,
  const size_t rows,ExceptionInfo *exception)
{
#define BoundingBox  "viewbox"

  DrawInfo
    *draw_info;

  Image
    *image;

  MagickBooleanType
    status;

  /*
    Open image.
  */
  image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  image->columns=columns;
  image->rows=rows;
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  draw_info->affine.sx=image->resolution.x == 0.0 ? 1.0 : image->resolution.x/
    DefaultResolution;
  draw_info->affine.sy=image->resolution.y == 0.0 ? 1.0 : image->resolution.y/
    DefaultResolution;
  image->columns=(size_t) (draw_info->affine.sx*image->columns);
  image->rows=(size_t) (draw_info->affine.sy*image->rows);
  status=SetImageExtent(image,image->columns,image->rows,exception);
  if (status == MagickFalse)
    return(DestroyImageList(image));
  if (SetImageBackgroundColor(image,exception) == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Render drawing.
  */
  if (GetBlobStreamData(image) == (unsigned char *) NULL)
    draw_info->primitive=FileToString(image->filename,~0UL,exception);
  else
    {
      draw_info->primitive=(char *) AcquireMagickMemory((size_t)
        GetBlobSize(image)+1);
      if (draw_info->primitive != (char *) NULL)
        {
          (void) memcpy(draw_info->primitive,GetBlobStreamData(image),
            (size_t) GetBlobSize(image));
          draw_info->primitive[GetBlobSize(image)]='\0';
        }
     }
  (void) DrawImage(image,draw_info,exception);
  draw_info=DestroyDrawInfo(draw_info);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}