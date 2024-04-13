MagickExport MagickBooleanType IsFuzzyEquivalencePixel(const Image *source,
  const Quantum *p,const Image *destination,const Quantum *q)
{
  double
    fuzz,
    pixel;

  register double
    distance,
    scale;

  fuzz=GetFuzzyColorDistance(source,destination);
  scale=1.0;
  distance=0.0;
  if (source->alpha_trait != UndefinedPixelTrait ||
      destination->alpha_trait != UndefinedPixelTrait)
    {
      /*
        Transparencies are involved - set alpha distance
      */
      pixel=GetPixelAlpha(source,p)-(double) GetPixelAlpha(destination,q);
      distance=pixel*pixel;
      if (distance > fuzz)
        return(MagickFalse);
      /*
        Generate a alpha scaling factor to generate a 4D cone on colorspace
        Note that if one color is transparent, distance has no color component.
      */
      if (source->alpha_trait != UndefinedPixelTrait)
        scale=QuantumScale*GetPixelAlpha(source,p);
      if (destination->alpha_trait != UndefinedPixelTrait)
        scale*=QuantumScale*GetPixelAlpha(destination,q);
      if (scale <= MagickEpsilon)
        return(MagickTrue);
    }
  /*
    RGB or CMY color cube
  */
  distance*=3.0;  /* rescale appropriately */
  fuzz*=3.0;
  pixel=GetPixelRed(source,p)-(double) GetPixelRed(destination,q);
  if ((source->colorspace == HSLColorspace) ||
      (source->colorspace == HSBColorspace) ||
      (source->colorspace == HWBColorspace))
    {
      /*
        Compute an arc distance for hue.  It should be a vector angle of
        'S'/'W' length with 'L'/'B' forming appropriate cones.
      */
      if (fabs((double) pixel) > (QuantumRange/2))
        pixel-=QuantumRange;
      pixel*=2;
    }
  distance+=scale*pixel*pixel;
  if (distance > fuzz)
    return(MagickFalse);
  pixel=GetPixelGreen(source,p)-(double) GetPixelGreen(destination,q);
  distance+=scale*pixel*pixel;
  if (distance > fuzz)
    return(MagickFalse);
  pixel=GetPixelBlue(source,p)-(double) GetPixelBlue(destination,q);
  distance+=scale*pixel*pixel;
  if (distance > fuzz)
    return(MagickFalse);
  return(MagickTrue);
}