MagickExport Image *HoughLineImage(const Image *image,const size_t width,
  const size_t height,const size_t threshold,ExceptionInfo *exception)
{
#define HoughLineImageTag  "HoughLine/Image"

  CacheView
    *image_view;

  char
    message[MagickPathExtent],
    path[MagickPathExtent];

  const char
    *artifact;

  double
    hough_height;

  Image
    *lines_image = NULL;

  ImageInfo
    *image_info;

  int
    file;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  MatrixInfo
    *accumulator;

  PointInfo
    center;

  register ssize_t
    y;

  size_t
    accumulator_height,
    accumulator_width,
    line_count;

  /*
    Create the accumulator.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  accumulator_width=180;
  hough_height=((sqrt(2.0)*(double) (image->rows > image->columns ?
    image->rows : image->columns))/2.0);
  accumulator_height=(size_t) (2.0*hough_height);
  accumulator=AcquireMatrixInfo(accumulator_width,accumulator_height,
    sizeof(double),exception);
  if (accumulator == (MatrixInfo *) NULL)
    ThrowImageException(ResourceLimitError,"MemoryAllocationFailed");
  if (NullMatrix(accumulator) == MagickFalse)
    {
      accumulator=DestroyMatrixInfo(accumulator);
      ThrowImageException(ResourceLimitError,"MemoryAllocationFailed");
    }
  /*
    Populate the accumulator.
  */
  status=MagickTrue;
  progress=0;
  center.x=(double) image->columns/2.0;
  center.y=(double) image->rows/2.0;
  image_view=AcquireVirtualCacheView(image,exception);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (GetPixelIntensity(image,p) > (QuantumRange/2.0))
        {
          register ssize_t
            i;

          for (i=0; i < 180; i++)
          {
            double
              count,
              radius;

            radius=(((double) x-center.x)*cos(DegreesToRadians((double) i)))+
              (((double) y-center.y)*sin(DegreesToRadians((double) i)));
            (void) GetMatrixElement(accumulator,i,(ssize_t)
              MagickRound(radius+hough_height),&count);
            count++;
            (void) SetMatrixElement(accumulator,i,(ssize_t)
              MagickRound(radius+hough_height),&count);
          }
        }
      p+=GetPixelChannels(image);
    }
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,CannyEdgeImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    {
      accumulator=DestroyMatrixInfo(accumulator);
      return((Image *) NULL);
    }
  /*
    Generate line segments from accumulator.
  */
  file=AcquireUniqueFileResource(path);
  if (file == -1)
    {
      accumulator=DestroyMatrixInfo(accumulator);
      return((Image *) NULL);
    }
  (void) FormatLocaleString(message,MagickPathExtent,
    "# Hough line transform: %.20gx%.20g%+.20g\n",(double) width,
    (double) height,(double) threshold);
  if (write(file,message,strlen(message)) != (ssize_t) strlen(message))
    status=MagickFalse;
  (void) FormatLocaleString(message,MagickPathExtent,
    "viewbox 0 0 %.20g %.20g\n",(double) image->columns,(double) image->rows);
  if (write(file,message,strlen(message)) != (ssize_t) strlen(message))
    status=MagickFalse;
  (void) FormatLocaleString(message,MagickPathExtent,
    "# x1,y1 x2,y2 # count angle distance\n");
  if (write(file,message,strlen(message)) != (ssize_t) strlen(message))
    status=MagickFalse;
  line_count=image->columns > image->rows ? image->columns/4 : image->rows/4;
  if (threshold != 0)
    line_count=threshold;
  for (y=0; y < (ssize_t) accumulator_height; y++)
  {
    register ssize_t
      x;

    for (x=0; x < (ssize_t) accumulator_width; x++)
    {
      double
        count;

      (void) GetMatrixElement(accumulator,x,y,&count);
      if (count >= (double) line_count)
        {
          double
            maxima;

          SegmentInfo
            line;

          ssize_t
            v;

          /*
            Is point a local maxima?
          */
          maxima=count;
          for (v=(-((ssize_t) height/2)); v <= (((ssize_t) height/2)); v++)
          {
            ssize_t
              u;

            for (u=(-((ssize_t) width/2)); u <= (((ssize_t) width/2)); u++)
            {
              if ((u != 0) || (v !=0))
                {
                  (void) GetMatrixElement(accumulator,x+u,y+v,&count);
                  if (count > maxima)
                    {
                      maxima=count;
                      break;
                    }
                }
            }
            if (u < (ssize_t) (width/2))
              break;
          }
          (void) GetMatrixElement(accumulator,x,y,&count);
          if (maxima > count)
            continue;
          if ((x >= 45) && (x <= 135))
            {
              /*
                y = (r-x cos(t))/sin(t)
              */
              line.x1=0.0;
              line.y1=((double) (y-(accumulator_height/2.0))-((line.x1-
                (image->columns/2.0))*cos(DegreesToRadians((double) x))))/
                sin(DegreesToRadians((double) x))+(image->rows/2.0);
              line.x2=(double) image->columns;
              line.y2=((double) (y-(accumulator_height/2.0))-((line.x2-
                (image->columns/2.0))*cos(DegreesToRadians((double) x))))/
                sin(DegreesToRadians((double) x))+(image->rows/2.0);
            }
          else
            {
              /*
                x = (r-y cos(t))/sin(t)
              */
              line.y1=0.0;
              line.x1=((double) (y-(accumulator_height/2.0))-((line.y1-
                (image->rows/2.0))*sin(DegreesToRadians((double) x))))/
                cos(DegreesToRadians((double) x))+(image->columns/2.0);
              line.y2=(double) image->rows;
              line.x2=((double) (y-(accumulator_height/2.0))-((line.y2-
                (image->rows/2.0))*sin(DegreesToRadians((double) x))))/
                cos(DegreesToRadians((double) x))+(image->columns/2.0);
            }
          (void) FormatLocaleString(message,MagickPathExtent,
            "line %g,%g %g,%g  # %g %g %g\n",line.x1,line.y1,line.x2,line.y2,
            maxima,(double) x,(double) y);
          if (write(file,message,strlen(message)) != (ssize_t) strlen(message))
            status=MagickFalse;
        }
    }
  }
  (void) close(file);
  /*
    Render lines to image canvas.
  */
  image_info=AcquireImageInfo();
  image_info->background_color=image->background_color;
  (void) FormatLocaleString(image_info->filename,MagickPathExtent,"%s",path);
  artifact=GetImageArtifact(image,"background");
  if (artifact != (const char *) NULL)
    (void) SetImageOption(image_info,"background",artifact);
  artifact=GetImageArtifact(image,"fill");
  if (artifact != (const char *) NULL)
    (void) SetImageOption(image_info,"fill",artifact);
  artifact=GetImageArtifact(image,"stroke");
  if (artifact != (const char *) NULL)
    (void) SetImageOption(image_info,"stroke",artifact);
  artifact=GetImageArtifact(image,"strokewidth");
  if (artifact != (const char *) NULL)
    (void) SetImageOption(image_info,"strokewidth",artifact);
  lines_image=RenderHoughLines(image_info,image->columns,image->rows,exception);
  artifact=GetImageArtifact(image,"hough-lines:accumulator");
  if ((lines_image != (Image *) NULL) &&
      (IsStringTrue(artifact) != MagickFalse))
    {
      Image
        *accumulator_image;

      accumulator_image=MatrixToImage(accumulator,exception);
      if (accumulator_image != (Image *) NULL)
        AppendImageToList(&lines_image,accumulator_image);
    }
  /*
    Free resources.
  */
  accumulator=DestroyMatrixInfo(accumulator);
  image_info=DestroyImageInfo(image_info);
  (void) RelinquishUniqueFileResource(path);
  return(GetFirstImageInList(lines_image));
}