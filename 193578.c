MagickExport Image *CannyEdgeImage(const Image *image,const double radius,
  const double sigma,const double lower_percent,const double upper_percent,
  ExceptionInfo *exception)
{
#define CannyEdgeImageTag  "CannyEdge/Image"

  CacheView
    *edge_view;

  CannyInfo
    element;

  char
    geometry[MagickPathExtent];

  double
    lower_threshold,
    max,
    min,
    upper_threshold;

  Image
    *edge_image;

  KernelInfo
    *kernel_info;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  MatrixInfo
    *canny_cache;

  ssize_t
    y;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  /*
    Filter out noise.
  */
  (void) FormatLocaleString(geometry,MagickPathExtent,
    "blur:%.20gx%.20g;blur:%.20gx%.20g+90",radius,sigma,radius,sigma);
  kernel_info=AcquireKernelInfo(geometry,exception);
  if (kernel_info == (KernelInfo *) NULL)
    ThrowImageException(ResourceLimitError,"MemoryAllocationFailed");
  edge_image=MorphologyImage(image,ConvolveMorphology,1,kernel_info,exception);
  kernel_info=DestroyKernelInfo(kernel_info);
  if (edge_image == (Image *) NULL)
    return((Image *) NULL);
  if (TransformImageColorspace(edge_image,GRAYColorspace,exception) == MagickFalse)
    {
      edge_image=DestroyImage(edge_image);
      return((Image *) NULL);
    }
  (void) SetImageAlphaChannel(edge_image,OffAlphaChannel,exception);
  /*
    Find the intensity gradient of the image.
  */
  canny_cache=AcquireMatrixInfo(edge_image->columns,edge_image->rows,
    sizeof(CannyInfo),exception);
  if (canny_cache == (MatrixInfo *) NULL)
    {
      edge_image=DestroyImage(edge_image);
      return((Image *) NULL);
    }
  status=MagickTrue;
  edge_view=AcquireVirtualCacheView(edge_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(edge_image,edge_image,edge_image->rows,1)
#endif
  for (y=0; y < (ssize_t) edge_image->rows; y++)
  {
    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(edge_view,0,y,edge_image->columns+1,2,
      exception);
    if (p == (const Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) edge_image->columns; x++)
    {
      CannyInfo
        pixel;

      double
        dx,
        dy;

      register const Quantum
        *magick_restrict kernel_pixels;

      ssize_t
        v;

      static double
        Gx[2][2] =
        {
          { -1.0,  +1.0 },
          { -1.0,  +1.0 }
        },
        Gy[2][2] =
        {
          { +1.0, +1.0 },
          { -1.0, -1.0 }
        };

      (void) memset(&pixel,0,sizeof(pixel));
      dx=0.0;
      dy=0.0;
      kernel_pixels=p;
      for (v=0; v < 2; v++)
      {
        ssize_t
          u;

        for (u=0; u < 2; u++)
        {
          double
            intensity;

          intensity=GetPixelIntensity(edge_image,kernel_pixels+u);
          dx+=0.5*Gx[v][u]*intensity;
          dy+=0.5*Gy[v][u]*intensity;
        }
        kernel_pixels+=edge_image->columns+1;
      }
      pixel.magnitude=hypot(dx,dy);
      pixel.orientation=0;
      if (fabs(dx) > MagickEpsilon)
        {
          double
            slope;

          slope=dy/dx;
          if (slope < 0.0)
            {
              if (slope < -2.41421356237)
                pixel.orientation=0;
              else
                if (slope < -0.414213562373)
                  pixel.orientation=1;
                else
                  pixel.orientation=2;
            }
          else
            {
              if (slope > 2.41421356237)
                pixel.orientation=0;
              else
                if (slope > 0.414213562373)
                  pixel.orientation=3;
                else
                  pixel.orientation=2;
            }
        }
      if (SetMatrixElement(canny_cache,x,y,&pixel) == MagickFalse)
        continue;
      p+=GetPixelChannels(edge_image);
    }
  }
  edge_view=DestroyCacheView(edge_view);
  /*
    Non-maxima suppression, remove pixels that are not considered to be part
    of an edge.
  */
  progress=0;
  (void) GetMatrixElement(canny_cache,0,0,&element);
  max=element.intensity;
  min=element.intensity;
  edge_view=AcquireAuthenticCacheView(edge_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(edge_image,edge_image,edge_image->rows,1)
#endif
  for (y=0; y < (ssize_t) edge_image->rows; y++)
  {
    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(edge_view,0,y,edge_image->columns,1,
      exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) edge_image->columns; x++)
    {
      CannyInfo
        alpha_pixel,
        beta_pixel,
        pixel;

      (void) GetMatrixElement(canny_cache,x,y,&pixel);
      switch (pixel.orientation)
      {
        case 0:
        default:
        {
          /*
            0 degrees, north and south.
          */
          (void) GetMatrixElement(canny_cache,x,y-1,&alpha_pixel);
          (void) GetMatrixElement(canny_cache,x,y+1,&beta_pixel);
          break;
        }
        case 1:
        {
          /*
            45 degrees, northwest and southeast.
          */
          (void) GetMatrixElement(canny_cache,x-1,y-1,&alpha_pixel);
          (void) GetMatrixElement(canny_cache,x+1,y+1,&beta_pixel);
          break;
        }
        case 2:
        {
          /*
            90 degrees, east and west.
          */
          (void) GetMatrixElement(canny_cache,x-1,y,&alpha_pixel);
          (void) GetMatrixElement(canny_cache,x+1,y,&beta_pixel);
          break;
        }
        case 3:
        {
          /*
            135 degrees, northeast and southwest.
          */
          (void) GetMatrixElement(canny_cache,x+1,y-1,&beta_pixel);
          (void) GetMatrixElement(canny_cache,x-1,y+1,&alpha_pixel);
          break;
        }
      }
      pixel.intensity=pixel.magnitude;
      if ((pixel.magnitude < alpha_pixel.magnitude) ||
          (pixel.magnitude < beta_pixel.magnitude))
        pixel.intensity=0;
      (void) SetMatrixElement(canny_cache,x,y,&pixel);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      #pragma omp critical (MagickCore_CannyEdgeImage)
#endif
      {
        if (pixel.intensity < min)
          min=pixel.intensity;
        if (pixel.intensity > max)
          max=pixel.intensity;
      }
      *q=0;
      q+=GetPixelChannels(edge_image);
    }
    if (SyncCacheViewAuthenticPixels(edge_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  edge_view=DestroyCacheView(edge_view);
  /*
    Estimate hysteresis threshold.
  */
  lower_threshold=lower_percent*(max-min)+min;
  upper_threshold=upper_percent*(max-min)+min;
  /*
    Hysteresis threshold.
  */
  edge_view=AcquireAuthenticCacheView(edge_image,exception);
  for (y=0; y < (ssize_t) edge_image->rows; y++)
  {
    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    for (x=0; x < (ssize_t) edge_image->columns; x++)
    {
      CannyInfo
        pixel;

      register const Quantum
        *magick_restrict p;

      /*
        Edge if pixel gradient higher than upper threshold.
      */
      p=GetCacheViewVirtualPixels(edge_view,x,y,1,1,exception);
      if (p == (const Quantum *) NULL)
        continue;
      status=GetMatrixElement(canny_cache,x,y,&pixel);
      if (status == MagickFalse)
        continue;
      if ((GetPixelIntensity(edge_image,p) == 0.0) &&
          (pixel.intensity >= upper_threshold))
        status=TraceEdges(edge_image,edge_view,canny_cache,x,y,lower_threshold,
          exception);
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
  edge_view=DestroyCacheView(edge_view);
  /*
    Free resources.
  */
  canny_cache=DestroyMatrixInfo(canny_cache);
  return(edge_image);
}