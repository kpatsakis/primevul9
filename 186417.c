static void MSLPushImage(MSLInfo *msl_info,Image *image)
{
  ssize_t
    n;

  if (image != (Image *) NULL)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(msl_info != (MSLInfo *) NULL);
  msl_info->n++;
  n=msl_info->n;
  msl_info->image_info=(ImageInfo **) ResizeQuantumMemory(msl_info->image_info,
    (n+1),sizeof(*msl_info->image_info));
  msl_info->draw_info=(DrawInfo **) ResizeQuantumMemory(msl_info->draw_info,
    (n+1),sizeof(*msl_info->draw_info));
  msl_info->attributes=(Image **) ResizeQuantumMemory(msl_info->attributes,
    (n+1),sizeof(*msl_info->attributes));
  msl_info->image=(Image **) ResizeQuantumMemory(msl_info->image,(n+1),
    sizeof(*msl_info->image));
  if ((msl_info->image_info == (ImageInfo **) NULL) ||
      (msl_info->draw_info == (DrawInfo **) NULL) ||
      (msl_info->attributes == (Image **) NULL) ||
      (msl_info->image == (Image **) NULL))
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  msl_info->image_info[n]=CloneImageInfo(msl_info->image_info[n-1]);
  msl_info->draw_info[n]=CloneDrawInfo(msl_info->image_info[n-1],
    msl_info->draw_info[n-1]);
  if (image == (Image *) NULL)
    msl_info->attributes[n]=AcquireImage(msl_info->image_info[n]);
  else
    msl_info->attributes[n]=CloneImage(image,0,0,MagickTrue,&image->exception);
  msl_info->image[n]=(Image *) image;
  if ((msl_info->image_info[n] == (ImageInfo *) NULL) ||
      (msl_info->attributes[n] == (Image *) NULL))
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  if (msl_info->number_groups != 0)
    msl_info->group_info[msl_info->number_groups-1].numImages++;
}