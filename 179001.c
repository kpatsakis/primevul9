static MagickBooleanType ComparePixels(const LayerMethod method,
  const PixelInfo *p,const PixelInfo *q)
{
  double
    o1,
    o2;

  /*
    Any change in pixel values
  */
  if (method == CompareAnyLayer)
    return((MagickBooleanType)(IsFuzzyEquivalencePixelInfo(p,q) == MagickFalse));

  o1 = (p->alpha_trait != UndefinedPixelTrait) ? p->alpha : OpaqueAlpha;
  o2 = (q->alpha_trait != UndefinedPixelTrait) ? q->alpha : OpaqueAlpha;
  /*
    Pixel goes from opaque to transprency.
  */
  if (method == CompareClearLayer)
    return((MagickBooleanType) ( (o1 >= ((double) QuantumRange/2.0)) &&
      (o2 < ((double) QuantumRange/2.0)) ) );
  /*
    Overlay would change first pixel by second.
  */
  if (method == CompareOverlayLayer)
    {
      if (o2 < ((double) QuantumRange/2.0))
        return MagickFalse;
      return((MagickBooleanType) (IsFuzzyEquivalencePixelInfo(p,q) == MagickFalse));
    }
  return(MagickFalse);
}