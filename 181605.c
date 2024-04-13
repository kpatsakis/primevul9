MagickExport Image *RollImage(const Image *image,const ssize_t x_offset,
  const ssize_t y_offset,ExceptionInfo *exception)
{
#define RollImageTag  "Roll/Image"

  Image
    *roll_image;

  MagickStatusType
    status;

  RectangleInfo
    offset;

  /*
    Initialize roll image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  roll_image=CloneImage(image,0,0,MagickTrue,exception);
  if (roll_image == (Image *) NULL)
    return((Image *) NULL);
  offset.x=x_offset;
  offset.y=y_offset;
  while (offset.x < 0)
    offset.x+=(ssize_t) image->columns;
  while (offset.x >= (ssize_t) image->columns)
    offset.x-=(ssize_t) image->columns;
  while (offset.y < 0)
    offset.y+=(ssize_t) image->rows;
  while (offset.y >= (ssize_t) image->rows)
    offset.y-=(ssize_t) image->rows;
  /*
    Roll image.
  */
  status=CopyImageRegion(roll_image,image,(size_t) offset.x,
    (size_t) offset.y,(ssize_t) image->columns-offset.x,(ssize_t) image->rows-
    offset.y,0,0,exception);
  (void) SetImageProgress(image,RollImageTag,0,3);
  status&=CopyImageRegion(roll_image,image,image->columns-offset.x,
    (size_t) offset.y,0,(ssize_t) image->rows-offset.y,offset.x,0,
    exception);
  (void) SetImageProgress(image,RollImageTag,1,3);
  status&=CopyImageRegion(roll_image,image,(size_t) offset.x,image->rows-
    offset.y,(ssize_t) image->columns-offset.x,0,0,offset.y,exception);
  (void) SetImageProgress(image,RollImageTag,2,3);
  status&=CopyImageRegion(roll_image,image,image->columns-offset.x,image->rows-
    offset.y,0,0,offset.x,offset.y,exception);
  (void) SetImageProgress(image,RollImageTag,3,3);
  roll_image->type=image->type;
  if (status == MagickFalse)
    roll_image=DestroyImage(roll_image);
  return(roll_image);
}