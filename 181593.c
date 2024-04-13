static inline ssize_t PixelRoundOffset(double x)
{
  /*
    Round the fraction to nearest integer.
  */
  if ((x-floor(x)) < (ceil(x)-x))
    return((ssize_t) floor(ConstrainPixelOffset(x)));
  return((ssize_t) ceil(ConstrainPixelOffset(x)));
}