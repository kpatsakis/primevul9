static inline ssize_t RoundToYCC(const double value)
{
  if (value <= 0.0)
    return(0);
  if (value >= 1388.0)
    return(1388);
  return((ssize_t) (value+0.5));
}