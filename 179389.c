static void InsertComplexFloatRow(float *p, int y, Image * image, double MinVal,
                                  double MaxVal)
{
  ExceptionInfo
    *exception;

  double f;
  int x;
  register PixelPacket *q;

  if (MinVal == 0)
    MinVal = -1;
  if (MaxVal == 0)
    MaxVal = 1;

  exception=(&image->exception);
  q = QueueAuthenticPixels(image, 0, y, image->columns, 1,exception);
  if (q == (PixelPacket *) NULL)
    return;
  for (x = 0; x < (ssize_t) image->columns; x++)
  {
    if (*p > 0)
    {
      f = (*p / MaxVal) * (QuantumRange - GetPixelRed(q));
      if (f + GetPixelRed(q) > QuantumRange)
        SetPixelRed(q,QuantumRange);
      else
        SetPixelRed(q,GetPixelRed(q)+(int) f);
      if ((int) f / 2.0 > GetPixelGreen(q))
        {
          SetPixelGreen(q,0);
          SetPixelBlue(q,0);
        }
      else
        {
          SetPixelBlue(q,GetPixelBlue(q)-(int) (f/2.0));
          SetPixelGreen(q,GetPixelBlue(q));
        }
    }
    if (*p < 0)
    {
      f = (*p / MaxVal) * (QuantumRange - GetPixelBlue(q));
      if (f + GetPixelBlue(q) > QuantumRange)
        SetPixelBlue(q,QuantumRange);
      else
        SetPixelBlue(q,GetPixelBlue(q)+(int) f);
      if ((int) f / 2.0 > q->green)
        {
          SetPixelGreen(q,0);
          SetPixelRed(q,0);
        }
      else
        {
          SetPixelRed(q,GetPixelRed(q)-(int) (f/2.0));
          SetPixelGreen(q,GetPixelRed(q));
        }
    }
    p++;
    q++;
  }
  if (!SyncAuthenticPixels(image,exception))
    return;
  return;
}