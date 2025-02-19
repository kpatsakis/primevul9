MagickExport ChannelFeatures *GetImageFeatures(const Image *image,
  const size_t distance,ExceptionInfo *exception)
{
  typedef struct _ChannelStatistics
  {
    PixelInfo
      direction[4];  /* horizontal, vertical, left and right diagonals */
  } ChannelStatistics;

  CacheView
    *image_view;

  ChannelFeatures
    *channel_features;

  ChannelStatistics
    **cooccurrence,
    correlation,
    *density_x,
    *density_xy,
    *density_y,
    entropy_x,
    entropy_xy,
    entropy_xy1,
    entropy_xy2,
    entropy_y,
    mean,
    **Q,
    *sum,
    sum_squares,
    variance;

  PixelPacket
    gray,
    *grays;

  MagickBooleanType
    status;

  register ssize_t
    i,
    r;

  size_t
    length;

  unsigned int
    number_grays;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if ((image->columns < (distance+1)) || (image->rows < (distance+1)))
    return((ChannelFeatures *) NULL);
  length=MaxPixelChannels+1UL;
  channel_features=(ChannelFeatures *) AcquireQuantumMemory(length,
    sizeof(*channel_features));
  if (channel_features == (ChannelFeatures *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  (void) memset(channel_features,0,length*
    sizeof(*channel_features));
  /*
    Form grays.
  */
  grays=(PixelPacket *) AcquireQuantumMemory(MaxMap+1UL,sizeof(*grays));
  if (grays == (PixelPacket *) NULL)
    {
      channel_features=(ChannelFeatures *) RelinquishMagickMemory(
        channel_features);
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      return(channel_features);
    }
  for (i=0; i <= (ssize_t) MaxMap; i++)
  {
    grays[i].red=(~0U);
    grays[i].green=(~0U);
    grays[i].blue=(~0U);
    grays[i].alpha=(~0U);
    grays[i].black=(~0U);
  }
  status=MagickTrue;
  image_view=AcquireVirtualCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (r=0; r < (ssize_t) image->rows; r++)
  {
    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,r,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      grays[ScaleQuantumToMap(GetPixelRed(image,p))].red=
        ScaleQuantumToMap(GetPixelRed(image,p));
      grays[ScaleQuantumToMap(GetPixelGreen(image,p))].green=
        ScaleQuantumToMap(GetPixelGreen(image,p));
      grays[ScaleQuantumToMap(GetPixelBlue(image,p))].blue=
        ScaleQuantumToMap(GetPixelBlue(image,p));
      if (image->colorspace == CMYKColorspace)
        grays[ScaleQuantumToMap(GetPixelBlack(image,p))].black=
          ScaleQuantumToMap(GetPixelBlack(image,p));
      if (image->alpha_trait != UndefinedPixelTrait)
        grays[ScaleQuantumToMap(GetPixelAlpha(image,p))].alpha=
          ScaleQuantumToMap(GetPixelAlpha(image,p));
      p+=GetPixelChannels(image);
    }
  }
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    {
      grays=(PixelPacket *) RelinquishMagickMemory(grays);
      channel_features=(ChannelFeatures *) RelinquishMagickMemory(
        channel_features);
      return(channel_features);
    }
  (void) memset(&gray,0,sizeof(gray));
  for (i=0; i <= (ssize_t) MaxMap; i++)
  {
    if (grays[i].red != ~0U)
      grays[gray.red++].red=grays[i].red;
    if (grays[i].green != ~0U)
      grays[gray.green++].green=grays[i].green;
    if (grays[i].blue != ~0U)
      grays[gray.blue++].blue=grays[i].blue;
    if (image->colorspace == CMYKColorspace)
      if (grays[i].black != ~0U)
        grays[gray.black++].black=grays[i].black;
    if (image->alpha_trait != UndefinedPixelTrait)
      if (grays[i].alpha != ~0U)
        grays[gray.alpha++].alpha=grays[i].alpha;
  }
  /*
    Allocate spatial dependence matrix.
  */
  number_grays=gray.red;
  if (gray.green > number_grays)
    number_grays=gray.green;
  if (gray.blue > number_grays)
    number_grays=gray.blue;
  if (image->colorspace == CMYKColorspace)
    if (gray.black > number_grays)
      number_grays=gray.black;
  if (image->alpha_trait != UndefinedPixelTrait)
    if (gray.alpha > number_grays)
      number_grays=gray.alpha;
  cooccurrence=(ChannelStatistics **) AcquireQuantumMemory(number_grays,
    sizeof(*cooccurrence));
  density_x=(ChannelStatistics *) AcquireQuantumMemory(2*(number_grays+1),
    sizeof(*density_x));
  density_xy=(ChannelStatistics *) AcquireQuantumMemory(2*(number_grays+1),
    sizeof(*density_xy));
  density_y=(ChannelStatistics *) AcquireQuantumMemory(2*(number_grays+1),
    sizeof(*density_y));
  Q=(ChannelStatistics **) AcquireQuantumMemory(number_grays,sizeof(*Q));
  sum=(ChannelStatistics *) AcquireQuantumMemory(number_grays,sizeof(*sum));
  if ((cooccurrence == (ChannelStatistics **) NULL) ||
      (density_x == (ChannelStatistics *) NULL) ||
      (density_xy == (ChannelStatistics *) NULL) ||
      (density_y == (ChannelStatistics *) NULL) ||
      (Q == (ChannelStatistics **) NULL) ||
      (sum == (ChannelStatistics *) NULL))
    {
      if (Q != (ChannelStatistics **) NULL)
        {
          for (i=0; i < (ssize_t) number_grays; i++)
            Q[i]=(ChannelStatistics *) RelinquishMagickMemory(Q[i]);
          Q=(ChannelStatistics **) RelinquishMagickMemory(Q);
        }
      if (sum != (ChannelStatistics *) NULL)
        sum=(ChannelStatistics *) RelinquishMagickMemory(sum);
      if (density_y != (ChannelStatistics *) NULL)
        density_y=(ChannelStatistics *) RelinquishMagickMemory(density_y);
      if (density_xy != (ChannelStatistics *) NULL)
        density_xy=(ChannelStatistics *) RelinquishMagickMemory(density_xy);
      if (density_x != (ChannelStatistics *) NULL)
        density_x=(ChannelStatistics *) RelinquishMagickMemory(density_x);
      if (cooccurrence != (ChannelStatistics **) NULL)
        {
          for (i=0; i < (ssize_t) number_grays; i++)
            cooccurrence[i]=(ChannelStatistics *)
              RelinquishMagickMemory(cooccurrence[i]);
          cooccurrence=(ChannelStatistics **) RelinquishMagickMemory(
            cooccurrence);
        }
      grays=(PixelPacket *) RelinquishMagickMemory(grays);
      channel_features=(ChannelFeatures *) RelinquishMagickMemory(
        channel_features);
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      return(channel_features);
    }
  (void) memset(&correlation,0,sizeof(correlation));
  (void) memset(density_x,0,2*(number_grays+1)*sizeof(*density_x));
  (void) memset(density_xy,0,2*(number_grays+1)*sizeof(*density_xy));
  (void) memset(density_y,0,2*(number_grays+1)*sizeof(*density_y));
  (void) memset(&mean,0,sizeof(mean));
  (void) memset(sum,0,number_grays*sizeof(*sum));
  (void) memset(&sum_squares,0,sizeof(sum_squares));
  (void) memset(density_xy,0,2*number_grays*sizeof(*density_xy));
  (void) memset(&entropy_x,0,sizeof(entropy_x));
  (void) memset(&entropy_xy,0,sizeof(entropy_xy));
  (void) memset(&entropy_xy1,0,sizeof(entropy_xy1));
  (void) memset(&entropy_xy2,0,sizeof(entropy_xy2));
  (void) memset(&entropy_y,0,sizeof(entropy_y));
  (void) memset(&variance,0,sizeof(variance));
  for (i=0; i < (ssize_t) number_grays; i++)
  {
    cooccurrence[i]=(ChannelStatistics *) AcquireQuantumMemory(number_grays,
      sizeof(**cooccurrence));
    Q[i]=(ChannelStatistics *) AcquireQuantumMemory(number_grays,sizeof(**Q));
    if ((cooccurrence[i] == (ChannelStatistics *) NULL) ||
        (Q[i] == (ChannelStatistics *) NULL))
      break;
    (void) memset(cooccurrence[i],0,number_grays*
      sizeof(**cooccurrence));
    (void) memset(Q[i],0,number_grays*sizeof(**Q));
  }
  if (i < (ssize_t) number_grays)
    {
      for (i--; i >= 0; i--)
      {
        if (Q[i] != (ChannelStatistics *) NULL)
          Q[i]=(ChannelStatistics *) RelinquishMagickMemory(Q[i]);
        if (cooccurrence[i] != (ChannelStatistics *) NULL)
          cooccurrence[i]=(ChannelStatistics *)
            RelinquishMagickMemory(cooccurrence[i]);
      }
      Q=(ChannelStatistics **) RelinquishMagickMemory(Q);
      cooccurrence=(ChannelStatistics **) RelinquishMagickMemory(cooccurrence);
      sum=(ChannelStatistics *) RelinquishMagickMemory(sum);
      density_y=(ChannelStatistics *) RelinquishMagickMemory(density_y);
      density_xy=(ChannelStatistics *) RelinquishMagickMemory(density_xy);
      density_x=(ChannelStatistics *) RelinquishMagickMemory(density_x);
      grays=(PixelPacket *) RelinquishMagickMemory(grays);
      channel_features=(ChannelFeatures *) RelinquishMagickMemory(
        channel_features);
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      return(channel_features);
    }
  /*
    Initialize spatial dependence matrix.
  */
  status=MagickTrue;
  image_view=AcquireVirtualCacheView(image,exception);
  for (r=0; r < (ssize_t) image->rows; r++)
  {
    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    ssize_t
      offset,
      u,
      v;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,-(ssize_t) distance,r,image->columns+
      2*distance,distance+2,exception);
    if (p == (const Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    p+=distance*GetPixelChannels(image);;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      for (i=0; i < 4; i++)
      {
        switch (i)
        {
          case 0:
          default:
          {
            /*
              Horizontal adjacency.
            */
            offset=(ssize_t) distance;
            break;
          }
          case 1:
          {
            /*
              Vertical adjacency.
            */
            offset=(ssize_t) (image->columns+2*distance);
            break;
          }
          case 2:
          {
            /*
              Right diagonal adjacency.
            */
            offset=(ssize_t) ((image->columns+2*distance)-distance);
            break;
          }
          case 3:
          {
            /*
              Left diagonal adjacency.
            */
            offset=(ssize_t) ((image->columns+2*distance)+distance);
            break;
          }
        }
        u=0;
        v=0;
        while (grays[u].red != ScaleQuantumToMap(GetPixelRed(image,p)))
          u++;
        while (grays[v].red != ScaleQuantumToMap(GetPixelRed(image,p+offset*GetPixelChannels(image))))
          v++;
        cooccurrence[u][v].direction[i].red++;
        cooccurrence[v][u].direction[i].red++;
        u=0;
        v=0;
        while (grays[u].green != ScaleQuantumToMap(GetPixelGreen(image,p)))
          u++;
        while (grays[v].green != ScaleQuantumToMap(GetPixelGreen(image,p+offset*GetPixelChannels(image))))
          v++;
        cooccurrence[u][v].direction[i].green++;
        cooccurrence[v][u].direction[i].green++;
        u=0;
        v=0;
        while (grays[u].blue != ScaleQuantumToMap(GetPixelBlue(image,p)))
          u++;
        while (grays[v].blue != ScaleQuantumToMap(GetPixelBlue(image,p+offset*GetPixelChannels(image))))
          v++;
        cooccurrence[u][v].direction[i].blue++;
        cooccurrence[v][u].direction[i].blue++;
        if (image->colorspace == CMYKColorspace)
          {
            u=0;
            v=0;
            while (grays[u].black != ScaleQuantumToMap(GetPixelBlack(image,p)))
              u++;
            while (grays[v].black != ScaleQuantumToMap(GetPixelBlack(image,p+offset*GetPixelChannels(image))))
              v++;
            cooccurrence[u][v].direction[i].black++;
            cooccurrence[v][u].direction[i].black++;
          }
        if (image->alpha_trait != UndefinedPixelTrait)
          {
            u=0;
            v=0;
            while (grays[u].alpha != ScaleQuantumToMap(GetPixelAlpha(image,p)))
              u++;
            while (grays[v].alpha != ScaleQuantumToMap(GetPixelAlpha(image,p+offset*GetPixelChannels(image))))
              v++;
            cooccurrence[u][v].direction[i].alpha++;
            cooccurrence[v][u].direction[i].alpha++;
          }
      }
      p+=GetPixelChannels(image);
    }
  }
  grays=(PixelPacket *) RelinquishMagickMemory(grays);
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    {
      for (i=0; i < (ssize_t) number_grays; i++)
        cooccurrence[i]=(ChannelStatistics *)
          RelinquishMagickMemory(cooccurrence[i]);
      cooccurrence=(ChannelStatistics **) RelinquishMagickMemory(cooccurrence);
      channel_features=(ChannelFeatures *) RelinquishMagickMemory(
        channel_features);
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      return(channel_features);
    }
  /*
    Normalize spatial dependence matrix.
  */
  for (i=0; i < 4; i++)
  {
    double
      normalize;

    register ssize_t
      y;

    switch (i)
    {
      case 0:
      default:
      {
        /*
          Horizontal adjacency.
        */
        normalize=2.0*image->rows*(image->columns-distance);
        break;
      }
      case 1:
      {
        /*
          Vertical adjacency.
        */
        normalize=2.0*(image->rows-distance)*image->columns;
        break;
      }
      case 2:
      {
        /*
          Right diagonal adjacency.
        */
        normalize=2.0*(image->rows-distance)*(image->columns-distance);
        break;
      }
      case 3:
      {
        /*
          Left diagonal adjacency.
        */
        normalize=2.0*(image->rows-distance)*(image->columns-distance);
        break;
      }
    }
    normalize=PerceptibleReciprocal(normalize);
    for (y=0; y < (ssize_t) number_grays; y++)
    {
      register ssize_t
        x;

      for (x=0; x < (ssize_t) number_grays; x++)
      {
        cooccurrence[x][y].direction[i].red*=normalize;
        cooccurrence[x][y].direction[i].green*=normalize;
        cooccurrence[x][y].direction[i].blue*=normalize;
        if (image->colorspace == CMYKColorspace)
          cooccurrence[x][y].direction[i].black*=normalize;
        if (image->alpha_trait != UndefinedPixelTrait)
          cooccurrence[x][y].direction[i].alpha*=normalize;
      }
    }
  }
  /*
    Compute texture features.
  */
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(image,image,number_grays,1)
#endif
  for (i=0; i < 4; i++)
  {
    register ssize_t
      y;

    for (y=0; y < (ssize_t) number_grays; y++)
    {
      register ssize_t
        x;

      for (x=0; x < (ssize_t) number_grays; x++)
      {
        /*
          Angular second moment:  measure of homogeneity of the image.
        */
        channel_features[RedPixelChannel].angular_second_moment[i]+=
          cooccurrence[x][y].direction[i].red*
          cooccurrence[x][y].direction[i].red;
        channel_features[GreenPixelChannel].angular_second_moment[i]+=
          cooccurrence[x][y].direction[i].green*
          cooccurrence[x][y].direction[i].green;
        channel_features[BluePixelChannel].angular_second_moment[i]+=
          cooccurrence[x][y].direction[i].blue*
          cooccurrence[x][y].direction[i].blue;
        if (image->colorspace == CMYKColorspace)
          channel_features[BlackPixelChannel].angular_second_moment[i]+=
            cooccurrence[x][y].direction[i].black*
            cooccurrence[x][y].direction[i].black;
        if (image->alpha_trait != UndefinedPixelTrait)
          channel_features[AlphaPixelChannel].angular_second_moment[i]+=
            cooccurrence[x][y].direction[i].alpha*
            cooccurrence[x][y].direction[i].alpha;
        /*
          Correlation: measure of linear-dependencies in the image.
        */
        sum[y].direction[i].red+=cooccurrence[x][y].direction[i].red;
        sum[y].direction[i].green+=cooccurrence[x][y].direction[i].green;
        sum[y].direction[i].blue+=cooccurrence[x][y].direction[i].blue;
        if (image->colorspace == CMYKColorspace)
          sum[y].direction[i].black+=cooccurrence[x][y].direction[i].black;
        if (image->alpha_trait != UndefinedPixelTrait)
          sum[y].direction[i].alpha+=cooccurrence[x][y].direction[i].alpha;
        correlation.direction[i].red+=x*y*cooccurrence[x][y].direction[i].red;
        correlation.direction[i].green+=x*y*
          cooccurrence[x][y].direction[i].green;
        correlation.direction[i].blue+=x*y*
          cooccurrence[x][y].direction[i].blue;
        if (image->colorspace == CMYKColorspace)
          correlation.direction[i].black+=x*y*
            cooccurrence[x][y].direction[i].black;
        if (image->alpha_trait != UndefinedPixelTrait)
          correlation.direction[i].alpha+=x*y*
            cooccurrence[x][y].direction[i].alpha;
        /*
          Inverse Difference Moment.
        */
        channel_features[RedPixelChannel].inverse_difference_moment[i]+=
          cooccurrence[x][y].direction[i].red/((y-x)*(y-x)+1);
        channel_features[GreenPixelChannel].inverse_difference_moment[i]+=
          cooccurrence[x][y].direction[i].green/((y-x)*(y-x)+1);
        channel_features[BluePixelChannel].inverse_difference_moment[i]+=
          cooccurrence[x][y].direction[i].blue/((y-x)*(y-x)+1);
        if (image->colorspace == CMYKColorspace)
          channel_features[BlackPixelChannel].inverse_difference_moment[i]+=
            cooccurrence[x][y].direction[i].black/((y-x)*(y-x)+1);
        if (image->alpha_trait != UndefinedPixelTrait)
          channel_features[AlphaPixelChannel].inverse_difference_moment[i]+=
            cooccurrence[x][y].direction[i].alpha/((y-x)*(y-x)+1);
        /*
          Sum average.
        */
        density_xy[y+x+2].direction[i].red+=
          cooccurrence[x][y].direction[i].red;
        density_xy[y+x+2].direction[i].green+=
          cooccurrence[x][y].direction[i].green;
        density_xy[y+x+2].direction[i].blue+=
          cooccurrence[x][y].direction[i].blue;
        if (image->colorspace == CMYKColorspace)
          density_xy[y+x+2].direction[i].black+=
            cooccurrence[x][y].direction[i].black;
        if (image->alpha_trait != UndefinedPixelTrait)
          density_xy[y+x+2].direction[i].alpha+=
            cooccurrence[x][y].direction[i].alpha;
        /*
          Entropy.
        */
        channel_features[RedPixelChannel].entropy[i]-=
          cooccurrence[x][y].direction[i].red*
          MagickLog10(cooccurrence[x][y].direction[i].red);
        channel_features[GreenPixelChannel].entropy[i]-=
          cooccurrence[x][y].direction[i].green*
          MagickLog10(cooccurrence[x][y].direction[i].green);
        channel_features[BluePixelChannel].entropy[i]-=
          cooccurrence[x][y].direction[i].blue*
          MagickLog10(cooccurrence[x][y].direction[i].blue);
        if (image->colorspace == CMYKColorspace)
          channel_features[BlackPixelChannel].entropy[i]-=
            cooccurrence[x][y].direction[i].black*
            MagickLog10(cooccurrence[x][y].direction[i].black);
        if (image->alpha_trait != UndefinedPixelTrait)
          channel_features[AlphaPixelChannel].entropy[i]-=
            cooccurrence[x][y].direction[i].alpha*
            MagickLog10(cooccurrence[x][y].direction[i].alpha);
        /*
          Information Measures of Correlation.
        */
        density_x[x].direction[i].red+=cooccurrence[x][y].direction[i].red;
        density_x[x].direction[i].green+=cooccurrence[x][y].direction[i].green;
        density_x[x].direction[i].blue+=cooccurrence[x][y].direction[i].blue;
        if (image->alpha_trait != UndefinedPixelTrait)
          density_x[x].direction[i].alpha+=
            cooccurrence[x][y].direction[i].alpha;
        if (image->colorspace == CMYKColorspace)
          density_x[x].direction[i].black+=
            cooccurrence[x][y].direction[i].black;
        density_y[y].direction[i].red+=cooccurrence[x][y].direction[i].red;
        density_y[y].direction[i].green+=cooccurrence[x][y].direction[i].green;
        density_y[y].direction[i].blue+=cooccurrence[x][y].direction[i].blue;
        if (image->colorspace == CMYKColorspace)
          density_y[y].direction[i].black+=
            cooccurrence[x][y].direction[i].black;
        if (image->alpha_trait != UndefinedPixelTrait)
          density_y[y].direction[i].alpha+=
            cooccurrence[x][y].direction[i].alpha;
      }
      mean.direction[i].red+=y*sum[y].direction[i].red;
      sum_squares.direction[i].red+=y*y*sum[y].direction[i].red;
      mean.direction[i].green+=y*sum[y].direction[i].green;
      sum_squares.direction[i].green+=y*y*sum[y].direction[i].green;
      mean.direction[i].blue+=y*sum[y].direction[i].blue;
      sum_squares.direction[i].blue+=y*y*sum[y].direction[i].blue;
      if (image->colorspace == CMYKColorspace)
        {
          mean.direction[i].black+=y*sum[y].direction[i].black;
          sum_squares.direction[i].black+=y*y*sum[y].direction[i].black;
        }
      if (image->alpha_trait != UndefinedPixelTrait)
        {
          mean.direction[i].alpha+=y*sum[y].direction[i].alpha;
          sum_squares.direction[i].alpha+=y*y*sum[y].direction[i].alpha;
        }
    }
    /*
      Correlation: measure of linear-dependencies in the image.
    */
    channel_features[RedPixelChannel].correlation[i]=
      (correlation.direction[i].red-mean.direction[i].red*
      mean.direction[i].red)/(sqrt(sum_squares.direction[i].red-
      (mean.direction[i].red*mean.direction[i].red))*sqrt(
      sum_squares.direction[i].red-(mean.direction[i].red*
      mean.direction[i].red)));
    channel_features[GreenPixelChannel].correlation[i]=
      (correlation.direction[i].green-mean.direction[i].green*
      mean.direction[i].green)/(sqrt(sum_squares.direction[i].green-
      (mean.direction[i].green*mean.direction[i].green))*sqrt(
      sum_squares.direction[i].green-(mean.direction[i].green*
      mean.direction[i].green)));
    channel_features[BluePixelChannel].correlation[i]=
      (correlation.direction[i].blue-mean.direction[i].blue*
      mean.direction[i].blue)/(sqrt(sum_squares.direction[i].blue-
      (mean.direction[i].blue*mean.direction[i].blue))*sqrt(
      sum_squares.direction[i].blue-(mean.direction[i].blue*
      mean.direction[i].blue)));
    if (image->colorspace == CMYKColorspace)
      channel_features[BlackPixelChannel].correlation[i]=
        (correlation.direction[i].black-mean.direction[i].black*
        mean.direction[i].black)/(sqrt(sum_squares.direction[i].black-
        (mean.direction[i].black*mean.direction[i].black))*sqrt(
        sum_squares.direction[i].black-(mean.direction[i].black*
        mean.direction[i].black)));
    if (image->alpha_trait != UndefinedPixelTrait)
      channel_features[AlphaPixelChannel].correlation[i]=
        (correlation.direction[i].alpha-mean.direction[i].alpha*
        mean.direction[i].alpha)/(sqrt(sum_squares.direction[i].alpha-
        (mean.direction[i].alpha*mean.direction[i].alpha))*sqrt(
        sum_squares.direction[i].alpha-(mean.direction[i].alpha*
        mean.direction[i].alpha)));
  }
  /*
    Compute more texture features.
  */
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(image,image,number_grays,1)
#endif
  for (i=0; i < 4; i++)
  {
    register ssize_t
      x;

    for (x=2; x < (ssize_t) (2*number_grays); x++)
    {
      /*
        Sum average.
      */
      channel_features[RedPixelChannel].sum_average[i]+=
        x*density_xy[x].direction[i].red;
      channel_features[GreenPixelChannel].sum_average[i]+=
        x*density_xy[x].direction[i].green;
      channel_features[BluePixelChannel].sum_average[i]+=
        x*density_xy[x].direction[i].blue;
      if (image->colorspace == CMYKColorspace)
        channel_features[BlackPixelChannel].sum_average[i]+=
          x*density_xy[x].direction[i].black;
      if (image->alpha_trait != UndefinedPixelTrait)
        channel_features[AlphaPixelChannel].sum_average[i]+=
          x*density_xy[x].direction[i].alpha;
      /*
        Sum entropy.
      */
      channel_features[RedPixelChannel].sum_entropy[i]-=
        density_xy[x].direction[i].red*
        MagickLog10(density_xy[x].direction[i].red);
      channel_features[GreenPixelChannel].sum_entropy[i]-=
        density_xy[x].direction[i].green*
        MagickLog10(density_xy[x].direction[i].green);
      channel_features[BluePixelChannel].sum_entropy[i]-=
        density_xy[x].direction[i].blue*
        MagickLog10(density_xy[x].direction[i].blue);
      if (image->colorspace == CMYKColorspace)
        channel_features[BlackPixelChannel].sum_entropy[i]-=
          density_xy[x].direction[i].black*
          MagickLog10(density_xy[x].direction[i].black);
      if (image->alpha_trait != UndefinedPixelTrait)
        channel_features[AlphaPixelChannel].sum_entropy[i]-=
          density_xy[x].direction[i].alpha*
          MagickLog10(density_xy[x].direction[i].alpha);
      /*
        Sum variance.
      */
      channel_features[RedPixelChannel].sum_variance[i]+=
        (x-channel_features[RedPixelChannel].sum_entropy[i])*
        (x-channel_features[RedPixelChannel].sum_entropy[i])*
        density_xy[x].direction[i].red;
      channel_features[GreenPixelChannel].sum_variance[i]+=
        (x-channel_features[GreenPixelChannel].sum_entropy[i])*
        (x-channel_features[GreenPixelChannel].sum_entropy[i])*
        density_xy[x].direction[i].green;
      channel_features[BluePixelChannel].sum_variance[i]+=
        (x-channel_features[BluePixelChannel].sum_entropy[i])*
        (x-channel_features[BluePixelChannel].sum_entropy[i])*
        density_xy[x].direction[i].blue;
      if (image->colorspace == CMYKColorspace)
        channel_features[BlackPixelChannel].sum_variance[i]+=
          (x-channel_features[BlackPixelChannel].sum_entropy[i])*
          (x-channel_features[BlackPixelChannel].sum_entropy[i])*
          density_xy[x].direction[i].black;
      if (image->alpha_trait != UndefinedPixelTrait)
        channel_features[AlphaPixelChannel].sum_variance[i]+=
          (x-channel_features[AlphaPixelChannel].sum_entropy[i])*
          (x-channel_features[AlphaPixelChannel].sum_entropy[i])*
          density_xy[x].direction[i].alpha;
    }
  }
  /*
    Compute more texture features.
  */
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(image,image,number_grays,1)
#endif
  for (i=0; i < 4; i++)
  {
    register ssize_t
      y;

    for (y=0; y < (ssize_t) number_grays; y++)
    {
      register ssize_t
        x;

      for (x=0; x < (ssize_t) number_grays; x++)
      {
        /*
          Sum of Squares: Variance
        */
        variance.direction[i].red+=(y-mean.direction[i].red+1)*
          (y-mean.direction[i].red+1)*cooccurrence[x][y].direction[i].red;
        variance.direction[i].green+=(y-mean.direction[i].green+1)*
          (y-mean.direction[i].green+1)*cooccurrence[x][y].direction[i].green;
        variance.direction[i].blue+=(y-mean.direction[i].blue+1)*
          (y-mean.direction[i].blue+1)*cooccurrence[x][y].direction[i].blue;
        if (image->colorspace == CMYKColorspace)
          variance.direction[i].black+=(y-mean.direction[i].black+1)*
            (y-mean.direction[i].black+1)*cooccurrence[x][y].direction[i].black;
        if (image->alpha_trait != UndefinedPixelTrait)
          variance.direction[i].alpha+=(y-mean.direction[i].alpha+1)*
            (y-mean.direction[i].alpha+1)*
            cooccurrence[x][y].direction[i].alpha;
        /*
          Sum average / Difference Variance.
        */
        density_xy[MagickAbsoluteValue(y-x)].direction[i].red+=
          cooccurrence[x][y].direction[i].red;
        density_xy[MagickAbsoluteValue(y-x)].direction[i].green+=
          cooccurrence[x][y].direction[i].green;
        density_xy[MagickAbsoluteValue(y-x)].direction[i].blue+=
          cooccurrence[x][y].direction[i].blue;
        if (image->colorspace == CMYKColorspace)
          density_xy[MagickAbsoluteValue(y-x)].direction[i].black+=
            cooccurrence[x][y].direction[i].black;
        if (image->alpha_trait != UndefinedPixelTrait)
          density_xy[MagickAbsoluteValue(y-x)].direction[i].alpha+=
            cooccurrence[x][y].direction[i].alpha;
        /*
          Information Measures of Correlation.
        */
        entropy_xy.direction[i].red-=cooccurrence[x][y].direction[i].red*
          MagickLog10(cooccurrence[x][y].direction[i].red);
        entropy_xy.direction[i].green-=cooccurrence[x][y].direction[i].green*
          MagickLog10(cooccurrence[x][y].direction[i].green);
        entropy_xy.direction[i].blue-=cooccurrence[x][y].direction[i].blue*
          MagickLog10(cooccurrence[x][y].direction[i].blue);
        if (image->colorspace == CMYKColorspace)
          entropy_xy.direction[i].black-=cooccurrence[x][y].direction[i].black*
            MagickLog10(cooccurrence[x][y].direction[i].black);
        if (image->alpha_trait != UndefinedPixelTrait)
          entropy_xy.direction[i].alpha-=
            cooccurrence[x][y].direction[i].alpha*MagickLog10(
            cooccurrence[x][y].direction[i].alpha);
        entropy_xy1.direction[i].red-=(cooccurrence[x][y].direction[i].red*
          MagickLog10(density_x[x].direction[i].red*density_y[y].direction[i].red));
        entropy_xy1.direction[i].green-=(cooccurrence[x][y].direction[i].green*
          MagickLog10(density_x[x].direction[i].green*
          density_y[y].direction[i].green));
        entropy_xy1.direction[i].blue-=(cooccurrence[x][y].direction[i].blue*
          MagickLog10(density_x[x].direction[i].blue*density_y[y].direction[i].blue));
        if (image->colorspace == CMYKColorspace)
          entropy_xy1.direction[i].black-=(
            cooccurrence[x][y].direction[i].black*MagickLog10(
            density_x[x].direction[i].black*density_y[y].direction[i].black));
        if (image->alpha_trait != UndefinedPixelTrait)
          entropy_xy1.direction[i].alpha-=(
            cooccurrence[x][y].direction[i].alpha*MagickLog10(
            density_x[x].direction[i].alpha*density_y[y].direction[i].alpha));
        entropy_xy2.direction[i].red-=(density_x[x].direction[i].red*
          density_y[y].direction[i].red*MagickLog10(density_x[x].direction[i].red*
          density_y[y].direction[i].red));
        entropy_xy2.direction[i].green-=(density_x[x].direction[i].green*
          density_y[y].direction[i].green*MagickLog10(density_x[x].direction[i].green*
          density_y[y].direction[i].green));
        entropy_xy2.direction[i].blue-=(density_x[x].direction[i].blue*
          density_y[y].direction[i].blue*MagickLog10(density_x[x].direction[i].blue*
          density_y[y].direction[i].blue));
        if (image->colorspace == CMYKColorspace)
          entropy_xy2.direction[i].black-=(density_x[x].direction[i].black*
            density_y[y].direction[i].black*MagickLog10(
            density_x[x].direction[i].black*density_y[y].direction[i].black));
        if (image->alpha_trait != UndefinedPixelTrait)
          entropy_xy2.direction[i].alpha-=(density_x[x].direction[i].alpha*
            density_y[y].direction[i].alpha*MagickLog10(
            density_x[x].direction[i].alpha*density_y[y].direction[i].alpha));
      }
    }
    channel_features[RedPixelChannel].variance_sum_of_squares[i]=
      variance.direction[i].red;
    channel_features[GreenPixelChannel].variance_sum_of_squares[i]=
      variance.direction[i].green;
    channel_features[BluePixelChannel].variance_sum_of_squares[i]=
      variance.direction[i].blue;
    if (image->colorspace == CMYKColorspace)
      channel_features[BlackPixelChannel].variance_sum_of_squares[i]=
        variance.direction[i].black;
    if (image->alpha_trait != UndefinedPixelTrait)
      channel_features[AlphaPixelChannel].variance_sum_of_squares[i]=
        variance.direction[i].alpha;
  }
  /*
    Compute more texture features.
  */
  (void) memset(&variance,0,sizeof(variance));
  (void) memset(&sum_squares,0,sizeof(sum_squares));
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(image,image,number_grays,1)
#endif
  for (i=0; i < 4; i++)
  {
    register ssize_t
      x;

    for (x=0; x < (ssize_t) number_grays; x++)
    {
      /*
        Difference variance.
      */
      variance.direction[i].red+=density_xy[x].direction[i].red;
      variance.direction[i].green+=density_xy[x].direction[i].green;
      variance.direction[i].blue+=density_xy[x].direction[i].blue;
      if (image->colorspace == CMYKColorspace)
        variance.direction[i].black+=density_xy[x].direction[i].black;
      if (image->alpha_trait != UndefinedPixelTrait)
        variance.direction[i].alpha+=density_xy[x].direction[i].alpha;
      sum_squares.direction[i].red+=density_xy[x].direction[i].red*
        density_xy[x].direction[i].red;
      sum_squares.direction[i].green+=density_xy[x].direction[i].green*
        density_xy[x].direction[i].green;
      sum_squares.direction[i].blue+=density_xy[x].direction[i].blue*
        density_xy[x].direction[i].blue;
      if (image->colorspace == CMYKColorspace)
        sum_squares.direction[i].black+=density_xy[x].direction[i].black*
          density_xy[x].direction[i].black;
      if (image->alpha_trait != UndefinedPixelTrait)
        sum_squares.direction[i].alpha+=density_xy[x].direction[i].alpha*
          density_xy[x].direction[i].alpha;
      /*
        Difference entropy.
      */
      channel_features[RedPixelChannel].difference_entropy[i]-=
        density_xy[x].direction[i].red*
        MagickLog10(density_xy[x].direction[i].red);
      channel_features[GreenPixelChannel].difference_entropy[i]-=
        density_xy[x].direction[i].green*
        MagickLog10(density_xy[x].direction[i].green);
      channel_features[BluePixelChannel].difference_entropy[i]-=
        density_xy[x].direction[i].blue*
        MagickLog10(density_xy[x].direction[i].blue);
      if (image->colorspace == CMYKColorspace)
        channel_features[BlackPixelChannel].difference_entropy[i]-=
          density_xy[x].direction[i].black*
          MagickLog10(density_xy[x].direction[i].black);
      if (image->alpha_trait != UndefinedPixelTrait)
        channel_features[AlphaPixelChannel].difference_entropy[i]-=
          density_xy[x].direction[i].alpha*
          MagickLog10(density_xy[x].direction[i].alpha);
      /*
        Information Measures of Correlation.
      */
      entropy_x.direction[i].red-=(density_x[x].direction[i].red*
        MagickLog10(density_x[x].direction[i].red));
      entropy_x.direction[i].green-=(density_x[x].direction[i].green*
        MagickLog10(density_x[x].direction[i].green));
      entropy_x.direction[i].blue-=(density_x[x].direction[i].blue*
        MagickLog10(density_x[x].direction[i].blue));
      if (image->colorspace == CMYKColorspace)
        entropy_x.direction[i].black-=(density_x[x].direction[i].black*
          MagickLog10(density_x[x].direction[i].black));
      if (image->alpha_trait != UndefinedPixelTrait)
        entropy_x.direction[i].alpha-=(density_x[x].direction[i].alpha*
          MagickLog10(density_x[x].direction[i].alpha));
      entropy_y.direction[i].red-=(density_y[x].direction[i].red*
        MagickLog10(density_y[x].direction[i].red));
      entropy_y.direction[i].green-=(density_y[x].direction[i].green*
        MagickLog10(density_y[x].direction[i].green));
      entropy_y.direction[i].blue-=(density_y[x].direction[i].blue*
        MagickLog10(density_y[x].direction[i].blue));
      if (image->colorspace == CMYKColorspace)
        entropy_y.direction[i].black-=(density_y[x].direction[i].black*
          MagickLog10(density_y[x].direction[i].black));
      if (image->alpha_trait != UndefinedPixelTrait)
        entropy_y.direction[i].alpha-=(density_y[x].direction[i].alpha*
          MagickLog10(density_y[x].direction[i].alpha));
    }
    /*
      Difference variance.
    */
    channel_features[RedPixelChannel].difference_variance[i]=
      (((double) number_grays*number_grays*sum_squares.direction[i].red)-
      (variance.direction[i].red*variance.direction[i].red))/
      ((double) number_grays*number_grays*number_grays*number_grays);
    channel_features[GreenPixelChannel].difference_variance[i]=
      (((double) number_grays*number_grays*sum_squares.direction[i].green)-
      (variance.direction[i].green*variance.direction[i].green))/
      ((double) number_grays*number_grays*number_grays*number_grays);
    channel_features[BluePixelChannel].difference_variance[i]=
      (((double) number_grays*number_grays*sum_squares.direction[i].blue)-
      (variance.direction[i].blue*variance.direction[i].blue))/
      ((double) number_grays*number_grays*number_grays*number_grays);
    if (image->colorspace == CMYKColorspace)
      channel_features[BlackPixelChannel].difference_variance[i]=
        (((double) number_grays*number_grays*sum_squares.direction[i].black)-
        (variance.direction[i].black*variance.direction[i].black))/
        ((double) number_grays*number_grays*number_grays*number_grays);
    if (image->alpha_trait != UndefinedPixelTrait)
      channel_features[AlphaPixelChannel].difference_variance[i]=
        (((double) number_grays*number_grays*sum_squares.direction[i].alpha)-
        (variance.direction[i].alpha*variance.direction[i].alpha))/
        ((double) number_grays*number_grays*number_grays*number_grays);
    /*
      Information Measures of Correlation.
    */
    channel_features[RedPixelChannel].measure_of_correlation_1[i]=
      (entropy_xy.direction[i].red-entropy_xy1.direction[i].red)/
      (entropy_x.direction[i].red > entropy_y.direction[i].red ?
       entropy_x.direction[i].red : entropy_y.direction[i].red);
    channel_features[GreenPixelChannel].measure_of_correlation_1[i]=
      (entropy_xy.direction[i].green-entropy_xy1.direction[i].green)/
      (entropy_x.direction[i].green > entropy_y.direction[i].green ?
       entropy_x.direction[i].green : entropy_y.direction[i].green);
    channel_features[BluePixelChannel].measure_of_correlation_1[i]=
      (entropy_xy.direction[i].blue-entropy_xy1.direction[i].blue)/
      (entropy_x.direction[i].blue > entropy_y.direction[i].blue ?
       entropy_x.direction[i].blue : entropy_y.direction[i].blue);
    if (image->colorspace == CMYKColorspace)
      channel_features[BlackPixelChannel].measure_of_correlation_1[i]=
        (entropy_xy.direction[i].black-entropy_xy1.direction[i].black)/
        (entropy_x.direction[i].black > entropy_y.direction[i].black ?
         entropy_x.direction[i].black : entropy_y.direction[i].black);
    if (image->alpha_trait != UndefinedPixelTrait)
      channel_features[AlphaPixelChannel].measure_of_correlation_1[i]=
        (entropy_xy.direction[i].alpha-entropy_xy1.direction[i].alpha)/
        (entropy_x.direction[i].alpha > entropy_y.direction[i].alpha ?
         entropy_x.direction[i].alpha : entropy_y.direction[i].alpha);
    channel_features[RedPixelChannel].measure_of_correlation_2[i]=
      (sqrt(fabs(1.0-exp(-2.0*(double) (entropy_xy2.direction[i].red-
      entropy_xy.direction[i].red)))));
    channel_features[GreenPixelChannel].measure_of_correlation_2[i]=
      (sqrt(fabs(1.0-exp(-2.0*(double) (entropy_xy2.direction[i].green-
      entropy_xy.direction[i].green)))));
    channel_features[BluePixelChannel].measure_of_correlation_2[i]=
      (sqrt(fabs(1.0-exp(-2.0*(double) (entropy_xy2.direction[i].blue-
      entropy_xy.direction[i].blue)))));
    if (image->colorspace == CMYKColorspace)
      channel_features[BlackPixelChannel].measure_of_correlation_2[i]=
        (sqrt(fabs(1.0-exp(-2.0*(double) (entropy_xy2.direction[i].black-
        entropy_xy.direction[i].black)))));
    if (image->alpha_trait != UndefinedPixelTrait)
      channel_features[AlphaPixelChannel].measure_of_correlation_2[i]=
        (sqrt(fabs(1.0-exp(-2.0*(double) (entropy_xy2.direction[i].alpha-
        entropy_xy.direction[i].alpha)))));
  }
  /*
    Compute more texture features.
  */
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(image,image,number_grays,1)
#endif
  for (i=0; i < 4; i++)
  {
    ssize_t
      z;

    for (z=0; z < (ssize_t) number_grays; z++)
    {
      register ssize_t
        y;

      ChannelStatistics
        pixel;

      (void) memset(&pixel,0,sizeof(pixel));
      for (y=0; y < (ssize_t) number_grays; y++)
      {
        register ssize_t
          x;

        for (x=0; x < (ssize_t) number_grays; x++)
        {
          /*
            Contrast:  amount of local variations present in an image.
          */
          if (((y-x) == z) || ((x-y) == z))
            {
              pixel.direction[i].red+=cooccurrence[x][y].direction[i].red;
              pixel.direction[i].green+=cooccurrence[x][y].direction[i].green;
              pixel.direction[i].blue+=cooccurrence[x][y].direction[i].blue;
              if (image->colorspace == CMYKColorspace)
                pixel.direction[i].black+=cooccurrence[x][y].direction[i].black;
              if (image->alpha_trait != UndefinedPixelTrait)
                pixel.direction[i].alpha+=
                  cooccurrence[x][y].direction[i].alpha;
            }
          /*
            Maximum Correlation Coefficient.
          */
          Q[z][y].direction[i].red+=cooccurrence[z][x].direction[i].red*
            cooccurrence[y][x].direction[i].red/density_x[z].direction[i].red/
            density_y[x].direction[i].red;
          Q[z][y].direction[i].green+=cooccurrence[z][x].direction[i].green*
            cooccurrence[y][x].direction[i].green/
            density_x[z].direction[i].green/density_y[x].direction[i].red;
          Q[z][y].direction[i].blue+=cooccurrence[z][x].direction[i].blue*
            cooccurrence[y][x].direction[i].blue/density_x[z].direction[i].blue/
            density_y[x].direction[i].blue;
          if (image->colorspace == CMYKColorspace)
            Q[z][y].direction[i].black+=cooccurrence[z][x].direction[i].black*
              cooccurrence[y][x].direction[i].black/
              density_x[z].direction[i].black/density_y[x].direction[i].black;
          if (image->alpha_trait != UndefinedPixelTrait)
            Q[z][y].direction[i].alpha+=
              cooccurrence[z][x].direction[i].alpha*
              cooccurrence[y][x].direction[i].alpha/
              density_x[z].direction[i].alpha/
              density_y[x].direction[i].alpha;
        }
      }
      channel_features[RedPixelChannel].contrast[i]+=z*z*
        pixel.direction[i].red;
      channel_features[GreenPixelChannel].contrast[i]+=z*z*
        pixel.direction[i].green;
      channel_features[BluePixelChannel].contrast[i]+=z*z*
        pixel.direction[i].blue;
      if (image->colorspace == CMYKColorspace)
        channel_features[BlackPixelChannel].contrast[i]+=z*z*
          pixel.direction[i].black;
      if (image->alpha_trait != UndefinedPixelTrait)
        channel_features[AlphaPixelChannel].contrast[i]+=z*z*
          pixel.direction[i].alpha;
    }
    /*
      Maximum Correlation Coefficient.
      Future: return second largest eigenvalue of Q.
    */
    channel_features[RedPixelChannel].maximum_correlation_coefficient[i]=
      sqrt((double) -1.0);
    channel_features[GreenPixelChannel].maximum_correlation_coefficient[i]=
      sqrt((double) -1.0);
    channel_features[BluePixelChannel].maximum_correlation_coefficient[i]=
      sqrt((double) -1.0);
    if (image->colorspace == CMYKColorspace)
      channel_features[BlackPixelChannel].maximum_correlation_coefficient[i]=
        sqrt((double) -1.0);
    if (image->alpha_trait != UndefinedPixelTrait)
      channel_features[AlphaPixelChannel].maximum_correlation_coefficient[i]=
        sqrt((double) -1.0);
  }
  /*
    Relinquish resources.
  */
  sum=(ChannelStatistics *) RelinquishMagickMemory(sum);
  for (i=0; i < (ssize_t) number_grays; i++)
    Q[i]=(ChannelStatistics *) RelinquishMagickMemory(Q[i]);
  Q=(ChannelStatistics **) RelinquishMagickMemory(Q);
  density_y=(ChannelStatistics *) RelinquishMagickMemory(density_y);
  density_xy=(ChannelStatistics *) RelinquishMagickMemory(density_xy);
  density_x=(ChannelStatistics *) RelinquishMagickMemory(density_x);
  for (i=0; i < (ssize_t) number_grays; i++)
    cooccurrence[i]=(ChannelStatistics *)
      RelinquishMagickMemory(cooccurrence[i]);
  cooccurrence=(ChannelStatistics **) RelinquishMagickMemory(cooccurrence);
  return(channel_features);
}