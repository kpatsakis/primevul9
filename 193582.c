static MagickBooleanType TraceEdges(Image *edge_image,CacheView *edge_view,
  MatrixInfo *canny_cache,const ssize_t x,const ssize_t y,
  const double lower_threshold,ExceptionInfo *exception)
{
  CannyInfo
    edge,
    pixel;

  MagickBooleanType
    status;

  register Quantum
    *q;

  register ssize_t
    i;

  q=GetCacheViewAuthenticPixels(edge_view,x,y,1,1,exception);
  if (q == (Quantum *) NULL)
    return(MagickFalse);
  *q=QuantumRange;
  status=SyncCacheViewAuthenticPixels(edge_view,exception);
  if (status == MagickFalse)
    return(MagickFalse);
  if (GetMatrixElement(canny_cache,0,0,&edge) == MagickFalse)
    return(MagickFalse);
  edge.x=x;
  edge.y=y;
  if (SetMatrixElement(canny_cache,0,0,&edge) == MagickFalse)
    return(MagickFalse);
  for (i=1; i != 0; )
  {
    ssize_t
      v;

    i--;
    status=GetMatrixElement(canny_cache,i,0,&edge);
    if (status == MagickFalse)
      return(MagickFalse);
    for (v=(-1); v <= 1; v++)
    {
      ssize_t
        u;

      for (u=(-1); u <= 1; u++)
      {
        if ((u == 0) && (v == 0))
          continue;
        if (IsAuthenticPixel(edge_image,edge.x+u,edge.y+v) == MagickFalse)
          continue;
        /*
          Not an edge if gradient value is below the lower threshold.
        */
        q=GetCacheViewAuthenticPixels(edge_view,edge.x+u,edge.y+v,1,1,
          exception);
        if (q == (Quantum *) NULL)
          return(MagickFalse);
        status=GetMatrixElement(canny_cache,edge.x+u,edge.y+v,&pixel);
        if (status == MagickFalse)
          return(MagickFalse);
        if ((GetPixelIntensity(edge_image,q) == 0.0) &&
            (pixel.intensity >= lower_threshold))
          {
            *q=QuantumRange;
            status=SyncCacheViewAuthenticPixels(edge_view,exception);
            if (status == MagickFalse)
              return(MagickFalse);
            edge.x+=u;
            edge.y+=v;
            status=SetMatrixElement(canny_cache,i,0,&edge);
            if (status == MagickFalse)
              return(MagickFalse);
            i++;
          }
      }
    }
  }
  return(MagickTrue);
}