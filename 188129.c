static int DelegateInfoCompare(const void *x,const void *y)
{
  const DelegateInfo
    **p,
    **q;

  int
    cmp;

  p=(const DelegateInfo **) x,
  q=(const DelegateInfo **) y;
  cmp=LocaleCompare((*p)->path,(*q)->path);
  if (cmp == 0)
    {
      if ((*p)->decode == (char *) NULL)
        if (((*p)->encode != (char *) NULL) &&
            ((*q)->encode != (char *) NULL))
          return(strcmp((*p)->encode,(*q)->encode));
      if (((*p)->decode != (char *) NULL) &&
          ((*q)->decode != (char *) NULL))
        return(strcmp((*p)->decode,(*q)->decode));
    }
  return(cmp);
}