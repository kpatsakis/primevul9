WandExport MagickBooleanType DrawSetStrokeDashArray(DrawingWand *wand,
  const size_t number_elements,const double *dasharray)
{
  MagickBooleanType
    update;

  register const double
    *p;

  register double
    *q;

  register ssize_t
    i;

  size_t
    n_new,
    n_old;

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  n_new=number_elements;
  if (dasharray == (const double *) NULL)
    n_new=0;
  n_old=0;
  update=MagickFalse;
  q=CurrentContext->dash_pattern;
  if (q != (const double *) NULL)
    while (fabs(*q++) < MagickEpsilon)
      n_old++;
  if ((n_old == 0) && (n_new == 0))
    update=MagickFalse;
  else
    if (n_old != n_new)
      update=MagickTrue;
    else
      if ((CurrentContext->dash_pattern != (double *) NULL) &&
          (dasharray != (double *) NULL))
        {
          p=dasharray;
          q=CurrentContext->dash_pattern;
          for (i=0; i < (ssize_t) n_new; i++)
          {
            if (fabs((*p)-(*q)) >= MagickEpsilon)
              {
                update=MagickTrue;
                break;
              }
            p++;
            q++;
          }
        }
  if ((wand->filter_off != MagickFalse) || (update != MagickFalse))
    {
      if (CurrentContext->dash_pattern != (double *) NULL)
        CurrentContext->dash_pattern=(double *)
          RelinquishMagickMemory(CurrentContext->dash_pattern);
      if (n_new != 0)
        {
          CurrentContext->dash_pattern=(double *) AcquireQuantumMemory((size_t)
            n_new+1UL,sizeof(*CurrentContext->dash_pattern));
          if (CurrentContext->dash_pattern == (double *) NULL)
            {
              ThrowDrawException(ResourceLimitError,"MemoryAllocationFailed",
                wand->name);
              return(MagickFalse);
            }
          for (i=0; i < (ssize_t) n_new; i++)
          {
            CurrentContext->dash_pattern[i]=0.0;
            if (dasharray != (double *) NULL)
              CurrentContext->dash_pattern[i]=dasharray[i];
          }
          CurrentContext->dash_pattern[n_new]=0.0;
        }
      (void) MVGPrintf(wand,"stroke-dasharray ");
      if (n_new == 0)
        (void) MVGPrintf(wand,"none\n");
      else
        if (dasharray != (double *) NULL)
          {
            for (i=0; i < (ssize_t) n_new; i++)
            {
              if (i != 0)
                (void) MVGPrintf(wand,",");
              (void) MVGPrintf(wand,"%.20g",dasharray[i]);
            }
            (void) MVGPrintf(wand,"\n");
          }
    }
  return(MagickTrue);
}