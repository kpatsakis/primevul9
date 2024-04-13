static void InsertComplexFloatRow(float *p,int y,Image *image,double MinVal,
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
  q = QueueAuthenticPixels(image, 0, y, image->columns, 1,exception);
  if (q == (PixelPacket *) NULL)
    return;
  for (x = 0; x < (ssize_t) image->columns; x++)
  {
    if (*p > 0)
      {
        f=(*p/MaxVal)*(Quantum) (QuantumRange-GetPixelRed(q));
        if ((f+GetPixelRed(q)) < QuantumRange)
          SetPixelRed(q,GetPixelRed(q)+ClampToQuantum(f));
        else
          SetPixelRed(q,QuantumRange);
        f/=2.0;
        if (f < GetPixelGreen(q))
          {
            SetPixelBlue(q,GetPixelBlue(q)-ClampToQuantum(f));
            SetPixelGreen(q,GetPixelBlue(q));
          }
        else
          {
            SetPixelGreen(q,0);
            SetPixelBlue(q,0);
          }
      }
    if (*p < 0)
      {
        f=(*p/MaxVal)*(Quantum) (QuantumRange-GetPixelBlue(q));
        if ((f+GetPixelBlue(q)) < QuantumRange)
          SetPixelBlue(q,GetPixelBlue(q)+ClampToQuantum(f));
        else
          SetPixelBlue(q,QuantumRange);
        f/=2.0;
        if (f < GetPixelGreen(q))
          {
            SetPixelRed(q,GetPixelRed(q)-ClampToQuantum(f));
            SetPixelGreen(q,GetPixelRed(q));
          }
        else
          {
            SetPixelGreen(q,0);
            SetPixelRed(q,0);
          }
      }
    p++;
    q++;
  }
  if (!SyncAuthenticPixels(image,exception))
    return;
  return;
}