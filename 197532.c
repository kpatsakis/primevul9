static double OTSUThreshold(const Image *image,const double *histogram,
  ExceptionInfo *exception)
{
  double
    max_sigma,
    *myu,
    *omega,
    *probability,
    *sigma,
    threshold;

  register ssize_t
    i;

  /*
    Compute optimal threshold from maximization of inter-class variance.
  */
  myu=(double *) AcquireQuantumMemory(MaxIntensity+1UL,sizeof(*myu));
  omega=(double *) AcquireQuantumMemory(MaxIntensity+1UL,sizeof(*omega));
  probability=(double *) AcquireQuantumMemory(MaxIntensity+1UL,
    sizeof(*probability));
  sigma=(double *) AcquireQuantumMemory(MaxIntensity+1UL,sizeof(*sigma));
  if ((myu == (double *) NULL) || (omega == (double *) NULL) ||
      (probability == (double *) NULL) || (sigma == (double *) NULL))
    {
      if (sigma != (double *) NULL)
        sigma=(double *) RelinquishMagickMemory(sigma);
      if (probability != (double *) NULL)
        probability=(double *) RelinquishMagickMemory(probability);
      if (omega != (double *) NULL)
        omega=(double *) RelinquishMagickMemory(omega);
      if (myu != (double *) NULL)
        myu=(double *) RelinquishMagickMemory(myu);
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      return(-1.0);
    }
  /*
    Calculate probability density.
  */
  for (i=0; i <= (ssize_t) MaxIntensity; i++)
    probability[i]=histogram[i];
  /*
    Generate probability of graylevels and mean value for separation.
  */
  omega[0]=probability[0];
  myu[0]=0.0;
  for (i=1; i <= (ssize_t) MaxIntensity; i++)
  {
    omega[i]=omega[i-1]+probability[i];
    myu[i]=myu[i-1]+i*probability[i];
  }
  /*
    Sigma maximization: inter-class variance and compute optimal threshold.
  */
  threshold=0;
  max_sigma=0.0;
  for (i=0; i < (ssize_t) MaxIntensity; i++)
  {
    sigma[i]=0.0;
    if ((omega[i] != 0.0) && (omega[i] != 1.0))
      sigma[i]=pow(myu[MaxIntensity]*omega[i]-myu[i],2.0)/(omega[i]*(1.0-
        omega[i]));
    if (sigma[i] > max_sigma)
      {
        max_sigma=sigma[i];
        threshold=(double) i;
      }
  }
  /*
    Free resources.
  */
  myu=(double *) RelinquishMagickMemory(myu);
  omega=(double *) RelinquishMagickMemory(omega);
  probability=(double *) RelinquishMagickMemory(probability);
  sigma=(double *) RelinquishMagickMemory(sigma);
  return(100.0*threshold/MaxIntensity);
}