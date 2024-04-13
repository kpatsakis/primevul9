MagickExport MagickBooleanType GetMultilineTypeMetrics(Image *image,
  const DrawInfo *draw_info,TypeMetric *metrics)
{
  char
    **textlist;

  DrawInfo
    *annotate_info;

  MagickBooleanType
    status;

  register ssize_t
    i;

  TypeMetric
    extent;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->text != (char *) NULL);
  assert(draw_info->signature == MagickCoreSignature);
  if (*draw_info->text == '\0')
    return(MagickFalse);
  annotate_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  annotate_info->text=DestroyString(annotate_info->text);
  /*
    Convert newlines to multiple lines of text.
  */
  textlist=StringToList(draw_info->text);
  if (textlist == (char **) NULL)
    return(MagickFalse);
  annotate_info->render=MagickFalse;
  annotate_info->direction=UndefinedDirection;
  (void) memset(metrics,0,sizeof(*metrics));
  (void) memset(&extent,0,sizeof(extent));
  /*
    Find the widest of the text lines.
  */
  annotate_info->text=textlist[0];
  status=GetTypeMetrics(image,annotate_info,&extent);
  *metrics=extent;
  for (i=1; textlist[i] != (char *) NULL; i++)
  {
    annotate_info->text=textlist[i];
    status=GetTypeMetrics(image,annotate_info,&extent);
    if (extent.width > metrics->width)
      *metrics=extent;
  }
  metrics->height=(double) (i*(size_t) (metrics->ascent-metrics->descent+0.5)+
    (i-1)*draw_info->interline_spacing);
  /*
    Relinquish resources.
  */
  annotate_info->text=(char *) NULL;
  annotate_info=DestroyDrawInfo(annotate_info);
  for (i=0; textlist[i] != (char *) NULL; i++)
    textlist[i]=DestroyString(textlist[i]);
  textlist=(char **) RelinquishMagickMemory(textlist);
  return(status);
}