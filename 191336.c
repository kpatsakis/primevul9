MagickExport ssize_t FormatMagickCaption(Image *image,DrawInfo *draw_info,
  const MagickBooleanType split,TypeMetric *metrics,char **caption)
{
  MagickBooleanType
    status;

  register char
    *p,
    *q,
    *s;

  register ssize_t
    i;

  size_t
    width;

  ssize_t
    n;

  q=draw_info->text;
  s=(char *) NULL;
  for (p=(*caption); GetUTFCode(p) != 0; p+=GetUTFOctets(p))
  {
    if (IsUTFSpace(GetUTFCode(p)) != MagickFalse)
      s=p;
    if (GetUTFCode(p) == '\n')
      {
        q=draw_info->text;
        continue;
      }
    for (i=0; i < (ssize_t) GetUTFOctets(p); i++)
      *q++=(*(p+i));
    *q='\0';
    status=GetTypeMetrics(image,draw_info,metrics);
    if (status == MagickFalse)
      break;
    width=(size_t) floor(metrics->width+draw_info->stroke_width+0.5);
    if (width <= image->columns)
      continue;
    if (s != (char *) NULL)
      {
        *s='\n';
        p=s;
      }
    else
      if (split != MagickFalse)
        {
          /*
            No convenient line breaks-- insert newline.
          */
          n=p-(*caption);
          if ((n > 0) && ((*caption)[n-1] != '\n'))
            {
              char
                *target;

              target=AcquireString(*caption);
              CopyMagickString(target,*caption,n+1);
              ConcatenateMagickString(target,"\n",strlen(*caption)+1);
              ConcatenateMagickString(target,p,strlen(*caption)+2);
              (void) DestroyString(*caption);
              *caption=target;
              p=(*caption)+n;
            }
        }
    q=draw_info->text;
    s=(char *) NULL;
  }
  n=0;
  for (p=(*caption); GetUTFCode(p) != 0; p+=GetUTFOctets(p))
    if (GetUTFCode(p) == '\n')
      n++;
  return(n);
}