WandExport MagickBooleanType MagickCommandGenesis(ImageInfo *image_info,
  MagickCommand command,int argc,char **argv,char **metadata,
  ExceptionInfo *exception)
{
  char
    client_name[MaxTextExtent],
    *option;

  double
    duration,
    serial;

  MagickBooleanType
    concurrent,
    regard_warnings,
    status;

  register ssize_t
    i;

  size_t
    iterations,
    number_threads;

  ssize_t
    n;

  (void) setlocale(LC_ALL,"");
  (void) setlocale(LC_NUMERIC,"C");
  GetPathComponent(argv[0],TailPath,client_name);
  (void) SetClientName(client_name);
  concurrent=MagickFalse;
  duration=(-1.0);
  iterations=1;
  status=MagickTrue;
  regard_warnings=MagickFalse;
  for (i=1; i < (ssize_t) (argc-1); i++)
  {
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      continue;
    if (LocaleCompare("-bench",option) == 0)
      iterations=StringToUnsignedLong(argv[++i]);
    if (LocaleCompare("-concurrent",option) == 0)
      concurrent=MagickTrue;
    if (LocaleCompare("-debug",option) == 0)
      (void) SetLogEventMask(argv[++i]);
    if (LocaleCompare("-distribute-cache",option) == 0)
      {
        DistributePixelCacheServer(StringToInteger(argv[++i]),exception);
        exit(0);
      }
    if (LocaleCompare("-duration",option) == 0)
      duration=StringToDouble(argv[++i],(char **) NULL);
    if (LocaleCompare("-regard-warnings",option) == 0)
      regard_warnings=MagickTrue;
  }
  if (iterations == 1)
    {
      char
        *text;

      text=(char *) NULL;
      status=command(image_info,argc,argv,&text,exception);
      if (exception->severity != UndefinedException)
        {
          if ((exception->severity > ErrorException) ||
              (regard_warnings != MagickFalse))
            status=MagickFalse;
          CatchException(exception);
        }
      if (text != (char *) NULL)
        {
          if (metadata != (char **) NULL)
            (void) ConcatenateString(&(*metadata),text);
          text=DestroyString(text);
        }
      return(status);
    }
  number_threads=GetOpenMPMaximumThreads();
  serial=0.0;
  for (n=1; n <= (ssize_t) number_threads; n++)
  {
    double
      e,
      parallel,
      user_time;

    TimerInfo
      *timer;

    (void) SetMagickResourceLimit(ThreadResource,(MagickSizeType) n);
    timer=AcquireTimerInfo();
    if (concurrent == MagickFalse)
      {
        for (i=0; i < (ssize_t) iterations; i++)
        {
          char
            *text;

          text=(char *) NULL;
          if (status == MagickFalse)
            continue;
          if (duration > 0)
            {
              if (GetElapsedTime(timer) > duration)
                continue;
              (void) ContinueTimer(timer);
            }
          status=command(image_info,argc,argv,&text,exception);
          if (exception->severity != UndefinedException)
            {
              if ((exception->severity > ErrorException) ||
                  (regard_warnings != MagickFalse))
                status=MagickFalse;
              CatchException(exception);
            }
          if (text != (char *) NULL)
            {
              if (metadata != (char **) NULL)
                (void) ConcatenateString(&(*metadata),text);
              text=DestroyString(text);
            }
          }
      }
    else
      {
        SetOpenMPNested(1);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
        # pragma omp parallel for shared(status)
#endif
        for (i=0; i < (ssize_t) iterations; i++)
        {
          char
            *text;

          text=(char *) NULL;
          if (status == MagickFalse)
            continue;
          if (duration > 0)
            {
              if (GetElapsedTime(timer) > duration)
                continue;
              (void) ContinueTimer(timer);
            }
          status=command(image_info,argc,argv,&text,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
          # pragma omp critical (MagickCore_MagickCommandGenesis)
#endif
          {
            if (exception->severity != UndefinedException)
              {
                if ((exception->severity > ErrorException) ||
                    (regard_warnings != MagickFalse))
                  status=MagickFalse;
                CatchException(exception);
              }
            if (text != (char *) NULL)
              {
                if (metadata != (char **) NULL)
                  (void) ConcatenateString(&(*metadata),text);
                text=DestroyString(text);
              }
          }
        }
      }
    user_time=GetUserTime(timer);
    parallel=GetElapsedTime(timer);
    e=1.0;
    if (n == 1)
      serial=parallel;
    else
      e=((1.0/(1.0/((serial/(serial+parallel))+(1.0-(serial/(serial+parallel)))/
        (double) n)))-(1.0/(double) n))/(1.0-1.0/(double) n);
    (void) FormatLocaleFile(stderr,
      "  Performance[%.20g]: %.20gi %0.3fips %0.6fe %0.6fu %lu:%02lu.%03lu\n",
      (double) n,(double) iterations,(double) iterations/parallel,e,user_time,
      (unsigned long) (parallel/60.0),(unsigned long) floor(fmod(parallel,
      60.0)),(unsigned long) (1000.0*(parallel-floor(parallel))+0.5));
    timer=DestroyTimerInfo(timer);
  }
  return(status);
}