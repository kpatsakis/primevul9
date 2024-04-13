MagickExport MagickBooleanType IsFuzzyEquivalencePixelInfo(const PixelInfo *p,
  const PixelInfo *q)
{
  double
    fuzz,
    pixel;

  register double
    scale,
    distance;

  fuzz=(double) MagickMax(MagickMax(p->fuzz,q->fuzz),(MagickRealType)
    MagickSQ1_2);
  fuzz*=fuzz;
  scale=1.0;
  distance=0.0;
  if ((p->alpha_trait != UndefinedPixelTrait) ||
      (q->alpha_trait != UndefinedPixelTrait))
    {
      /*
        Transparencies are involved - set alpha distance.
      */
      pixel=(p->alpha_trait != UndefinedPixelTrait ? p->alpha : OpaqueAlpha)-
        (q->alpha_trait != UndefinedPixelTrait ? q->alpha : OpaqueAlpha);
      distance=pixel*pixel;
      if (distance > fuzz)
        return(MagickFalse);
      /*
        Generate a alpha scaling factor to generate a 4D cone on colorspace.
        If one color is transparent, distance has no color component.
      */
      if (p->alpha_trait != UndefinedPixelTrait)
        scale=(QuantumScale*p->alpha);
      if (q->alpha_trait != UndefinedPixelTrait)
        scale*=(QuantumScale*q->alpha);
      if (scale <= MagickEpsilon )
        return(MagickTrue);
    }
  /*
    CMYK create a CMY cube with a multi-dimensional cone toward black.
  */
  if (p->colorspace == CMYKColorspace)
    {
      pixel=p->black-q->black;
      distance+=pixel*pixel*scale;
      if (distance > fuzz)
        return(MagickFalse);
      scale*=(double) (QuantumScale*(QuantumRange-p->black));
      scale*=(double) (QuantumScale*(QuantumRange-q->black));
    }
  /*
    RGB or CMY color cube.
  */
  distance*=3.0;  /* rescale appropriately */
  fuzz*=3.0;
  pixel=p->red-q->red;
  if ((p->colorspace == HSLColorspace) || (p->colorspace == HSBColorspace) ||
      (p->colorspace == HWBColorspace))
    {
      /*
        This calculates a arc distance for hue-- it should be a vector
        angle of 'S'/'W' length with 'L'/'B' forming appropriate cones.
        In other words this is a hack - Anthony.
      */
      if (fabs((double) pixel) > (QuantumRange/2))
        pixel-=QuantumRange;
      pixel*=2;
    }
  distance+=pixel*pixel*scale;
  if (distance > fuzz)
    return(MagickFalse);
  pixel=p->green-q->green;
  distance+=pixel*pixel*scale;
  if (distance > fuzz)
    return(MagickFalse);
  pixel=p->blue-q->blue;
  distance+=pixel*pixel*scale;
  if (distance > fuzz)
    return(MagickFalse);
  return(MagickTrue);
}