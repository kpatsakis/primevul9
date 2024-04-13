WandExport MagickBooleanType MogrifyImages(ImageInfo *image_info,
  const MagickBooleanType post,const int argc,const char **argv,
  Image **images,ExceptionInfo *exception)
{
#define MogrifyImageTag  "Mogrify/Image"

  MagickStatusType
    status;

  MagickBooleanType
    proceed;

  size_t
    n;

  register ssize_t
    i;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (images == (Image **) NULL)
    return(MogrifyImage(image_info,argc,argv,images,exception));
  assert((*images)->previous == (Image *) NULL);
  assert((*images)->signature == MagickCoreSignature);
  if ((*images)->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      (*images)->filename);
  if ((argc <= 0) || (*argv == (char *) NULL))
    return(MagickTrue);
  (void) SetImageInfoProgressMonitor(image_info,(MagickProgressMonitor) NULL,
    (void *) NULL);
  status=MagickTrue;
#if 0
  (void) FormatLocaleFile(stderr, "mogrify start %s %d (%s)\n",argv[0],argc,
    post?"post":"pre");
#endif
  /*
    Pre-process multi-image sequence operators
  */
  if (post == MagickFalse)
    status&=MogrifyImageList(image_info,argc,argv,images,exception);
  /*
    For each image, process simple single image operators
  */
  i=0;
  n=GetImageListLength(*images);
  for ( ; ; )
  {
#if 0
  (void) FormatLocaleFile(stderr,"mogrify %ld of %ld\n",(long)
    GetImageIndexInList(*images),(long)GetImageListLength(*images));
#endif
    status&=MogrifyImage(image_info,argc,argv,images,exception);
    proceed=SetImageProgress(*images,MogrifyImageTag,(MagickOffsetType) i, n);
    if (proceed == MagickFalse)
      break;
    if ( (*images)->next == (Image *) NULL )
      break;
    *images=(*images)->next;
    i++;
  }
  assert( *images != (Image *) NULL );
#if 0
  (void) FormatLocaleFile(stderr,"mogrify end %ld of %ld\n",(long)
    GetImageIndexInList(*images),(long)GetImageListLength(*images));
#endif
  /*
    Post-process, multi-image sequence operators
  */
  *images=GetFirstImageInList(*images);
  if (post != MagickFalse)
    status&=MogrifyImageList(image_info,argc,argv,images,exception);
  return(status != 0 ? MagickTrue : MagickFalse);
}