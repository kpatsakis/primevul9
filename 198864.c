static double TriangleThreshold(const double *histogram)
{
  double
    a,
    b,
    c,
    count,
    distance,
    inverse_ratio,
    max_distance,
    segment,
    x1,
    x2,
    y1,
    y2;

  register ssize_t
    i;

  ssize_t
    end,
    max,
    start,
    threshold;

  /*
    Compute optimal threshold with triangle algorithm.
  */
  start=0;  /* find start bin, first bin not zero count */
  for (i=0; i <= (ssize_t) MaxIntensity; i++)
    if (histogram[i] > 0.0)
      {
        start=i;
        break;
      }
  end=0;  /* find end bin, last bin not zero count */
  for (i=(ssize_t) MaxIntensity; i >= 0; i--)
    if (histogram[i] > 0.0)
      {
        end=i;
        break;
      }
  max=0;  /* find max bin, bin with largest count */
  count=0.0;
  for (i=0; i <= (ssize_t) MaxIntensity; i++)
    if (histogram[i] > count)
      {
        max=i;
        count=histogram[i];
      }
  /*
    Compute threshold at split point.
  */
  x1=(double) max;
  y1=histogram[max];
  x2=(double) end;
  if ((max-start) >= (end-max))
    x2=(double) start;
  y2=0.0;
  a=y1-y2;
  b=x2-x1;
  c=(-1.0)*(a*x1+b*y1);
  inverse_ratio=1.0/sqrt(a*a+b*b+c*c);
  threshold=0;
  max_distance=0.0;
  if (x2 == (double) start)
    for (i=start; i < max; i++)
    {
      segment=inverse_ratio*(a*i+b*histogram[i]+c);
      distance=sqrt(segment*segment);
      if ((distance > max_distance) && (segment > 0.0))
        {
          threshold=i;
          max_distance=distance;
        }
    }
  else
    for (i=end; i > max; i--)
    {
      segment=inverse_ratio*(a*i+b*histogram[i]+c);
      distance=sqrt(segment*segment);
      if ((distance > max_distance) && (segment < 0.0))
        {
          threshold=i;
          max_distance=distance;
        }
    }
  return(100.0*threshold/MaxIntensity);
}