static void InsertComplexDoubleRow(double *p,int y,Image *image,double MinVal,
  double MaxVal)
{
  ExceptionInfo
    *exception;

  double f;
  int x;
  register PixelPacket *q;

  if (MinVal >= 0)
    MinVal = -1;
  if (MaxVal <= 0)
    MaxVal = 1;

  exception=(&image->exception);
  q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
  if (q == (PixelPacket *) NULL)
    return;
  for (x = 0; x < (ssize_t) image->columns; x++)
  {
    if (*p > 0)
      {
        f=(*p/MaxVal)*(Quantum) (QuantumRange-GetPixelRed(q));
        if ((f+GetPixelRed(q)) >= QuantumRange)
          SetPixelRed(q,QuantumRange);
        else
          SetPixelRed(q,GetPixelRed(q)+ClampToQuantum(f));
        f=GetPixelGreen(q)-f/2.0;
        if (f <= 0.0)
          {
            SetPixelGreen(q,0);
            SetPixelBlue(q,0);
          }
        else
          {
            SetPixelBlue(q,ClampToQuantum(f));
            SetPixelGreen(q,ClampToQuantum(f));
          }
      }
    if (*p < 0)
      {
        f=(*p/MinVal)*(Quantum) (QuantumRange-GetPixelBlue(q));
        if ((f+GetPixelBlue(q)) >= QuantumRange)
          SetPixelBlue(q,QuantumRange);
        else
          SetPixelBlue(q,GetPixelBlue(q)+ClampToQuantum(f));
        f=GetPixelGreen(q)-f/2.0;
        if (f <= 0.0)
          {
            SetPixelRed(q,0);
            SetPixelGreen(q,0);
          }
        else
          {
            SetPixelRed(q,ClampToQuantum(f));
            SetPixelGreen(q,ClampToQuantum(f));
          }
      }
    p++;
    q++;
  }
  if (!SyncAuthenticPixels(image,exception))
    return;
  return;
}