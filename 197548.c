static double KapurThreshold(const Image *image,const double *histogram,
  ExceptionInfo *exception)
{
#define MaxIntensity  255

  double
    *black_entropy,
    *cumulative_histogram,
    entropy,
    epsilon,
    maximum_entropy,
    *white_entropy;

  register ssize_t
    i,
    j;

  size_t
    threshold;

  /*
    Compute optimal threshold from the entopy of the histogram.
  */
  cumulative_histogram=(double *) AcquireQuantumMemory(MaxIntensity+1UL,
    sizeof(*cumulative_histogram));
  black_entropy=(double *) AcquireQuantumMemory(MaxIntensity+1UL,
    sizeof(*black_entropy));
  white_entropy=(double *) AcquireQuantumMemory(MaxIntensity+1UL,
    sizeof(*white_entropy));
  if ((cumulative_histogram == (double *) NULL) ||
      (black_entropy == (double *) NULL) || (white_entropy == (double *) NULL))
    {
      if (white_entropy != (double *) NULL)
        white_entropy=(double *) RelinquishMagickMemory(white_entropy);
      if (black_entropy != (double *) NULL)
        black_entropy=(double *) RelinquishMagickMemory(black_entropy);
      if (cumulative_histogram != (double *) NULL)
        cumulative_histogram=(double *)
          RelinquishMagickMemory(cumulative_histogram);
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      return(-1.0);
    }
   /*
     Entropy for black and white parts of the histogram.
   */
   cumulative_histogram[0]=histogram[0];
   for (i=1; i <= MaxIntensity; i++)
     cumulative_histogram[i]=cumulative_histogram[i-1]+histogram[i];
   epsilon=MagickMinimumValue;
   for (j=0; j <= MaxIntensity; j++)
   {
     /*
       Black entropy.
     */
     black_entropy[j]=0.0;
     if (cumulative_histogram[j] > epsilon)
       {
         entropy=0.0;
         for (i=0; i <= j; i++)
           if (histogram[i] > epsilon)
             entropy-=histogram[i]/cumulative_histogram[j]*
               log(histogram[i]/cumulative_histogram[j]);
         black_entropy[j]=entropy;
       }
     /*
       White entropy.
     */
     white_entropy[j]=0.0;
     if ((1.0-cumulative_histogram[j]) > epsilon)
       {
         entropy=0.0;
         for (i=j+1; i <= MaxIntensity; i++)
           if (histogram[i] > epsilon)
             entropy-=histogram[i]/(1.0-cumulative_histogram[j])*
               log(histogram[i]/(1.0-cumulative_histogram[j]));
         white_entropy[j]=entropy;
       }
   }
  /*
    Find histogram bin with maximum entropy.
  */
  maximum_entropy=black_entropy[0]+white_entropy[0];
  threshold=0;
  for (j=1; j <= MaxIntensity; j++)
    if ((black_entropy[j]+white_entropy[j]) > maximum_entropy)
      {
        maximum_entropy=black_entropy[j]+white_entropy[j];
        threshold=(size_t) j;
      }
  /*
    Free resources.
  */
  white_entropy=(double *) RelinquishMagickMemory(white_entropy);
  black_entropy=(double *) RelinquishMagickMemory(black_entropy);
  cumulative_histogram=(double *) RelinquishMagickMemory(cumulative_histogram);
  return(100.0*threshold/MaxIntensity);
}